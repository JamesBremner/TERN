#include <iostream>
#include "tern.h"
#include "task.h"
using namespace std;
using namespace raven::sim;
int main()
{
    task::cSource S(L"src");
    task::cDelay  D(L"delay");
    task::cSink   K{L"sink"};

    tern::theSimulationEngine.Connect( L"src", L"delay" );
    tern::theSimulationEngine.Connect( L"delay", L"sink" );

    tern::theSimulationEngine.myStopTime = 10;

    tern::theSimulationEngine.setConsoleLog();

    // Start simulation run
    tern::theSimulationEngine.Run();

    return 0;
}
