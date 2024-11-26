#include <iostream>
#include "tern.h"
#include "cSource.h"
#include "cSink.h"
#include "cDelay.h"
using namespace std;
using namespace raven::sim;
int main()
{
    tern::cSource S("src",1,true);
    tern::cDelay  D("delay");
    tern::cSink   K{"sink"};

    tern::theSimulationEngine.Connect( "src", "delay" );
    tern::theSimulationEngine.Connect( "delay", "sink" );

    tern::theSimulationEngine.myStopTime = 10;

    tern::theSimulationEngine.setConsoleLog();

    // Start simulation run
    tern::theSimulationEngine.Run();

    return 0;
}
