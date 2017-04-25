#include <sstream>
#include <vector>
#include <map>
#include "tern.h"
#include "cSource.h"
#include "probability.h"

namespace raven
{
namespace sim
{
namespace tern
{
void cSource::ScheduleArrival()
{
    // Calculate time of next arrival

    long long next_time = tern::theSimulationEngine.theTime;
    if( myMean < 0.9 )
    {
        // source is disabled
        return;
    }
    else
    {

        if( myfSteady )
        {
            // constant rate
            next_time += myMean;
        }
        else
        {
            // random time from now with specified mean
            next_time += (__int64)raven::sim::prob::cPoisson::ran( myMean );
        }
    }

    // construct the arriving planet
    tern::cPlanet * planet = new tern::cPlanet( tern::theSimulationEngine );
    planet->setQuality( myQuality );
    myTotal++;

    //cout << "cSource::ScheduleArrival " << myMean <<" " << next_time << endl;

    // schedule
    tern::theSimulationEngine.Add(
        planet,       // create a planet, which will wander through simulation recording progress
        1,            // event type
        this,         // handle the event here
        next_time
    );
}
std::string cSource::FinalReportText()
{
    std::stringstream ss;
    ss << "Source " << myName;
    ss << " Tasks generated " << myTotal << endl;
    return ss.str();
}

void cSource::SaveRunStatsToReplicationStats()
{
    #ifdef tern_console
    myRepCount( myTotal );
    myTotal = 0;
    #endif
}

std::string cSource::ReplicationReportText()
{
    std::stringstream ss;

#ifdef tern_console
    ss << getName() << " replication report\n";
    ss << " count report: "
       << "count:" << boost::accumulators::count(myRepCount)
       << " min:" << boost::accumulators::min(myRepCount)
       << " aver:" << (int)boost::accumulators::mean(myRepCount)
       << " max:" << boost::accumulators::max(myRepCount)
       << " std dev:" << (int)sqrt(boost::accumulators::variance(myRepCount))
       << "\n";
    ss << " min report: "
       << "count:" << boost::accumulators::count(myRepMin)
       << " min:" << boost::accumulators::min(myRepMin)
       << " aver:" << (int)boost::accumulators::mean(myRepMin)
       << " max:" << boost::accumulators::max(myRepMin)
       << " std dev:" << (int)sqrt(boost::accumulators::variance(myRepMin))
       << "\n";
    ss << " aver report: "
       << "count:" << boost::accumulators::count(myRepAver)
       << " min:" << boost::accumulators::min(myRepAver)
       << " aver:" << (int)boost::accumulators::mean(myRepAver)
       << " max:" << boost::accumulators::max(myRepAver)
       << " std dev:" << (int)sqrt(boost::accumulators::variance(myRepAver))
       << "\n";
    ss << " max report: "
       << "count:" << boost::accumulators::count(myRepMax)
       << " min:" << boost::accumulators::min(myRepMax)
       << " aver:" << (int)boost::accumulators::mean(myRepMax)
       << " max:" << boost::accumulators::max(myRepMax)
       << " std dev:" << (int)sqrt(boost::accumulators::variance(myRepMax))
       << "\n";
#endif // tern_console
    return ss.str();
}

}
}
}

