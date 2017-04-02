#include <iostream>

#include "nlohmann_json.hpp"
#include "tern.h"
#include "task.h"
#include "terntime.h"
#include "cStoppingMachine.h"

using namespace std;
using namespace raven::sim;

int main()
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

    // run the simulation for 60 simulated seconds
    tern::theSimulationEngine.myStopTime = 60;

    // uncomment this to see detailed log of simulation events
    //tern::theSimulationEngine.setConsoleLog();

    // Start simulation run

    tern::theSimulationEngine.Run();

    return 0;
}
