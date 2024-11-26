// terntest.cpp : Defines the entry point for the console application.
//

#include <cstdio>
#include <iostream>
using namespace std;
#include "tern.h"
#include "probability.h"

/// An event source
class cSource
    : public tern::cEventHandler
{
public:
    /** Construct

    @param[in] name
    @param[in] mean  time between events

    */
    cSource( const std::wstring& name, double mean )
    : cEventHandler( name )
    , myMean( mean)
    {}



    /// initialize simulation, automaticalled called by simulator
    void Start()
    {
        ScheduleArrival();
    }

    ///  Schedule next arrival
    void ScheduleArrival()
    {
        tern::theSimulationEngine.Add(
            new tern::cPlanet( tern::theSimulationEngine ),       // create a planet, which will wander through simulation recording progress
            1,                              // event type
            this,                           // handle the event here
            tern::theSimulationEngine.theTime + (__int64)cPoisson::ran( myMean )  // random time from now with specified mean
        );
    }

    /// Handle an arrival, auto called by simulation
    int Handle( tern::cEvent* e )
    {
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

private:
    double myMean;
};


///  Ultimate destination for planets

class cSink
    : public tern::cEventHandler
{
public:
    cSink(  const std::wstring& name )
    : cEventHandler( name )
    {

    }
    /// Planet has arrived at sink
    int Handle( tern::cEvent* e )
    {
        // report on planet's history
        wcout << " Planet " << e->myPlanet->myID << " arrived at sink " << myName
             << " at " << tern::theSimulationEngine.theTime << " after " << e->myPlanet->getLifetime() << endl;

        // delete the planet
        tern::theSimulationEngine.Delete( e->myPlanet );

        // all done
        return 1;
    }

};


int main(int argc, char* argv[])
{

    // Source of events arriving with mean 10 clock tiks
    cSource theSource( L"src1", 10 );

    // Sink of events
    cSink   theSink( L"snk1" );

    // connect source to sink
    tern::theSimulationEngine.Connect( L"src1", L"snk1" );

    // Request run lasting 100 clock ticks
    tern::theSimulationEngine.myStopTime = 1000;

    // Start simulation run
    tern::theSimulationEngine.Run();

    return 0;
}

