#include <iostream>

#include "nlohmann_json.hpp"
#include "tern.h"
#include "task.h"
#include "terntime.h"
#include "cStoppingMachine.h"

using namespace std;
using namespace raven::sim;

class cReplicationReport
{
public:
    cReplicationReport( task::cSink& sink )
        : mySink( sink )
    {

    }
    void SaveRun();

    std::string Report();

private:
    task::cSink& mySink;
    task::cSink::stats_t aCount;
    task::cSink::stats_t aMin;
    task::cSink::stats_t aAver;
    task::cSink::stats_t aMax;
    task::cSink::stats_t aDev;
};

void cReplicationReport::SaveRun()
{
    task::cSink::stats_t& runAccumulator = mySink.Accumulator();
    aCount((int)boost::accumulators::count(runAccumulator));
    aMin((int)boost::accumulators::min(runAccumulator));
    aAver((int)boost::accumulators::mean(runAccumulator));
    aMax((int)boost::accumulators::max(runAccumulator));
    aDev((int)(int)sqrt(boost::accumulators::variance(runAccumulator)));
}

std::string cReplicationReport::Report()
{
    std::stringstream ss;
    ss << " count report: "
       << "count:" << boost::accumulators::count(aCount)
       << " min:" << boost::accumulators::min(aCount)
       << " aver:" << (int)boost::accumulators::mean(aCount)
       << " max:" << boost::accumulators::max(aCount)
       << " std dev:" << (int)sqrt(boost::accumulators::variance(aCount))
       << "\n";
    ss << " min report: "
       << "count:" << boost::accumulators::count(aMin)
       << " min:" << boost::accumulators::min(aMin)
       << " aver:" << (int)boost::accumulators::mean(aMin)
       << " max:" << boost::accumulators::max(aMin)
       << " std dev:" << (int)sqrt(boost::accumulators::variance(aMin))
       << "\n";
    ss << " aver report: "
       << "count:" << boost::accumulators::count(aAver)
       << " min:" << boost::accumulators::min(aAver)
       << " aver:" << (int)boost::accumulators::mean(aAver)
       << " max:" << boost::accumulators::max(aAver)
       << " std dev:" << (int)sqrt(boost::accumulators::variance(aAver))
       << "\n";
    ss << " max report: "
       << "count:" << boost::accumulators::count(aMax)
       << " min:" << boost::accumulators::min(aMax)
       << " aver:" << (int)boost::accumulators::mean(aMax)
       << " max:" << boost::accumulators::max(aMax)
       << " std dev:" << (int)sqrt(boost::accumulators::variance(aMax))
       << "\n";

    return ss.str();
}

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
    task::cSink   K{"sink"};

    // connect the machines in two parrallel series

    tern::theSimulationEngine.Connect( "src1", "MAC01A" );
    tern::theSimulationEngine.Connect( "MAC01A", "MAC01B" );
    tern::theSimulationEngine.Connect( "MAC01B", "MAC01C" );
    tern::theSimulationEngine.Connect( "MAC01C", "sink" );
    tern::theSimulationEngine.Connect( "src2", "MAC02A" );
    tern::theSimulationEngine.Connect( "MAC02A", "MAC02B" );
    tern::theSimulationEngine.Connect( "MAC02B", "MAC02C" );
    tern::theSimulationEngine.Connect( "MAC02C", "sink" );



    // run the simulation for 60 simulated seconds
    tern::theSimulationEngine.myStopTime = 600;

    // uncomment this to see detailed log of simulation events
    //tern::theSimulationEngine.setConsoleLog();

    cReplicationReport RepReport( K );

    // run simulation many times

    for( int rep = 0; rep < 100; rep++ )
    {
        tern::theSimulationEngine.Run();

        RepReport.SaveRun();

        tern::theSimulationEngine.Clear();

    }

    cout << RepReport.Report();

    return 0;
}


int main()
{
    //return RunHistorical();

    //return RunRandom();

    return RunRandom2();
}
