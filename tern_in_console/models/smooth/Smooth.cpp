// terntest.cpp : Defines the entry point for the console application.
//

#include <cstdlib>
#include <cstdio>
#include <iostream>

#include <boost/program_options.hpp>

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
    : cSource( "Receiver")
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
    int myNormalProcessingMean;
    int myOccasionalProcessingMean;

    public:
    cMixer2()
        : cDelay( "Mixer")
        , myNormalProcessingMean( 20 )
    {

    }
    int Delay( tern::cPlanet*  )
    {
        // calculate processing time
        static int count = 0;
        int mean_delay = myNormalProcessingMean;
        if( count++ > 5 )
        {
            count = 0;
            mean_delay = myOccasionalProcessingMean;
        }
        return  distribution::normal( mean_delay, 2 );
    }

    void NormalProcessingMean( int d )
    {
        myNormalProcessingMean = d;
    }

    void OccasionalProcessingMean( int d )
    {
        myOccasionalProcessingMean = d;
    }
};

class cSmoother : public task::cDelay
{
    int my_required_total_delay;
public:
    cSmoother( int required_total_delay)
    :cDelay( "Smoother" )
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
//        cout  << " extra delay " << extraDelay << "\n";

        return extraDelay;
    }

    /** set the reruired total delay

    This has the effect of setting a minimum processing delay

    */
    void required_total_delay( int d )
    {
        my_required_total_delay = d;
    }
};



// parse user options

void opts( int ac, char* av[],
          cMixer2& mixer,
          cSmoother& smoother )
{
    namespace po = boost::program_options;
// Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce help message")
    ("interval", po::value<int>()->default_value( 50 ), "set packet interval in msecs")
    ("reqdelay", po::value<int>()->default_value( 50 ), "required processing delay in msecs")
    ("normal", po::value<int>()->default_value( 20 ), "normal processing delay in msecs")
    ("occasional", po::value<int>()->default_value( 90 ), "occasional processing delay in msecs")
    ("run", po::value<int>()->default_value( 1000 ), "simulation run length in msecs")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        cout << desc << "\n";
        exit(1);
    }

    if( vm.count("interval"))
    {
        //PacketData.setInterval( vm["interval"].as<int>() );
    }
    if( vm.count("reqdelay"))
    {
        smoother.required_total_delay( vm["reqdelay"].as<int>() );
    }
    if( vm.count("normal"))
    {
        mixer.NormalProcessingMean( vm["normal"].as<int>() );
    }
    if( vm.count("occasional"))
    {
        mixer.OccasionalProcessingMean( vm["occasional"].as<int>() );
    }
     if( vm.count("run"))
     {
         tern::theSimulationEngine.myStopTime = vm["run"].as<int>();
     }
    }



int main(int argc, char* argv[])
{
    // construct the receiver where packets arrive every 50 milliseconds
    cRx Rx;

    // construct the Mixer which takes a random time to process the packets
    cMixer2 theMixer;

    // construct the smoother attempts to smooth the delay variability
    cSmoother theConstDelay( 50 );

    // construct packet transmitter
    task::cSink   theSink( "Tx" );

    // parse user options
    opts( argc, argv,
         theMixer,
         theConstDelay );

    //tern::theSimulationEngine.setConsoleLog();

    // connect everything together
    tern::theSimulationEngine.Connect( "Receiver", "Mixer");
    tern::theSimulationEngine.Connect( "Mixer", "Smoother" );
    tern::theSimulationEngine.Connect( "Smoother", "Tx" );

    // Start simulation run
    tern::theSimulationEngine.Run();

    return 0;
}

