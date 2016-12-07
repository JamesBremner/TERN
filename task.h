/**

  TERN event handler for task simulation

*/

#include <queue>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/variance.hpp>

namespace raven
{
namespace sim
{
namespace task
{

class cSource :  public tern::cEventHandler
{
    public:
    cSource( const std::wstring& name )
        : cEventHandler( name )
    {

    }

    /** Calculate delay before next arrival

    Defaults to 1 clock tick.  Override in real model

    */
    virtual int Delay()
    {
        return 1;
    }

    /** Construct next planet to arrive

    Overrride in real model

    */
    virtual tern::cPlanet * Construct()
    {
        return new tern::cPlanet( tern::theSimulationEngine );
    }
    void Start()
    {
        ScheduleNextArrival();
    }
    void ScheduleNextArrival()
    {
        tern::theSimulationEngine.Add(
            Construct(),
            1,                              // event type
            this,                           // handle the event here
            tern::theSimulationEngine.theTime + Delay() );
    }
    int Handle( tern::cEvent* e )
    {
        switch( e->myType )
        {

        case 1:
            tern::theSimulationEngine.Add(
                e->myPlanet,
                1,
                myDstID,
                tern::theSimulationEngine.theTime );
            ScheduleNextArrival();
            return 1;

        default:
            return 0;
        }
    }
};
/** Simulate a delay, storing planets in a queue until the delay is over */

class cDelay : public tern::cEventHandler
{
    std::queue < tern::cPlanet * > myQ;
    int myQMax;

public:
    cDelay(  const std::wstring& name )
        : cEventHandler( name )
        , myQMax( 0 )
    {

    }
    /** Calculate delay for a planet at the head of the queue

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

        // schedule completion
        tern::theSimulationEngine.Add(
            new tern::cPlanet( tern::theSimulationEngine ),
            2,
            this,
            tern::theSimulationEngine.theTime + delay
        );

    }
    /** Handle event */
    int Handle( tern::cEvent* e )
    {
        // switch on event
        switch( e->myType )
        {

        case 1:
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

        case tern::event_type_final_report:
            std::wcout << myName << " report: max Queue size " << myQMax << "\n";
            return 1;

        default:
            return 0;
        }

    }
};


///  Ultimate destination for packets

class cSink
    : public tern::cEventHandler
{
public:
    cSink(  const std::wstring& name )
        : cEventHandler( name )
    {

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

            // simulation over, report
            std::wcout << myName << " report: "
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

private:

    // statistics accumulator
    boost::accumulators::accumulator_set<int, boost::accumulators::stats<
    boost::accumulators::tag::min,
    boost::accumulators::tag::max,
    boost::accumulators::tag::mean,
    boost::accumulators::tag::variance,
    boost::accumulators::tag::count> >
    myAccumulator;

};


}
}
}

