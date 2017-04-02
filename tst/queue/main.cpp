#include <iostream>
#include "tern.h"
#include "task.h"
using namespace std;
using namespace raven::sim;
int main()
{
    task::cSource S("src");
    task::cDelay  D("delay");
    task::cSink   K{"sink"};

    tern::theSimulationEngine.Connect( "src", "delay" );
    tern::theSimulationEngine.Connect( "delay", "sink" );

    tern::theSimulationEngine.myStopTime = 10;

    tern::theSimulationEngine.setConsoleLog();

    // Start simulation run
    tern::theSimulationEngine.Run();

    return 0;
}
