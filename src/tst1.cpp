#include <iostream>
#include <sstream>

#include "tern.h"
#include "cSource.h"
#include "cSink.h"
#include "cDelay.h"


using namespace std;
using namespace raven::sim;

bool Delay()
{
    tern::theSimulationEngine.ClearAll();

    // src ->delay -> sink
    tern::cSource Source( "src", 2, true );
    tern::cDelay  Delay("delay",10);
    tern::cSink   Sink{"sink"};
    tern::theSimulationEngine.Connect( "src", "delay" );
    tern::theSimulationEngine.Connect( "delay", "sink" );

    tern::theSimulationEngine.myStopTime = 50;
    tern::theSimulationEngine.setConsoleLog();
    tern::theSimulationEngine.Run( );

    bool OK = true;
    if( (int)Source.FinalReportText().find( " generated 26" ) == -1 )
    {
        cout <<  Source.FinalReportText();
        cout << "Delay 1 failed\n";
        OK = false;
    }
    if( (int)Delay.FinalReportText().find( "Queue size 1" ) == -1 )
    {
        cout << Delay.FinalReportText();
        cout << "Delay 2 failed\n";
        OK = false;
    }

    if( (int)Sink.FinalReportText().find( "count:24" ) == -1 )
    {
        cout << Sink.FinalReportText();
        cout << "Delay 3 failed\n";
        OK = false;
    }

    return OK;
}

bool Src2SinkReplication()
{
    tern::theSimulationEngine.ClearAll();

    // src -> sink
    tern::cSource Source( "src", 2, true );
    tern::cSink   Sink{"sink"};
    tern::theSimulationEngine.Connect( "src", "sink" );

    tern::theSimulationEngine.myStopTime = 50;
    tern::theSimulationEngine.ReplicateRun( 10 );

    bool OK = true;
    if( (int)Source.ReplicationReportText().find( "min:26 aver:26 max:26" ) == -1 )
    {
        cout << "Src2SinkReplication 1 failed\n";
        OK = false;
    }
    if( (int)Sink.ReplicationReportText().find( "min:24 aver:24 max:24" ) == -1 )
    {
        cout << "Src2SinkReplication 2 failed\n";
        OK = false;
    }

    return OK;
}

bool Src2Sink()
{
    tern::theSimulationEngine.ClearAll();

    // src -> sink
    tern::cSource Source( "src", 1, true );
    tern::cSink   Sink{"sink"};
    tern::theSimulationEngine.Connect( "src", "sink" );

    tern::theSimulationEngine.myStopTime = 50;
    tern::theSimulationEngine.setConsoleLog();
    tern::theSimulationEngine.Run( );

    bool OK = true;
    if( (int)Source.FinalReportText().find( " generated 51" ) == -1 )
    {
        cout <<  Source.FinalReportText();
        cout << "Src2Sink 1 failed\n";
        OK = false;
    }
    if( (int)Sink.FinalReportText().find( "count:49" ) == -1 )
    {
        cout << Sink.FinalReportText();
        cout << "Src2Sink 2 failed\n";
        OK = false;
    }

    return OK;

}

int main()
{

    bool OK = true;
    if( ! Delay() )
        OK = false;
    if( ! Src2Sink() )
        OK = false;
    if( ! Src2SinkReplication() )
        OK = false;


    if( OK )
        cout << "ALL unit tests PASSED\n";
    else
        cout << "Unit test2 FAILED\n";

    return 0;
}

