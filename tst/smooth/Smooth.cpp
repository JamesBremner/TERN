// terntest.cpp : Defines the entry point for the console application.
//

#include <cstdlib>
#include <cstdio>
#include <iostream>

using namespace std;
#include "tern.h"
#include "task.h"

using namespace raven::sim;

class cPacket : public tern::cPlanet
{
public:
    cPacket()
        : cPlanet( tern::theSimulationEngine )
    {
        myID = nextID++;		// unique ID
    }
    int getPacketID()
    {
        return myID;
    }
private:
    static int nextID;				///< ID of next packet created
    int myID;
};

int cPacket::nextID = 1;

class cRx : public task::cSource
{
    public:
    cRx()
    : cSource( L"Receiver")
    {

    }
    tern::cPlanet * Construct()
    {
        return new cPacket();
    }
    /** Packets arrive once every 50 msecs */
    int Delay()
    {
        return 50;
    }

};

class cMixer2 : public task::cDelay
{
    public:
    cMixer2()
        : cDelay( L"Mixer")
    {

    }
    int Delay( tern::cPlanet*  )
    {
        // calculate processing time
        static int count = 0;
        int mean_delay = 20;
        if( count++ > 5 )
        {
            count = 0;
            mean_delay = 90;
        }
        return  distribution::normal( mean_delay, 2 );
    }
};

class cSmoother : public task::cDelay
{
    int my_required_total_delay;
public:
    cSmoother( int required_total_delay)
    :cDelay( L"Smoother" )
    , my_required_total_delay( required_total_delay )
    {

    }
    int Delay( tern::cPlanet* planet  )
    {
         // calculate extra delay to add
        int extraDelay = my_required_total_delay - planet->getLifetime();
        if( extraDelay < 0 )
            extraDelay = 0;

//        cout  << " at " << tern::theSimulationEngine.theTime
//              << " Packet " << ((cPacket*)myQ.front())->getPacketID() << " extra delay " << extraDelay << "\n";
        cout  << " extra delay " << extraDelay << "\n";

        return extraDelay;
    }
};



int main(int argc, char* argv[])
{
    //tern::theSimulationEngine.setConsoleLog();

    // the receiver where packets arrive every 50 milliseconds
    cRx Rx;

    // the Mixer which takes a random time to process the packets
    cMixer2 theMixer;

    // the smoother attempts to smooth the delay variability
    cSmoother theConstDelay( atoi( argv[1] ) );

    // Transmit the packets
    task::cSink   theSink( L"Tx" );

    // connect everything together
    tern::theSimulationEngine.Connect( L"Receiver", L"Mixer");
    tern::theSimulationEngine.Connect( L"Mixer", L"Smoother" );
    tern::theSimulationEngine.Connect( L"Smoother", L"Tx" );

    // Request run lasting so many clock ticks ( msecs )
    tern::theSimulationEngine.myStopTime = 1000;

    // Start simulation run
    tern::theSimulationEngine.Run();

    return 0;
}

