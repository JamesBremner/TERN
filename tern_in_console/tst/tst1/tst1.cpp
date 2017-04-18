#include <iostream>
#include <sstream>

#include "tern.h"
#include "cSource.h"
#include "cSink.h"


using namespace std;
using namespace raven::sim;


int main()
{

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

    return 0;
}

