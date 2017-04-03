#include <string>
#include <sstream>
#include "terntime.h"
#include "tern.h"
namespace raven
{
namespace sim
{
namespace tern
{
std::string cCalendar::Text()
{
    std::stringstream st;
    if( ! IsUsed )
    {
        // Not using calendar
        // so just return the simulation time
        st << theSimulationEngine.theTime;
        return st.str();
    }

    // Number of seconds since start of simulation
    std::chrono::duration<int> sim_secs( theSimulationEngine.theTime );

    // return human readable calendar time
    return fTime(
                 "%Y-%m-%d %H:%M:%S",
                 myRTStart + sim_secs );
}
}
}
}

