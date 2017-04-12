

/**

  TERN event handler for task simulation

*/

#include <queue>

#include "stats.h"

namespace raven
{
namespace sim
{
namespace task
{

/** Simulate a delay, storing planets in a queue until the delay is over */

class cDelay : public tern::cEventHandler
{
    protected:
    std::queue < tern::cPlanet * > myQ;
    int myQMax;
    int myTotal;
    int myTotalLastPlot;
    stats::stats_t myRep;

public:
    cDelay(  const std::string& name )
        : cEventHandler( name )
        , myQMax( 0 )
        , myTotal( 0 )
        , myTotalLastPlot( 0 )
    {
        AddPlot( "MaxQ");
        AddPlot( "Total" );
    }

    /** Calculate delay for a planet at the head of the queue

    @return delay in clock ticks, -1 for infinite

    This defaults to 1 clock tick.  Override to specialize

    */
    virtual int Delay( tern::cPlanet * planet )
    {
        return 1;
    }

    /** Schedule completion for the planet at the head of the queue */
    void ScheduleCompletion()
    {
        // check we have something waiting on the queue
        if( ! myQ.size() )
            return;

        // calculate delay
        int delay = Delay( myQ.front() );

        // a negative delay causes the planet to be left on the queue
        if( delay < 0 )
            return;

        // schedule completion
        tern::theSimulationEngine.Add(
            new tern::cPlanet( tern::theSimulationEngine ),
            2,
            this,
            tern::theSimulationEngine.theTime + delay
        );

    }
    /** Handle event */
    virtual int Handle( tern::cEvent* e )
    {
        // base class handles some standard events
        if( cEventHandler::Handle( e ))
            return 1;

        // switch on event
        switch( e->myType )
        {

        case 1:
            // planet has arrived

            // add to queue
            myQ.push( e->myPlanet );

            // update queue length high water mark
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

            // increment total throughput
            myTotal++;

            // schedule completion of next planet on queue
            ScheduleCompletion();

            return 1;

        default:
            return 0;
        }

    }

    virtual void HandlePlotPointEvent()
    {
        //std::cout << "cDelay::HandlePlotPointEvent "  << myQMax << "\n";
        myPlot[1].myData.push_back( myQ.size() );
        myPlot[2].myData.push_back( myTotal - myTotalLastPlot );
        myTotalLastPlot = myTotal;
    }

    virtual void FinalReport()
    {
        std::cout << myName << " report: max Queue size " << myQMax << "\n";
        PlotOutput();
    }

    virtual void Clear()
    {
        cEventHandler::Clear();
        myQ = std::queue < tern::cPlanet * >();
        myQMax = 0;
    }

    virtual void SaveRunStatsToReplicationStats();

    virtual void ReplicationReport();
};


///  Ultimate destination for packets

class cSink
    : public tern::cEventHandler
{
public:
    cSink(  const std::string& name )
        : cEventHandler( name )
    {

    }

    virtual void Clear()
    {
        cEventHandler::Clear();
        myAccumulator = stats::stats_t();
    }

    int Handle( tern::cEvent* e )
    {
        // switch on event type
        switch ( e->myType )
        {
        case 1:

            // planet has arrived at sink

            // add to accumulated statistics
            myAccumulator( e->myPlanet->getLifetime() );

            // delete the planet
            tern::theSimulationEngine.Delete( e->myPlanet );

            // all done
            return 1;


        case tern::event_type_final_report:

            // single simulation run over, report
            std::cout << myName << " report: "
                      << "count:" << boost::accumulators::count(myAccumulator)
                      << " min:" << boost::accumulators::min(myAccumulator)
                      << " aver:" << (int)boost::accumulators::mean(myAccumulator)
                      << " max:" << boost::accumulators::max(myAccumulator)
                      << " std dev:" << (int)sqrt(boost::accumulators::variance(myAccumulator))
                      << "\n";
            return 1;

        default:
            return 0;
        }
    }

    virtual void SaveRunStatsToReplicationStats();

    virtual void ReplicationReport();

private:

    // run stats
    stats::stats_t myAccumulator;

    // replication stats
    stats::stats_t myRepCount;
    stats::stats_t myRepMin;
    stats::stats_t myRepAver;
    stats::stats_t myRepMax;
    stats::stats_t myRepDev;
};


}
}
}

