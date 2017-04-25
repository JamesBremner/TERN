#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
#include <functional>
#include <algorithm>
using namespace std;
#include "raven_sqlite.h"
#include "tern.h"
#include "probability.h"

namespace raven
{
namespace sim
{
namespace tern
{

typedef std::multiset < cEvent, std::less<long long> >::iterator event_iter;

// Construct the simulation engine
cTERN theSimulationEngine;

// Construct the next IDs
int cEventHandler::nextID = 1;
int cPlanet::nextID = 1;


/*
#ifndef For_each
#define For_each( container, functor ) \
	std::for_each( container.begin(), container.end(), functor )
#endif
*/
/**

Run the simulation.

*/
void cTERN::Run()
{
    // use up first rand number from generator
//    cPoisson::ran( 10 ) ;

    // start at the begining of time
    theTime = 0;

    // clear the schedule
    myEventQueue.clear();

    // start each handler
    Start();

    // schedule plot point saves
    AddPlotPoints();

    //  continue until requested time elapsed
    while( theTime < myStopTime )
        handle_next_event();

    // Save final plot points
    for( auto& h : myHandlers )
    {
        h->HandlePlotPointEvent();
    }

    // generate final report
    FinalReport();
}

/**
Schedule an event.

@param[in] planet       pointer to event recorder
@param[in] type			event type
@param[in] ObjectID		ID object event happens to, may be null
@param[in] time			time of event

An event type of 0 will stop the simulation.
Any other type is passed on for use by the event handlers

*/
void cTERN::Add(
    cPlanet* planet,
    int type,
    int ObjectID,
    long long time )
{
    cEvent e;
    e.myPlanet = planet;
    e.myType = type;
    e.myHandler = Find( ObjectID );
    e.myTime = time;
    myEventQueue.insert( e );
    planet->Add( e );

    if( fConsolLog )
    {
        cout << "Planet " << planet->myID
              << " scheduled for " << e.myHandler->getName()
              << " at " << e.myTime << endl;
    }
    // DumpQueue();
}
void cTERN::Add(
    cPlanet* planet,
    int type,
    cEventHandler* handler,
    long long time )
{
    cEvent e;
    e.myPlanet = planet;
    e.myType = type;
    e.myHandler = handler;
    e.myTime = time;
    myEventQueue.insert( e );
    planet->Add( e );

    if( fConsolLog )
    {
        cout << "Planet " << planet->myID
              << " scheduled for " << e.myHandler->getName()
              << " at " << e.myTime << endl;
    }
    //DumpQueue();
}

void cTERN::AddPlotPoints()
{
    int plot_inc = myStopTime / myPlotPoints;
    for( int k = 0; k <= myStopTime; k += plot_inc )
    {
        for( auto& h : myHandlers )
        {
            Add( new cPlanet( *this ), event_type_plot_point, h, k);
        }
    }
}

/**
Schedule next Poisson event

@param[in] e		previous event
@param[in] mean	average delay before next event

@return time of new event

Schedules an event with the same type and object as previous
Occurring after a Poisson delay after the previous event

*/
long long cTERN::NextPoisson( cEvent& e, int mean )
{
    e.myTime += (long long)raven::sim::prob::cPoisson::ran( (double) mean );
    myEventQueue.insert( e );
    return e.myTime;
}

/// start each handler

void cTERN::Start()
{
    if( myHandlers.size() == 0 )
    {
        tern::theSimulationEngine.HandleFatalError(
            L"ERROR: No event handlers registered\n");
    }

    // loop over handlers, starting each one
    for( auto h : myHandlers )
    {
        h->Start();
    }

    if( myEventQueue.size() == 0 )
    {
        tern::theSimulationEngine.HandleFatalError(
            L"ERROR: No events required\n");
    }

}
/**  Request final report from handlers

Send an event of type 999 to every handler

*/
void cTERN::FinalReport()
{
    freopen("tern_final_report.txt","w",stdout);
    cout << "Final Report:" << endl;
    cEvent e;
    e.myType = event_type_final_report;
    for( cEventHandler* h : myHandlers )
    {
        h->Handle( &e );
    }
}

/** Writes message to final report and exits the simulation */
void cTERN::HandleFatalError( const wstring& msg )
{
    freopen("tern_final_report.txt","w",stdout);
    cout << "FATAL ERROR:" << endl;
    wcout << msg << endl;
    exit(1);

}
void cTERN::HandleFatalError( const string& msg )
{
    freopen("tern_final_report.txt","w",stdout);
    cout << "FATAL ERROR:" << endl;
    cout << msg << endl;
    exit(1);

}
/**
Handle next event

*/
void cTERN::handle_next_event()
{
    // get next event from the event queue
    cEvent* e = getNextEvent();

    // handle event according to type
    switch( e->myType )
    {

    case 0:
        printf("No more events\n");
        theTime = myStopTime;
        return;

    default:

//        if ( e->myType = 998 )
//        {
//        cout << "handle_next_event "
//            << e->myType << " "
//            << e->myHandler->getName() << "\n";
//        }

        e->myHandler->Handle( e );
        break;

    }

}
cEventHandler * cTERN::Find( int id )
{
    cEventHandler target( id );
    handler_iter ph = myHandlers.find( &target );
    if( ph == myHandlers.end() )
    {
        throw std::runtime_error("Cannot find event handler");
    }
    return *ph;
}

cEventHandler * cTERN::Find( const std::string& name )
{
    for( auto& h : myHandlers )
    {
        if( h->getName() == name )
        {
            return h;
        }
    }
    return 0;
}

void cTERN::Connect( const std::string& src,
                     const std::string& dst )
{
    cEventHandler* psrc = Find( src );
    if( ! psrc )
    {
        HandleFatalError( "ERROR: Cannot find " + src );
    }
    cEventHandler* pdst = Find( dst );
    if( ! pdst )
    {
        HandleFatalError( "ERROR: Cannot find " + dst );
    }
    psrc->setDestination( pdst );
}
void cTERN::Connect2( const std::string& src,
                      const std::string& dst )
{
    cEventHandler* psrc = Find( src );
    if( ! psrc )
    {
        HandleFatalError( "ERROR: Cannot find " + src );
    }
    cEventHandler* pdst = Find( dst );
    if( ! pdst )
    {
        HandleFatalError( "ERROR: Cannot find " + dst );
    }
    psrc->setDestination2( pdst );
}

/**

Get next scheduled event.

@return  pointer to next scheduled event.

The simulation time is advanced to the event time.
The event is removed from the event queue.

The event will be over-written when the next scheduled
is pulled from the event queue.
*/
cEvent* cTERN::getNextEvent()
{
    static cEvent currentEvent;

    //first scheduled event
    event_iter pe = myEventQueue.begin();

    if( pe == myEventQueue.end() )
    {
        // schedule is empty
        currentEvent.myType = 0;
        return &currentEvent;
    }


    // move event from queue
    currentEvent = *pe;
    myEventQueue.erase( pe );

    // increment clock
    theTime = currentEvent.myTime;

    //DumpQueue();

    return &currentEvent;
}

void cTERN::DumpQueue()
{
    printf("event Q\n"
           "P  T  S\n");
    for( event_iter event = myEventQueue.begin();
            event != myEventQueue.end(); event++ )
    {
        printf("%d %d %d ( %x %x )\n",
               event->myPlanet->myID, event->myType, event->myHandler->getID(),
               &(*event), event->myPlanet);
    }
}
/**

  Delete an event recorder

  @param[in] planet pointer to event recorder to be deleted

*/
void cTERN::Delete( cPlanet* planet )
{
    std::vector <cPlanet*>::iterator p =
        std::find( myPlanets.begin(), myPlanets.end(), planet );
    if( p == myPlanets.end() )
        return;
    delete *p;
    myPlanets.erase( p );

}
void cTERN::Add( cEventHandler& handler )
{
    /* A pointer to the handler is stored in a set.
    The set is sorted by the handler ID, so that it can be quickly looked up
    The set stores pointers to the handlers, rather than a copy of the handler
    so that application classes can override the handler method called by the
    simulation code.
    */

    myHandlers.insert( &handler );
}



cEventHandler::cEventHandler( const std::string& name )
    : myName( name )
{
    myID = nextID++;		                // unique ID for this handler
    theSimulationEngine.Add( *this );		// inform simulator this handler exists
}

void cEventHandler::Start()
{
    cout << "empty start called for " << myName << endl;
}

/**  Handle standard events

  @param[in] e the event
  @return 1 if the event handled, otherwise 0

  Calls virtual functions to handle the standard events.
  For the base class, these are no-ops.  If a specialized class needs to handle
  these events, it should over-ride the standard event handlers
*/
int cEventHandler::Handle( cEvent* e )
{
    switch( e->myType )
    {
    case event_type_plot_point:
        HandlePlotPointEvent();
        return 1;
    case event_type_final_report:
        FinalReport();
        return 1;
    default:
        break;
    }
    return 0;
}

void cEventHandler::PlotOutput()
{
//    if( ! myPlot[0].size())
//        return;

    raven::sqlite::cDB db;
    db.Open("vase.dat");
    db.Query("DELETE FROM plot WHERE flower = '%s';",
            myName.c_str() );

    // loop over plots
    for( auto& vplot : myPlot )
    {
        // loop over data points
        stringstream ss;
        for ( auto p : vplot.myData )
        {
            // space separated values
            ss << p << " ";
        }

        // write to database
        db.Query("INSERT INTO plot VALUES ( '%s', '%s', '%s' );",
                 myName.c_str(),
                 vplot.myName.c_str(),
                 ss.str().c_str() );
    }


}
cPlanet::cPlanet( cTERN& sim )
{
    myID = nextID++;		// unique ID
    //cout << "Constructed planet " << myID << endl;
    sim.Add( *this );		// inform simulator this planet exists
}

	void cPlanet::setQuality( int i, double v ) { myQuality.setValue( i, v ); }
	void cPlanet::setQuality( const cQuality& q) { myQuality = q; }
	double cPlanet::getQuality( int i ) const { return myQuality.getValue( i ); }
	cQuality& cPlanet::Quality() { return myQuality; }

};


}
}


/**************************************************************************
//   Designed and coded by James Bremner, Raven's Point Consulting,
//   james@ravenspoint.com


*************************************************************************/
