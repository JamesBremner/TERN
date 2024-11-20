#include <iostream>
#include <sstream>

#include "tern.h"
#include "cSource.h"
#include "cSink.h"
#include "cDelay.h"
#include "probability.h"

#include "cStoppingMachine.h"


using namespace std;
using namespace raven::sim;

bool StoppingMachine()
{
    tern::theSimulationEngine.ClearAll();

    raven::sim::prob::cPoisson::ran(10);
    raven::sim::prob::cPoisson::ran(10);
    raven::sim::prob::cPoisson::ran(10);


    // src ->StoppingMachine -> sink
    tern::cSource Source( "src", 2, true );
    cStoppingMachine  M1("Machine1",10,2,1,5);
    tern::cSink   Sink{"sink"};
    tern::theSimulationEngine.Connect( "src", "Machine1" );
    tern::theSimulationEngine.Connect( "Machine1", "sink" );

    tern::theSimulationEngine.myStopTime = 100;
    tern::theSimulationEngine.setConsoleLog();
    tern::theSimulationEngine.Run( );

    bool OK = true;
    if( (int)Source.FinalReportText().find( " generated 51" ) == -1 )
    {
        cout <<  Source.FinalReportText();
        cout << "Test 1 failed\n";
        OK = false;
    }
    if( (int)M1.FinalReportText().find( "Stopped Time 13" ) == -1 )
    {
        cout << M1.FinalReportText();
        cout << "Test 2 failed\n";
        OK = false;
    }

    if( (int)Sink.FinalReportText().find( "count:49" ) == -1 )
    {
        cout << Sink.FinalReportText();
        cout << "Test 3 failed\n";
        OK = false;
    }

    return OK;
}


int main()
{

    bool OK = true;
    if( ! StoppingMachine() )
        OK = false;


    if( OK )
        cout << "ALL unit tests PASSED\n";

    return 0;
}

