#include <iostream>
#include <sstream>

#include "tern.h"
#include "cSource.h"
#include "cSink.h"


using namespace std;
using namespace raven::sim;


void tst2()
{
    tern::theSimulationEngine.ClearAll();

     // src -> sink
    tern::cSource Source( "src", 2, true );
    tern::cSink   Sink{"sink"};
    tern::theSimulationEngine.Connect( "src", "sink" );

    tern::theSimulationEngine.myStopTime = 50;
    tern::theSimulationEngine.setConsoleLog();
    //tern::theSimulationEngine.Run( );
    tern::theSimulationEngine.ReplicateRun( 10 );

    bool OK = true;
    if( (int)Source.ReplicationReportText().find( "min:26 aver:26 max:26" ) == -1 )
    {
        cout << "source failed\n";
        OK = false;
    }
    if( (int)Sink.ReplicationReportText().find( "min:24 aver:24 max:24" ) == -1 ) {
        cout << "sink failed\n";
        OK = false;
    }
    if( ! OK )
        cout << "unit test failed\n";
    else
        cout << "unit test PASSED\n";
}

int main()
{

    // src -> sink
    tern::cSource Source( "src", 1, true );
    tern::cSink   Sink{"sink"};
    tern::theSimulationEngine.Connect( "src", "sink" );

    tern::theSimulationEngine.myStopTime = 50;
    tern::theSimulationEngine.setConsoleLog();
    //tern::theSimulationEngine.Run( );
    tern::theSimulationEngine.ReplicateRun( 10 );

    bool OK = true;
    if( (int)Source.ReplicationReportText().find( "min:51 aver:51 max:51" ) == -1 )
    {
        cout << "source failed\n";
        OK = false;
    }
    if( (int)Sink.ReplicationReportText().find( "min:49 aver:49 max:49" ) == -1 ) {
        cout << "sink failed\n";
        OK = false;
    }
    if( ! OK )
        cout << "unit test failed\n";
    else
        cout << "unit test PASSED\n";

    tst2();

    return 0;
}

