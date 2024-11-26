#define NOMINMAX

#include <windows.h>
#include <iostream>
#include <fstream>
#include <queue>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/min.hpp>

#include "tern.h"
#include "cVase.h"
#include "probability.h"
#include "cFunnel.h"
#include "cSource.h"
#include "cSink.h"
#include "model.h"


#include "raven_sqlite.h"

using namespace std;
namespace raven
{
namespace sim
{
namespace tern
{


class cSourceFlow
    : public tern::cEventHandler
{
public:
    /** Construct

    @param[in] name
    @param[in] mean  time between events

    */
    cSourceFlow( const raven::sim::gui::cFlower* f )
        : cEventHandler( f->getName() )
        , myTotal( 0 )
    {
        for ( auto it : myQuality )
        {
            std::cout <<"SourceFlow constructor " << it.first
                      <<" " << it.second
                      <<" "<< f->getValue( it.first ) << std::endl;

            myQuality.setValue( it.second, f->getValue( it.first ) );
        }
    }



    /// initialize simulation, automaticalled called by simulator
    void Start()
    {
        ScheduleArrival();
    }

    ///  Schedule next arrival
    void ScheduleArrival()
    {
        long long next_time = tern::theSimulationEngine.theTime;
        tern::cPlanet * planet;

        // continuous flow creates one planet every time unit
        next_time += 1;

        // create planet that will be launched on its travels when scheduled
        planet = new tern::cPlanet( tern::theSimulationEngine );

        planet->setQuality( myQuality );

        tern::theSimulationEngine.Add(
            planet,
            1,            // event type
            this,         // handle the event here
            next_time
        );
    }

    /// Handle an arrival, auto called by simulation
    int Handle( tern::cEvent* e )
    {
        // base class handles some standard events
        if( cEventHandler::Handle( e ))
            return 1;

        // Schedule the next arrival
        ScheduleArrival();

        // Send this arrival to the destination immediatly
        tern::theSimulationEngine.Add(
            e->myPlanet,
            1,
            myDstID,
            tern::theSimulationEngine.theTime );

        return 1;
    }
    void FinalReport()
    {
        cout << "SourceFlow " << myName;
        cout << " Total volume " << myTotal << endl;

    }

private:
    double myTotal;
    int myQualityIndexVolume;
    cQuality myQuality;
};
//
/////  Ultimate destination for planets
//
//class cSink
//    : public tern::cEventHandler
//{
//public:
//    cSink(  const std::string& name )
//        : cEventHandler( name )
//        , myVolume( 0 )
//        , myPlotVolume( 0 )
//    {
//        myQualityIndexVolume = cQuality::getIndex( "Volume");
//
//        // loop over defined qualities
//        for( auto q : myQuality )
//        {
//            // skip volume, already handled
//            if( q.first == "Volume" )
//                continue;
//
//            // Initial value of quality
//            myQuality.setValue( q.second, 0 );
//
//            // Add plot for quality
//            AddPlot( q.first );
//        }
//    }
//    /// Planet has arrived at sink
//    int Handle( tern::cEvent* e )
//    {
//        // base class handles some standard events
//        if( cEventHandler::Handle( e ))
//            return 1;
//
//        // report on planet's history
//        long long lifetime = e->myPlanet->getLifetime();
//        acc( lifetime );
//        cout << " Planet " << e->myPlanet->myID << " arrived at sink " << myName
//             << " at " << tern::theSimulationEngine.theTime << " after " << lifetime << endl;
//
//        // Add to total volume, just 0 if volume not specified
//        myVolume += e->myPlanet->getQuality(myQualityIndexVolume);
//
//        myQuality = e->myPlanet->Quality();
//
//        // delete the planet
//        tern::theSimulationEngine.Delete( e->myPlanet );
//
//        // all done
//        return 1;
//    }
//    void HandlePlotPointEvent()
//    {
//        // Plot the rate
//        myPlot[0].myData.push_back( ( myVolume - myPlotVolume ) / 2 );
//        myPlotVolume = myVolume;
//
//        // loop over  defined qualities
//        int index = 1;
//        for( auto q : myQuality )
//        {
//            // skip volume, already handled
//            if( q.first == "Volume" )
//                continue;
//            myPlot[ index++ ].myData.push_back( myQuality.getValue( q.second ));
//        }
//    }
//    void FinalReport()
//    {
//        cout << "Sink " << myName;
//
//        //  Output volume plot, if volume quality specified
//        if( myQualityIndexVolume >= 0 )
//        {
//            cout << " Total volume " << myVolume << endl;
//            PlotOutput();
//        }
//        else
//        {
//            if(  boost::accumulators::count( acc ) == 0 )
//            {
//                wcout << " No arrivals" << endl;
//            }
//            else
//            {
//                wcout << " arrivals: " << boost::accumulators::count( acc )
//                      << " min lifetime " << boost::accumulators::min( acc )
//                      << " mean lifetime " << boost::accumulators::mean( acc )
//                      << " max lifetime " << boost::accumulators::max( acc ) << endl;
//            }
//        }
//
//    }
//private:
//    boost::accumulators::accumulator_set<long long,
//    boost::accumulators::stats<boost::accumulators::tag::mean,
//    boost::accumulators::tag::max, boost::accumulators::tag::min,
//    boost::accumulators::tag::count > > acc;
//    double myVolume;
//    double myPlotVolume;
//    int myQualityIndexVolume;
//    cQuality myQuality;
//};

/**  Delay planets in their progress for some time

This is like a conveyor belt, many planets can be delayed in sequence

*/

class cDelay_Flow
    : public tern::cEventHandler
{
public:
    /** Construct

    @param[in] name
    @param[in] mean  delay time

    */
    cDelay_Flow(  const std::string& name, double mean  )
        : cEventHandler( name )
        , myMean( mean )
    {
    }
    /// Planet has arrived at delay
    int Handle( tern::cEvent* e )
    {
        switch( e->myType )
        {
        case event_type_final_report:
            break;
        default:

            // schedule arrival at next event handler after specified mean delay
            tern::theSimulationEngine.Add(
                e->myPlanet,
                1,
                myDstID,                                                               // next event handler
                tern::theSimulationEngine.theTime + (__int64)raven::sim::prob::cPoisson::ran( myMean )  // random time from now with specified mean
            );
        }
        return 1;
    }
private:
    double myMean;
};

/**  Passes input onto output


*/

class cGeneric
    : public tern::cEventHandler
{
public:
    /** Construct

    @param[in] name


    */
    cGeneric(  const std::string& name  )
        : cEventHandler( name )

    {
    }
    /// Planet has arrived at delay
    int Handle( tern::cEvent* e )
    {
        switch( e->myType )
        {
        case event_type_final_report:
            break;
        default:

            // schedule arrival at next event handler
            tern::theSimulationEngine.Add(
                e->myPlanet,
                1,
                myDstID,                                                               // next event handler
                tern::theSimulationEngine.theTime  // random time from now with specified mean
            );
        }
        return 1;
    }
private:
    double myMean;
};


class cVessel
    : public tern::cEventHandler
{
public:
    /** Construct

    @param[in] name
    @param[in] capacity

    */
    cVessel(  const std::string& name, double capacity  )
        : cEventHandler( name )
        , myCapacity( capacity )
        , myLevel( 0 )
        , full_time( 0 )
    {
        myQualityIndexVolume = cQuality::getIndex( "Volume");
        if( myQualityIndexVolume < 0  )
        {
            tern::theSimulationEngine.HandleFatalError(L"ERROR: Vessel cannot find Volume quality");
        }
    }
    /// Planet has arrived at handler
    int Handle( tern::cEvent* e )
    {

        // base class handles some standard events
        if( cEventHandler::Handle( e ))
            return 1;

        // add arriving flow to level in vessel
        myLevel += e->myPlanet->getQuality( myQualityIndexVolume );

        // no output, so we are done with the input
        tern::theSimulationEngine.Delete( e->myPlanet );

        // if vessel has become full, record time
        if( myLevel > myCapacity && full_time == 0)
        {
            full_time = tern::theSimulationEngine.theTime;
        }

        // Excess input just vanishes
        if( myLevel > myCapacity )
            myLevel = myCapacity;


        return 1;
    }
    void HandlePlotPointEvent()
    {
        myPlot[0].myData.push_back( myLevel );
    }
    void FinalReport()
    {
        cout << "Vessel " << myName;
        if( full_time == 0 )
            cout << " Partially filled at " << myLevel << endl;
        else
            cout << " Filled up at time " << full_time << endl;
        PlotOutput();
    }
private:
    double myCapacity;
    double myLevel;
    long long full_time;
    int myQualityIndexVolume;
};

class cBusy
    : public tern::cEventHandler
{
public:
    /** Construct

    @param[in] name
    @param[in] mean  delay time

    */
    cBusy(  const std::string& name, double mean  )
        : cEventHandler( name )
        , myMean( mean )
        , myNextFree( 0 )
    {
    }
    /// Planet has arrived at handler
    int Handle( tern::cEvent* e )
    {
        if( cEventHandler::Handle( e ) )
            return 1;

        switch( e->myType )
        {

        default:
            // check if we are busy
            if( tern::theSimulationEngine.theTime < myNextFree )
            {
                cout << "Error: Collision at Busy " << myName
                     << " time is " << tern::theSimulationEngine.theTime
                     << " but busy till " << myNextFree << endl;
                return 0;
            }
            // schedule arrival at next event handler after specified mean delay
            myNextFree = tern::theSimulationEngine.theTime + (__int64)raven::sim::prob::cPoisson::ran( myMean );  // random time from now with specified mean
            //wcout << "Busy " << myName << " busy from " << tern::theSimulationEngine.theTime << " until " << myNextFree << endl;
            tern::theSimulationEngine.Add(
                e->myPlanet,
                1,
                myDstID,                                                               // next event handler
                myNextFree
            );
        }
        return 1;
    }
    long long getNextFree()
    {
        return myNextFree;
    }
private:
    double myMean;
    long long myNextFree;
};


class cQueue
    : public tern::cEventHandler
{
public:
    cQueue(  const std::string& name )
        : cEventHandler( name )
        , myMax( 0 )
    {
    }
    void Start()
    {
        myBusy = (cBusy*)tern::theSimulationEngine.Find( myDstID );
    }

    void DumpQueue()
    {
        cout << "Q is ";
        std::queue < tern::cPlanet* > temp;
        while ( myQ.size() )
        {
            cout << myQ.front()->myID << " ";
            //    << " ( " << myQ.front() << " " <<  myQ.front()->myPlanet << " ) ";
            temp.push( myQ.front( ) );
            myQ.pop();
        }
        cout << endl;
        myQ = temp;
    }


    /// Planet has arrived at handler
    int Handle( tern::cEvent* e )
    {
//        cout << "->Q Handle: ";
//        DumpQueue();

        if( cEventHandler::Handle( e ) )
            return 1;

        switch ( e->myType )
        {
        case 1:

            cout << "Q arrival " << e->myPlanet->myID << " at " <<tern::theSimulationEngine.theTime << " Q size " << myQ.size() << endl;

            // Add planet to the queue
            myQ.push( e->myPlanet );

            if( myQ.size() > 1 )
            {
                // there were planets already waiting on queue
                // so this one will just have to wait its turn
//                cout << "waiting"  << endl;
//                DumpQueue();
                if( myQ.size() > myMax  )
                    myMax = myQ.size();

                break;
            }

            // nothing ahead of this planet, check if busy
            if( myBusy->getNextFree() > tern::theSimulationEngine.theTime )
            {
                // busy
                // schedule next pop for when the busy handler becomes free again
                //cout << "sched pop #1 for " << myQ.front()->myID << endl;
                tern::theSimulationEngine.Add(
                    myQ.front(),
                    2,
                    this,
                    myBusy->getNextFree() + 1
                );
//               DumpQueue();
                break;
            }

            // not busy, so schedule pop immediatly
            //cout << "sched pop #2 for " << e->myPlanet->myID << endl;
            tern::theSimulationEngine.Add(
                e->myPlanet,
                2,
                this,
                tern::theSimulationEngine.theTime );
//           DumpQueue();
            break;

            cout << "ERROR";

        case 2:
//            cout << "Q pop " << e->myPlanet->myID << " at " <<tern::theSimulationEngine.theTime
//                 << " Q size " << myQ.size() << endl;
//            cout << "front is " << myQ.front()->myID << endl;

            // pop planet waiting at front of queue
            myQ.pop();

            // handle event immediatly
            // ( neccessary so as to update nextfree time on busy event handler )
            e->myType = 1;
            myBusy->Handle( e );

            // check if more events waiting
            if( myQ.size() )
            {
                // schedule next pop for when the busy handler becomes free again
                //cout << "sched pop #3 for " << myQ.front()->myID << endl;
                tern::theSimulationEngine.Add(
                    myQ.front(),
                    2,
                    this,
                    myBusy->getNextFree() + 1
                );
            }
            break;

        default:
            cout << "ERROR: Unrecognized event type" << endl;
            break;
        }
//        cout << "<-Q Handle: ";
//        DumpQueue();
        return 1;
    }

    void FinalReport()
    {
        cout << "Queue " << myName;
        cout << " Max length " << myMax << endl;
    }
private:
    cBusy * myBusy;
    std::queue < tern::cPlanet* > myQ;
    int myMax;
};

void cFunnel::FinalReport()
{
    cout << "Funnel " << myName;
    cout << " Total Volume handled " << myTotalInput << " ";
    if( full_time == 0 )
        cout << " Partially filled at " << myLevel << endl;
    else
        cout << " Filled up at time " << full_time << endl;

    PlotOutput();

}


/**  Load simulation engine from process model in vase

@param[in/out] theSimulationEngine
@param[in] the Vase

*/
void Load( cTERN& theSimulationEngine,
           raven::sim::gui::cVase& theVase )
{
    using namespace raven::sim::gui;


    //myTasks.clear();

    // construct the event handlers for each flower in the vase
    for( const auto f : theVase )
    {
        cout << f->getName() << endl;

        if( f->getType() == cFlowerFactory::Index("Source") )
            new tern::cSource( f );

        else if( f->getType() == cFlowerFactory::Index("SourceFlow") )
            new tern::cSourceFlow( f );

        else if( f->getType() == cFlowerFactory::Index("Sink") )
            new tern::cSink( f->getName() );

        else if( f->getType() == cFlowerFactory::Index("Base") )
            new tern::cGeneric( f->getName() );

        else if( f->getType() == cFlowerFactory::Index("Delay") )
            new tern::cDelay_Flow( f->getName(),  f->getValue("Mean") );

        else if( f->getType() == cFlowerFactory::Index("Busy") )
            new tern::cBusy( f->getName(),  f->getValue("Mean") );

        else if( f->getType() == cFlowerFactory::Index("Queue") )
            new tern::cQueue( f->getName() );

        else if( f->getType() == cFlowerFactory::Index("Vesse") )
            new tern::cVessel( f->getName(), f->getValue("Volume") );

        else if( f->getType() == cFlowerFactory::Index("Funnel") )
            new tern::cFunnel( f );

        else if( f->getType() == cFlowerFactory::Index("PipeBend") )
            // pipebends are ignored
            ;

        // it isn't one of the standard flowers
        // so lets try construction one of the special model flowers
        else if( ! ConstructModelFlowers( f ) )
        {

            int dbg = (int) f->getType();
            int dbg2 = 0;

            tern::theSimulationEngine.HandleFatalError(
                "Loading tern, Unrecognized type for flower named " + f->getName() );
        }
    }

    // connect the event handlers as specified by the vase
    for( auto sf : theVase )
    {
        if( sf->getType() == cFlowerFactory::Index("PipeBend") )
            continue;

        cFlower* df = sf->getDestination();
        if( df )
        {
            theSimulationEngine.Connect( sf->getName(), df->getName() );
        }
        df = sf->getDestination2();
        if( df )
        {
            theSimulationEngine.Connect2( sf->getName(), df->getName() );
        }
    }

    cout << "Loaded " << theSimulationEngine.getHandlerCount() << " event handlers" << endl;
}



void ReadDB()
{
    cout << "tern::ReadDB()" << endl;

    raven::sqlite::cDB db;
    db.Open("vase.dat");
    db.Query("SELECT type, time, plot_points FROM params;");
    if( db.myError )
    {
        tern::theSimulationEngine.HandleFatalError(
            std::string("ERROR reading parameter database: ") + db.myError);
    }
    tern::theSimulationEngine.myType =
        ( tern::cTERN::etype ) ( strtol( db.myResultA[0].c_str(),NULL,10) );
    tern::theSimulationEngine.myStopTime =
        ( strtol( db.myResultA[1].c_str(),NULL,10) );
    tern::theSimulationEngine.myPlotPoints =
        ( strtol( db.myResultA[2].c_str(),NULL,10) );

    db.Query("SELECT * FROM quality_names;");
    cQuality::setNames( db.myResultA );

}

}
}
}
int main()
{
    // redirect standard out to log file
    freopen("tern.log","w",stdout);

    // construct a flower factory
    // this will iniialize the flower dictionary
    raven::sim::gui::cFlowerFactory FlowerFactory;

    // read the database
    raven::sim::tern::ReadDB();

    // read the model as defined by the GUI
    raven::sim::gui::cVase theVase;
    theVase.Read("vase.dot");
    cout << theVase.getFlowerCount() << " flowers read" << endl;
    if( ! theVase.getFlowerCount())
    {
        return 0;
    }

    // Load simulation with process model in vase
    raven::sim::tern::Load( raven::sim::tern::theSimulationEngine, theVase );

    try
    {

        // Start simulation run
        raven::sim::tern::theSimulationEngine.Run();

    }

    catch( std::runtime_error& e )
    {
        raven::sim::tern::theSimulationEngine.HandleFatalError( e.what() );
    }


    return 0;
}
