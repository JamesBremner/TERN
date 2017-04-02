#include <iostream>
#include <chrono>
#include "nlohmann_json.hpp"
#include "tern.h"
#include "task.h"
using namespace std;
using namespace raven::sim;

typedef chrono::time_point<chrono::system_clock> date_t;
date_t fTime( int year, int month, int day,
              int hr, int min, int sec )
{

    tm seed;
    seed.tm_year = year - 1900;
    seed.tm_mon = month - 1;    // [0, 11) since Jan
    seed.tm_mday = day;        // [1, 31)
    seed.tm_hour = hr;        // [0, 23) since midnight
    seed.tm_min = min;          // [0, 59) after the hour
    seed.tm_sec = sec;          // [0, 60) after the min
    seed.tm_isdst = 0;

    return chrono::system_clock::from_time_t( mktime( &seed ));
}
date_t f1Time( const string& st )
{
    return fTime(
               atoi(st.substr(6,4).c_str()),
               atoi(st.substr(3,2).c_str()),
               atoi(st.substr(0,2).c_str()),
               atoi(st.substr(11,2).c_str()),
               atoi(st.substr(14,2).c_str()),
               atoi(st.substr(17,2).c_str())
           );
}

/** Simulate a machine that stops from time to time */

class cStoppingMachine : public task::cDelay
{
public:

    /** Construct
        @param[in) name of the machine
        @param[in) stopschedule json formatted stopping schedule
    */

    cStoppingMachine(
        const string& name,
        const string& stopschedule );

    /** Initialize

    This converts the stopping schedule into simulation time
    It must be called after all machines have been constructed
    but before the simulations starts
    */
    void Init();

    virtual int Delay( tern::cPlanet * planet );

private:
    vector< date_t > myStop;
    vector< date_t > myStart;
    vector< long long > myStopInSimTime;
    vector< long long > myStartInSimTime;
    static date_t theEarliestStop;
};

string stopschedule =
    "{ \"machines\": ["
    " { \"id\": \"MAC01A\","
    "   \"stops\": ["
    "     {\"stop_time\": \"13-03-2017 06:20:00\","
    "      \"start_time\": \"13-03-2017 06:20:02\"},"
    "     {\"stop_time\": \"13-03-2017 06:20:06\","
    "      \"start_time\": \"13-03-2017 06:20:08\""
    "     }]}]}";

int main()
{
    // construct the machines
    // using the json formatted
    task::cSource S("src");
    cStoppingMachine  MAC01A("MAC01A",stopschedule);
    cStoppingMachine  MAC01B("MAC01B",stopschedule);
    cStoppingMachine  MAC01C("MAC01C",stopschedule);
    task::cSink   K{"sink"};

    MAC01A.Init();
    MAC01B.Init();
    MAC01C.Init();

    tern::theSimulationEngine.Connect( "src", "MAC01A" );
    tern::theSimulationEngine.Connect( "MAC01A", "MAC01B" );
    tern::theSimulationEngine.Connect( "MAC01B", "MAC01C" );
    tern::theSimulationEngine.Connect( "MAC01C", "sink" );

    tern::theSimulationEngine.myStopTime = 60;

    //tern::theSimulationEngine.setConsoleLog();

    // Start simulation run
    tern::theSimulationEngine.Run();

    return 0;
}

date_t cStoppingMachine::theEarliestStop /* = f1Time("13-03-2050 06:20:00") */;

cStoppingMachine::cStoppingMachine(
    const string& name,
    const string& stopschedule )
    : raven::sim::task::cDelay( name )
{
    nlohmann::json J = nlohmann::json::parse( stopschedule );
    //cout << J.dump(2) << "\n====\n";

    // loop over machines
    for( auto& machine : J["machines"] )
    {
        // are these the specs for this machine
        if( machine["id"] == getName() )
        {
            cout << machine.dump(2) << "\n";

            // loop over stops
            for( auto& stop : machine["stops"] )
            {
                myStop.push_back(
                    f1Time( stop["stop_time"] ) );
                myStart.push_back(
                    f1Time( stop["start_time"] ) );
            }
        }
    }

    if( myStop.size() )
    {
        if( theEarliestStop == date_t() )
            theEarliestStop = myStop[0];
        else if( myStop[0] < theEarliestStop )
            theEarliestStop = myStop[0];
    }
}

void cStoppingMachine::Init()
{
    const int time_to_first_stop = 10;

    using namespace std::chrono;

    if( ! myStop.size() )
        return;

    int dtFirtStop = theEarliestStop.time_since_epoch().count() * system_clock::period::num / system_clock::period::den;

    for( auto& stop : myStop )
    {
        myStopInSimTime.push_back(
            stop.time_since_epoch().count() * system_clock::period::num / system_clock::period::den
            - dtFirtStop + time_to_first_stop );

        //cout << dtFirtStop << " " << myStopInSimTime.back() << "\n";
    }
    for( auto& start : myStart )
    {
        myStartInSimTime.push_back(
            start.time_since_epoch().count() * system_clock::period::num / system_clock::period::den
            - dtFirtStop + time_to_first_stop );
    }

}

int cStoppingMachine::Delay( tern::cPlanet * planet )
{
    for( int kstop = 0; kstop < (int)myStop.size(); kstop++ )
    {
        if( myStopInSimTime[kstop] == tern::theSimulationEngine.theTime &&
                tern::theSimulationEngine.theTime < myStartInSimTime[ kstop ] )
        {
            int delay = myStartInSimTime[ kstop ] - tern::theSimulationEngine.theTime;
            //cout << myStartInSimTime[ kstop ] << " " << tern::theSimulationEngine.theTime << "\n";
            cout << getName() << " delay at " << tern::theSimulationEngine.theTime << " for " <<delay << "\n";
            return delay;
        }
    }
    return 1;
}
