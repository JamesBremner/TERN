#include <iostream>
#include <sstream>
#include "tern.h"
#include "task.h"

namespace raven {
namespace sim {
namespace task {

void cSink::SaveRunStatsToReplicationStats()
{
    myRepCount((int)boost::accumulators::count(myAccumulator));
    myRepMin((int)boost::accumulators::min(myAccumulator));
    myRepAver((int)boost::accumulators::mean(myAccumulator));
    myRepMax((int)boost::accumulators::max(myAccumulator));
    myRepDev((int)(int)sqrt(boost::accumulators::variance(myAccumulator)));

    // clear run stats
    myAccumulator = stats::stats_t();
}

void cSink::ReplicationReport()
{
    std::stringstream ss;
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

    std::cout << ss.str();
}

void cDelay::SaveRunStatsToReplicationStats()
{
    myRep( myQMax );
    myQMax = 0;
}

void cDelay::ReplicationReport()
{
    std::stringstream ss;
    ss << getName() << " replication report\n";
    ss << " Q max report: "
       << "count:" << boost::accumulators::count(myRep)
       << " min:" << boost::accumulators::min(myRep)
       << " aver:" << (int)boost::accumulators::mean(myRep)
       << " max:" << boost::accumulators::max(myRep)
       << " std dev:" << (int)sqrt(boost::accumulators::variance(myRep))
       << "\n";

    std::cout << ss.str();
}
}
}
}
