#include <iostream>

#include "nlohmann_json.hpp"
#include "tern.h"
#include "task.h"
#include "terntime.h"
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

    task::cSource S("src");
    cStoppingMachine  MAC01A( "MAC01A", stopschedule );
    cStoppingMachine  MAC01B( "MAC01B", stopschedule );
    cStoppingMachine  MAC01C( "MAC01C", stopschedule );
    task::cSink   K{"sink"};

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

    cStoppingMachineSource S( "src", 1 );
    cStoppingMachine  MAC01A( "MAC01A", MeanSecsBetweenStops, MeanSecsStopDuration, DevSecsStopDuration );
    cStoppingMachine  MAC01B( "MAC01B", MeanSecsBetweenStops, MeanSecsStopDuration, DevSecsStopDuration );
    cStoppingMachine  MAC01C( "MAC01C", MeanSecsBetweenStops, MeanSecsStopDuration, DevSecsStopDuration );
    task::cSink   K{"sink"};

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

    cStoppingMachineSource S1( "src1", 2 );
    cStoppingMachine  MAC01A( "MAC01A", MeanSecsBetweenStops, MeanSecsStopDuration, DevSecsStopDuration );
    cStoppingMachine  MAC01B( "MAC01B", MeanSecsBetweenStops, MeanSecsStopDuration, DevSecsStopDuration );
    cStoppingMachine  MAC01C( "MAC01C", MeanSecsBetweenStops, MeanSecsStopDuration, DevSecsStopDuration );
    cStoppingMachineSource S2( "src2", 2 );
    cStoppingMachine  MAC02A( "MAC02A", MeanSecsBetweenStops, MeanSecsStopDuration, DevSecsStopDuration );
    cStoppingMachine  MAC02B( "MAC02B", MeanSecsBetweenStops, MeanSecsStopDuration, DevSecsStopDuration );
    cStoppingMachine  MAC02C( "MAC02C", MeanSecsBetweenStops, MeanSecsStopDuration, DevSecsStopDuration );
    task::cSink   K{"sink1"};
    task::cSink   K2{"sink2"};

    // connect the machines in two parrallel series

    tern::theSimulationEngine.Connect( "src1", "MAC01A" );
    tern::theSimulationEngine.Connect( "MAC01A", "MAC01B" );
    tern::theSimulationEngine.Connect( "MAC01B", "MAC01C" );
    tern::theSimulationEngine.Connect( "MAC01C", "sink1" );
    tern::theSimulationEngine.Connect( "src2", "MAC02A" );
    tern::theSimulationEngine.Connect( "MAC02A", "MAC02B" );
    tern::theSimulationEngine.Connect( "MAC02B", "MAC02C" );
    tern::theSimulationEngine.Connect( "MAC02C", "sink2" );



    // run the simulation for 60 simulated seconds
    tern::theSimulationEngine.myStopTime = 600;

    // uncomment this to see detailed log of simulation events
    tern::theSimulationEngine.setConsoleLog();

    // Start simulation run

    tern::theSimulationEngine.Run();

    return 0;
}


int main()
{
    //return RunHistorical();

    //return RunRandom();

    return RunRandom2();
}
