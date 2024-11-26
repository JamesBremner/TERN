#include <iostream>

#include "json.hpp"
#include "tern.h"
#include "cSink.h"
#include "cDelay.h"
#include "terntime.h"
#include "cSource.h"
#include "cStoppingMachine.h"

using namespace std;
using namespace raven::sim;

int RunHistorical()
{
    /* specify the stopping schedule

    {
    "machines": [
    {
      "id": "MAC01A",
      "stops": [
        {
          "start_time": "13-03-2017 06:20:02",
          "stop_time": "13-03-2017 06:20:00"
        },
        {
          "start_time": "13-03-2017 06:20:08",
          "stop_time": "13-03-2017 06:20:06"
        }
      ]
    }
    ]
    }

    */
    string stopschedule =
        "{ \"machines\": ["
        " { \"id\": \"MAC01A\","
        "   \"stops\": ["
        "     {\"stop_time\": \"13-03-2017 06:20:00\","
        "      \"start_time\": \"13-03-2017 06:20:02\"},"
        "     {\"stop_time\": \"13-03-2017 06:20:06\","
        "      \"start_time\": \"13-03-2017 06:20:08\""
        "     }]}]}";

    // construct the machines
    // using the json formatted stopping schedule

    tern::cSource S("src", 1, true );
    cStoppingMachine  MAC01A( "MAC01A", stopschedule );
    cStoppingMachine  MAC01B( "MAC01B", stopschedule );
    cStoppingMachine  MAC01C( "MAC01C", stopschedule );
    tern::cSink   K{"sink"};

    // connect the machines in series

    tern::theSimulationEngine.Connect( "src", "MAC01A" );
    tern::theSimulationEngine.Connect( "MAC01A", "MAC01B" );
    tern::theSimulationEngine.Connect( "MAC01B", "MAC01C" );
    tern::theSimulationEngine.Connect( "MAC01C", "sink" );

    tern::theSimulationEngine.UseCalendar();

    // run the simulation for 60 simulated seconds
    tern::theSimulationEngine.myStopTime = 60;

    // uncomment this to see detailed log of simulation events
    //tern::theSimulationEngine.setConsoleLog();

    // Start simulation run

    tern::theSimulationEngine.Run();

    return 0;
}
int RunRandom()
{

    // construct the machines
    // to use randomly generated stopping schedules
    // based on these parameters

    int MeanSecsBetweenStops = 10;
    int MeanSecsStopDuration = 4;
    int DevSecsStopDuration = 1;

    tern::cSource S( "src", 1, 0 );
    cStoppingMachine  MAC01A( "MAC01A", MeanSecsBetweenStops, MeanSecsStopDuration, DevSecsStopDuration, 10 );
    cStoppingMachine  MAC01B( "MAC01B", MeanSecsBetweenStops, MeanSecsStopDuration, DevSecsStopDuration, 10 );
    cStoppingMachine  MAC01C( "MAC01C", MeanSecsBetweenStops, MeanSecsStopDuration, DevSecsStopDuration, 10 );
    tern::cSink   K{"sink"};

    // connect the machines in series

    tern::theSimulationEngine.Connect( "src", "MAC01A" );
    tern::theSimulationEngine.Connect( "MAC01A", "MAC01B" );
    tern::theSimulationEngine.Connect( "MAC01B", "MAC01C" );
    tern::theSimulationEngine.Connect( "MAC01C", "sink" );



    // run the simulation for 60 simulated seconds
    tern::theSimulationEngine.myStopTime = 600;

    // uncomment this to see detailed log of simulation events
    //tern::theSimulationEngine.setConsoleLog();

    // Start simulation run

    tern::theSimulationEngine.Run();

    return 0;
}

int RunRandom2()
{

    // construct the machines
    // to use randomly generated stopping schedules
    // based on these parameters

    int MeanSecsBetweenStops = 10;
    int MeanSecsStopDuration = 4;
    int DevSecsStopDuration = 1;
    int OutputBufferThreshold = 10;

    tern::cSource S1( "src1", 2, true );
    cStoppingMachine  MAC01A( "MAC01A", MeanSecsBetweenStops, MeanSecsStopDuration, DevSecsStopDuration, OutputBufferThreshold );
    cStoppingMachine  MAC01B( "MAC01B", MeanSecsBetweenStops, MeanSecsStopDuration, DevSecsStopDuration, OutputBufferThreshold  );
    cStoppingMachine  MAC01C( "MAC01C", MeanSecsBetweenStops, MeanSecsStopDuration, DevSecsStopDuration, OutputBufferThreshold  );
    tern::cSource S2( "src2", 2, true );
    cStoppingMachine  MAC02A( "MAC02A", MeanSecsBetweenStops, MeanSecsStopDuration, DevSecsStopDuration, OutputBufferThreshold  );
    cStoppingMachine  MAC02B( "MAC02B", MeanSecsBetweenStops, MeanSecsStopDuration, DevSecsStopDuration, OutputBufferThreshold  );
    cStoppingMachine  MAC02C( "MAC02C", MeanSecsBetweenStops, MeanSecsStopDuration, DevSecsStopDuration, OutputBufferThreshold  );
    tern::cSink   K{"sink"};

    // connect the machines in two parrallel series

    tern::theSimulationEngine.Connect( "src1", "MAC01A" );
    tern::theSimulationEngine.Connect( "MAC01A", "MAC01B" );
    tern::theSimulationEngine.Connect( "MAC01B", "MAC01C" );
    tern::theSimulationEngine.Connect( "MAC01C", "sink" );
    tern::theSimulationEngine.Connect( "src2", "MAC02A" );
    tern::theSimulationEngine.Connect( "MAC02A", "MAC02B" );
    tern::theSimulationEngine.Connect( "MAC02B", "MAC02C" );
    tern::theSimulationEngine.Connect( "MAC02C", "sink" );

    // run the simulation for 60 simulated seconds each time
    tern::theSimulationEngine.myStopTime = 600;

    // uncomment this to see detailed log of simulation events
    //tern::theSimulationEngine.setConsoleLog();

    // run simulation many times
    tern::theSimulationEngine.ReplicateRun( 10 );

    return 0;
}

int RunThresholdTest()
{

    tern::cSource S1( "src", 1, true );

    // construct a machine that never stops while output below threshold
    cStoppingMachine  M1( "machine1",
                          1000,     // do not stop for 1000 secs
                          0,        // don't stop for any length
                          1,
                          10        // output buffer threshold
                        );

    // construct a machine that stops quickly and stays stopped
    cStoppingMachine  M2( "machine2",
                          0,            // stop soon after starts
                          20,         // stay stopped for a long tim
                          1,
                          10
                        );

    tern::cSink   K{"sink"};
    tern::theSimulationEngine.Connect( "src", "machine1" );
    tern::theSimulationEngine.Connect( "machine1", "machine2" );
    tern::theSimulationEngine.Connect( "machine2", "sink" );
    tern::theSimulationEngine.myStopTime = 50;
    tern::theSimulationEngine.setConsoleLog();
    tern::theSimulationEngine.Run();

    return 0;
}
int main()
{
    //return RunHistorical();

    //return RunRandom();

    //return RunRandom2();

    return RunThresholdTest();
}
