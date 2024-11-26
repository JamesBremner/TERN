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

int cSink::Handle( tern::cEvent* e )
{
    if( cEventHandler::Handle( e ))
        return 1;

    // switch on event type
    switch ( e->myType )
    {
    case 1:

        // planet has arrived at sink

        // add to accumulated statistics
#ifdef tern_console
        myAccumulator( e->myPlanet->getLifetime() );
#endif // tern_console
#ifdef tern_vase
        myTotal++;
#endif // tern_vase

        // delete the planet
        tern::theSimulationEngine.Delete( e->myPlanet );

        // all done
        return 1;


    default:
        return 0;
    }
}

void cSink::HandlePlotPointEvent()
{
    if( ! myPlot.size() )
        return;
    myPlot[0].myData.push_back( ( myTotal - myPlotTotal ) / 2 );
    myPlotTotal = myTotal;
}

std::string cSink::FinalReportText()
{
    PlotOutput();

    std::stringstream ss;
#ifdef tern_console
    ss << "Sink " << myName << " report: "
       << "count:" << boost::accumulators::count(myAccumulator)
       << " min:" << boost::accumulators::min(myAccumulator)
       << " aver:" << (int)boost::accumulators::mean(myAccumulator)
       << " max:" << boost::accumulators::max(myAccumulator)
       << " std dev:" << (int)sqrt(boost::accumulators::variance(myAccumulator))
       << "\n";
#endif // tern_console
#ifdef vase
    ss << "Source " << myName;
    ss << " Total Arrivals " << myTotal << "\n";
#endif // tern_vase
    return ss.str();
}
void cSink::SaveRunStatsToReplicationStats()
{
#ifdef tern_console
    myRepCount((int)boost::accumulators::count(myAccumulator));
    myRepMin((int)boost::accumulators::min(myAccumulator));
    myRepAver((int)boost::accumulators::mean(myAccumulator));
    myRepMax((int)boost::accumulators::max(myAccumulator));
    myRepDev((int)(int)sqrt(boost::accumulators::variance(myAccumulator)));

    // clear run stats
    myAccumulator = stats::stats_t();
#endif // tern_console
}
std::string cSink::ReplicationReportText()
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
