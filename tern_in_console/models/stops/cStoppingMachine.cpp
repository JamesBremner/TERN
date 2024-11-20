#include <iostream>
#include "terntime.h"
#include "json.hpp"
#include "tern.h"
#include "cDelay.h"
#include "probability.h"


using namespace std;
using namespace raven::sim;

#include "cStoppingMachine.h"

tern::date_t cStoppingMachine::theEarliestStop;

cStoppingMachine::cStoppingMachine(
    const string& name,
    const string& stopschedule )
    : raven::sim::tern::cDelay( name )
    , myfHistorical( true )
{
    myTypeName = "StoppingMachine";

    nlohmann::json J = nlohmann::json::parse( stopschedule );
    // cout << J.dump(2) << "\n====\n";

    // loop over machines
    for( auto& machine : J["machines"] )
    {
        // are these the specs for this machine
        if( machine["id"] == getName() )
        {
            //cout << machine.dump(2) << "\n";

            // loop over stops
            for( auto& stop : machine["stops"] )
            {
                myStop.push_back(
                    tern::f1Time( stop["stop_time"] ) );
                myStart.push_back(
                    tern::f1Time( stop["start_time"] ) );
            }
        }
    }

    if( myStop.size() )
    {
        if( theEarliestStop == tern::date_t() )
            theEarliestStop = myStop[0];
        else if( myStop[0] < theEarliestStop )
            theEarliestStop = myStop[0];
    }
}

cStoppingMachine::cStoppingMachine(
    const string& name,
    int MeanSecsBetweenStops,
    int MeanSecsStopDuration,
    int DevSecsStopDuration,
    int Threshold )
    : raven::sim::tern::cDelay( name )
    , myfHistorical( false )
    , myMeanSecsBetweenStops( MeanSecsBetweenStops )
    , myMeanSecsStopDuration( MeanSecsStopDuration )
    , myDevSecsStopDuration( DevSecsStopDuration )
    , myThreshold( Threshold )
    , myTotalStoppedTime( 0 )
    , myTotalBlockedTime( 0 )
{
    myTypeName = "StoppingMachine";
}

void cStoppingMachine::Start()
{
    const int time_to_first_stop = 10;

    using namespace std::chrono;

    if( myfHistorical )
    {
        if( ! myStop.size() )
            return;

        tern::theSimulationEngine.Start( theEarliestStop );

        int dtFirtStop = theEarliestStop.time_since_epoch().count() * system_clock::period::num / system_clock::period::den;

        for( auto& stop : myStop )
        {
            myStopInSimTime.push_back(
                stop.time_since_epoch().count() * system_clock::period::num / system_clock::period::den
                - dtFirtStop + time_to_first_stop );

            //cout << dtFirtStop << " " << myStopInSimTime.back() << "\n";
        }
        for( auto& start : myStart )
        {
            myStartInSimTime.push_back(
                start.time_since_epoch().count() * system_clock::period::num / system_clock::period::den
                - dtFirtStop + time_to_first_stop );
        }
    }
    else
    {
        myNextStop = raven::sim::prob::cPoisson::ran( myMeanSecsBetweenStops );
    }

}

int cStoppingMachine::Delay( tern::cPlanet * planet )
{
    if ( ! IsOutputClear() )
    {
        // output blocked

        cout << getName() << " output blocked at " << tern::theSimulationEngine.theTime << "\n";

        myTimeStartBlocked = tern::theSimulationEngine.theTime;

        // schedule next check for output cleared
        tern::theSimulationEngine.Add(
            new tern::cPlanet( tern::theSimulationEngine ),
            3,
            this,
            tern::theSimulationEngine.theTime + 1
        );

        // no delay can be determined at this point
        return -1;
    }

    if( myfHistorical )
    {
        // using an historical stopping schedule

        for( int kstop = 0; kstop < (int)myStop.size(); kstop++ )
        {
            if( myStopInSimTime[kstop] == tern::theSimulationEngine.theTime &&
                    tern::theSimulationEngine.theTime < myStartInSimTime[ kstop ] )
            {
                int delay = myStartInSimTime[ kstop ] - tern::theSimulationEngine.theTime;
                //cout << myStartInSimTime[ kstop ] << " " << tern::theSimulationEngine.theTime << "\n";
                //cout << getName() << " delay at " << tern::theSimulationEngine.theTime << " for " <<delay << "\n";
                cout << getName() << " delay at " << tern::theSimulationEngine.Calendar() << " for " <<delay << "\n";
                return delay;
            }
        }
        return 1;
    }
    else
    {
        // using a randomly generated schedule
        if( tern::theSimulationEngine.theTime >= myNextStop )
        {
            if( tern::theSimulationEngine.ConsoleLog() )
                cout << myNextStop <<" " << getName() << " stopped \n";

            // normally distributed stop duration
            int delay = raven::sim::prob::cNormal::ran( myMeanSecsStopDuration, myDevSecsStopDuration );

            myTotalStoppedTime += delay;

            // exponentially distributed run time to next stop
            myNextStop = tern::theSimulationEngine.theTime + delay
                         + raven::sim::prob::cPoisson::ran( myMeanSecsBetweenStops );

            //cout << getName() << " delay at " << tern::theSimulationEngine.theTime << " for " <<delay << "\n";

            return delay;

        }

        return 1;

    }
}

int cStoppingMachine::Handle( tern::cEvent* e )
{
    if( e->myType == 3 )
    {
        // output was blocked, check for clearance
        if( ! IsOutputClear() )
        {
            //still blocked, schedule another check
            tern::theSimulationEngine.Add(
                new tern::cPlanet( tern::theSimulationEngine ),
                3,
                this,
                tern::theSimulationEngine.theTime + 1
            );
            return 1;
        }

        // the output has cleared
        cout << getName() << " output cleared at " << tern::theSimulationEngine.theTime << "\n";
        myTotalBlockedTime += tern::theSimulationEngine.theTime - myTimeStartBlocked;
        // schedule completion of next planet on queue
        ScheduleCompletion();


    }

    // handle other events with cDelay handler
    return cDelay::Handle( e );


}

bool cStoppingMachine::IsOutputClear()
{
    // check size of buffer in output machine
    cStoppingMachine * dst = (  cStoppingMachine * ) tern::theSimulationEngine.Find( myDstID );
    if( dst == NULL )
        return true;
    if( dst->TypeName() != "StoppingMachine" )
        return true;

    if ( dst->CurrentQLength() < myThreshold )
        cout << getName() << " output clear at " << tern::theSimulationEngine.theTime << "\n";
    else
        cout << getName() << " output blocked at " << tern::theSimulationEngine.theTime << "\n";

    return ( dst->CurrentQLength() < myThreshold );
}

std::string cStoppingMachine::FinalReportText()
{
    std::stringstream ss;
    ss << myName << " report: max Queue size " << myQMax
        << " Stopped Time " << myTotalStoppedTime
        << " Blocked Time " << myTotalBlockedTime
        << "\n";
    return ss.str();
}
