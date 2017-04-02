
/** Wrappers for std::chrono */
#pragma once
#include <cstdlib>
#include <chrono>

using namespace std;

namespace raven {
    namespace sim {
        namespace time {
typedef chrono::time_point<chrono::system_clock> date_t;
inline date_t fTime( int year, int month, int day,
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
inline date_t f1Time( const string& st )
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
        }
    }
}
