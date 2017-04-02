#include <iostream>
#include "terntime.h"
#include "nlohmann_json.hpp"
#include "tern.h"
#include "task.h"


using namespace std;
using namespace raven::sim;

#include "cStoppingMachine.h"

time::date_t cStoppingMachine::theEarliestStop /* = f1Time("13-03-2050 06:20:00") */;

cStoppingMachine::cStoppingMachine(
    const string& name,
    const string& stopschedule )
    : raven::sim::task::cDelay( name )
{
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
                    time::f1Time( stop["stop_time"] ) );
                myStart.push_back(
                    time::f1Time( stop["start_time"] ) );
            }
        }
    }

    if( myStop.size() )
    {
        if( theEarliestStop == time::date_t() )
            theEarliestStop = myStop[0];
        else if( myStop[0] < theEarliestStop )
            theEarliestStop = myStop[0];
    }
}

void cStoppingMachine::Start()
{
    const int time_to_first_stop = 10;

    using namespace std::chrono;

    if( ! myStop.size() )
        return;

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

int cStoppingMachine::Delay( tern::cPlanet * planet )
{
    for( int kstop = 0; kstop < (int)myStop.size(); kstop++ )
    {
        if( myStopInSimTime[kstop] == tern::theSimulationEngine.theTime &&
                tern::theSimulationEngine.theTime < myStartInSimTime[ kstop ] )
        {
            int delay = myStartInSimTime[ kstop ] - tern::theSimulationEngine.theTime;
            //cout << myStartInSimTime[ kstop ] << " " << tern::theSimulationEngine.theTime << "\n";
            cout << getName() << " delay at " << tern::theSimulationEngine.theTime << " for " <<delay << "\n";
            return delay;
        }
    }
    return 1;
}

