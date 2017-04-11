#pragma once

#include <vector>
#include <set>
#include <map>
#include <string>

#include "cQuality.h"

namespace raven {
namespace sim {
namespace tern {

	class cPlanet;
	class cTERN;
	class cEventHandler;

	const int event_type_plot_point   = 998;
	const int event_type_final_report = 999;

/**

  the events

  An event is constructed in the method vTERN::Add(), copied onto the event queue and detroyed
  When the event reaches the front of the event queue it is copied into the current event ( overwriting the previous current event ) and destroyed

*/
class cEvent
{
public:

	long long myTime;				///< scheduled time of event
	int myType;						///< event type

	cEventHandler* myHandler;		///< object that handles the event
	cPlanet* myPlanet;				///< object that records the event

	cEvent()
	{
	}

	/// allows event queue to sort events by scheduled time
	operator const long long & () const { return myTime; }
};

/**

  Base class that records an event as it wanders through the simulation



  */
class cPlanet {
public:
	int myID;
	std::vector< cEvent > myRecord;

	cPlanet( cTERN& sim );
	/**

	Add event record

	@param[in] the event to add to the record

	*/
	void Add( cEvent& e )
	{
		//assert( e.myTime >= 0 );
		myRecord.push_back( e );
	}
	/**

	Time duration between first and last event record

	@return duration

	*/
	long long getLifetime()
	{
		return myRecord.back().myTime - myRecord.front().myTime;
	}

	void setQuality( int i, double v );
	void setQuality( const cQuality& q);
	double getQuality( int i ) const;
	cQuality& Quality();

private:
	static int nextID;				///< ID of next planet created
	cQuality myQuality;

};

struct sPlot
{
    std::string            myName;
    std::vector<double>     myData;
};

/**

  Base class for an event handler.

  Inherit this into any class which needs to respond to events

*/
class cEventHandler {

	static int          nextID; 	///< ID of next event handler created

protected:
	int                 myID;		///< event handler ID, used to identify events for this handler
	std::string         myName;     ///< name of handler
	int                 myDstID;    ///< where planets go next from port 1
	int                 myDstID2;   ///< where planets go next from port 2
    std::vector <sPlot> myPlot;     ///< simulation results plot values

public:

	/**

	Constructor - lets the simulator know this handler exists

	@param[in] sim  the event simulator

	*/
	cEventHandler( const std::string& name );

	/**

	Constructor - don't register this handler with simulator

	@param[in] id the id we want for this handler

	This is used as a temporary target to search for the registered handler wirth the same ID

	*/
	cEventHandler( int id )
		: myID( id )
	{}

	/** Add plot
        Constructs empty vector to hold plot data
        and adds to vector of plots.
    */
	void AddPlot( const std::string& name )
	{
	    sPlot plot;
	    plot.myName = name;
        myPlot.push_back( plot );
	}

	/**
	  called exactly once at the start of every simulation run.

	  Over-ride to schedule any initial events

	*/
	virtual void Start();


	/**
	 called each time an event occurs.

	 @param[in] e  pointer to the event

	 Over-ride to check and handle appropriate events

	 The event pointed to only remains valid until the
	 next event occurs.

	*/
	virtual int Handle( cEvent* e );

	virtual void HandlePlotPointEvent() { /*std::cout << "Empty HandlePlotPointEvent for " << myName << "\n"; */ }
	virtual void FinalReport() {}
	void PlotOutput();

	int getID() const { return myID; }
	std::string getName() const { return myName; }

	bool operator<(const cEventHandler other) const { return myID < other.myID; }
	bool operator==(const cEventHandler other) const { return myID == other.myID; }

	/**

	Compare pointers to handlers by ID

	This functor is used to sort the handlers in the set that stores them

	*/
	struct less {
		bool operator()(const cEventHandler* l, const cEventHandler* r) {
			return l->myID < r->myID; }
	};

	// disable copies
	cEventHandler(const cEventHandler&); // not defined
    cEventHandler& operator=(const cEventHandler&); // not defined

    /**  Specify where events go next from port 1

    @param[in] d pointer to destination event handler

    */
    void setDestination( tern::cEventHandler * d )
    {
        myDstID = d->getID();
    }
    /**  Specify where events go next from port 2

    @param[in] d pointer to destination event handler

    */
    void setDestination2( tern::cEventHandler * d )
    {
        myDstID2 = d->getID();
    }

    virtual void Clear() {}
        virtual void SaveRunStatsToReplicationStats()  {}

    virtual void ReplicationReport() {}
};

/**

	The Event RuNer - an event simulator

*/
class cTERN
{
public:

	/// the current time
	long long theTime;

	/// maximum duration of simulation
	long long myStopTime;

	/// number of plot points
	int myPlotPoints;

	enum etype {
        none,
        task,
        flow }
        myType;

	/// construct new simulation
	cTERN()
	: theTime( 0 )
	, fConsolLog( false )
	{}

	/// connect two event handlers
	void Connect( const std::string& src,
              const std::string& dst );
	void Connect2( const std::string& src,
              const std::string& dst );

	/// run the simulation.
	void Run();

	/// schedule an event
	void Add(
		cPlanet* planet,
		int type,
		int ObjectID,
		long long time );
	void Add(
		cPlanet* planet,
		int type,
		cEventHandler* handler,
		long long time );

    void setConsoleLog( bool flag  = true )     { fConsolLog = flag; }

	/// Schedule next Poisson event
	long long NextPoisson( cEvent& e, int mean );

	/// Add event handler
	void Add( cEventHandler& handler );

	void Add( cPlanet& planet ) { myPlanets.push_back( &planet ); }
	void Delete( cPlanet* planet );

	/// Find event handler
	cEventHandler * Find( int id );

	int getHandlerCount()           { return (int) myHandlers.size(); }

	void HandleFatalError( const std::wstring& msg );
	void HandleFatalError( const std::string& msg );

private:

	/// events scheduled for the future.
	std::multiset < cEvent, std::less<long long> > myEventQueue;

    /// the event handlers
	typedef std::set < cEventHandler*, cEventHandler::less > handler_set_t;
	typedef handler_set_t::iterator handler_iter;
	handler_set_t myHandlers;

	/// event recorders
	std::vector <cPlanet*> myPlanets;

	bool fConsolLog;

	/// start handlers
	void Start();

	/// schedule plot point saves
	void AddPlotPoints();

	/// request final report from handlers
	void FinalReport();

	/// get a reference to the earliest scheduled event
	cEvent* getNextEvent();

	/// handle the earliest scheduled event
	void handle_next_event();

	cEventHandler* Find( const std::string& name );


	void DumpQueue();

};


extern cTERN theSimulationEngine;

}

}
}
