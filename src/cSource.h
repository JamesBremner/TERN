#include <iostream>

#ifndef tern_console
#include "cQuality.h"
#include "flower.h"
#endif // tern_console

using namespace std;

namespace raven
{
namespace sim
{
namespace tern
{

/// An event source
class cSource
    : public tern::cEventHandler
{
public:
    /** Construct from flower in vase

    @param[in] f the vase flower that specifies the source

    f->getName() the name of the flower

    f->getValue( "Mean" ) then mean time between arrivals
        0 indicates the source is disabled
        1 use for flow sources

    f->getValue( "Steady" )
        0 for a task orientated simulation with rendom, exponential distributed arrivals
        1 for a flow simulation, of for non-random arrivals

    f->getValue( quality_name ) specify qualitites of the flow generated

    */
#ifdef vase

    cSource( const raven::sim::gui::cFlower* f );

#endif // vase

#ifdef tern_console

    /** Construct from parameters */
    cSource( const string& name, double mean, bool steady )
        : cEventHandler( name )
        , myfSteady( steady )
        , myMean( mean )
        , myTotal( 0 )
        {

        }

#endif

    /// initialize simulation, automatically called by simulator
    void Start()
    {
        ScheduleArrival();
    }

    ///  Schedule next arrival
    void ScheduleArrival();

    /// Handle an arrival, auto called by simulation
    int Handle( tern::cEvent* e )
    {
        if( cEventHandler::Handle( e ))
            return 1;

        switch( e->myType )
        {

        default:
            // Schedule the next arrival
            ScheduleArrival();

            // Send this arrival to the destination immediatly
            tern::theSimulationEngine.Add(
                e->myPlanet,
                1,
                myDstID,
                tern::theSimulationEngine.theTime );
        }
        return 1;
    }

    virtual void HandlePlotPointEvent();

    void FinalReport()
        {
        std::cout << FinalReportText();
    }
    virtual std::string FinalReportText();

    virtual void SaveRunStatsToReplicationStats();

    virtual void ReplicationReport()
    {
        std::cout << ReplicationReportText();
    }
    virtual std::string ReplicationReportText();

private:
    bool myfSteady;     /// true if steady generation, false if exponential
    double myMean;
    double myTotal;
    int myPlotTotal;
    cQuality myQuality;

#ifdef tern_console
        // replication stats
    stats::stats_t myRepCount;
    stats::stats_t myRepMin;
    stats::stats_t myRepAver;
    stats::stats_t myRepMax;
    stats::stats_t myRepDev;
#endif // tern_console

};
}
}
}
