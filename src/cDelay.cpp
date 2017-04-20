#include <iostream>
#include <sstream>
#include "tern.h"
#include "cDelay.h"

namespace raven
{
namespace sim
{
namespace tern
{

int cDelay::Handle( tern::cEvent* e )
{
    if( cEventHandler::Handle( e ))
        return 1;

    // switch on event
    switch( e->myType )
    {

    case 1:
        if( cEventHandler::Handle( e ))
            return 1;
        // planet has arrived

        // add to queue
        myQ.push( e->myPlanet );

        if( (int)myQ.size() > myQMax )
            myQMax = myQ.size();

        // check if ready to start processing immediatly
        // i.e. new planet is at front of queue
        if( myQ.size() == 1 )
        {
            ScheduleCompletion();
        }
        return 1;

    case 2:

        // delay has completed

        // send planet onwards
        tern::theSimulationEngine.Add(
            myQ.front(),
            1,
            myDstID,
            tern::theSimulationEngine.theTime );

        // remove from queue
        myQ.pop();

        // schedule completion of next planet on queue
        ScheduleCompletion();

        return 1;


    default:
        return 0;
    }

}

void cDelay::ScheduleCompletion()
{
    // check we have something waiting on the queue
    if( ! myQ.size() )
        return;

    // calculate delay
    int delay = Delay( myQ.front() );
    if( delay == -1 )
        return;

    // schedule completion
    tern::theSimulationEngine.Add(
        new tern::cPlanet( tern::theSimulationEngine ),
        2,
        this,
        tern::theSimulationEngine.theTime + delay
    );

}
void cDelay::SaveRunStatsToReplicationStats()
{
    myRep( myQMax );
    myQMax = 0;
}

void cDelay::ReplicationReport()
{
    std::stringstream ss;
    ss << getName() << " replication report\n";
    ss << " Q max report: "
       << "count:" << boost::accumulators::count(myRep)
       << " min:" << boost::accumulators::min(myRep)
       << " aver:" << (int)boost::accumulators::mean(myRep)
       << " max:" << boost::accumulators::max(myRep)
       << " std dev:" << (int)sqrt(boost::accumulators::variance(myRep))
       << "\n";

    std::cout << ss.str();
}
std::string cDelay::FinalReportText()
{
    std::stringstream ss;
    ss << myName << " report: max Queue size " << myQMax << "\n";
    return ss.str();
}
}
}
}
