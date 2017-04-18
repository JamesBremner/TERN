#include <iostream>
#include <sstream>
#include "tern.h"
#include "cSink.h"

namespace raven
{
namespace sim
{
namespace tern
{

std::string cSink::FinalReportText()
{
    std::stringstream ss;
    ss << "Sink " << myName << " report: "
       << "count:" << boost::accumulators::count(myAccumulator)
       << " min:" << boost::accumulators::min(myAccumulator)
       << " aver:" << (int)boost::accumulators::mean(myAccumulator)
       << " max:" << boost::accumulators::max(myAccumulator)
       << " std dev:" << (int)sqrt(boost::accumulators::variance(myAccumulator))
       << "\n";
    return ss.str();
}
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
std::string cSink::ReplicationReportText()
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

    return ss.str();
}
}
}
}
