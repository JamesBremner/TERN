#include <queue>

namespace raven
{
namespace sim
{
namespace tern
{

///** Simulate a delay, storing planets in a queue until the delay is over */

class cDelay : public tern::cEventHandler
{
protected:

    std::queue < tern::cPlanet * > myQ;
    int myQMax;
    double myMean;
#ifdef tern_console
    stats::stats_t myRep;
#endif

public:
    cDelay(  const std::string& name, double mean )
        : cEventHandler( name )
        , myQMax( 0 )
        , myMean( mean )
    {

    }
    /** Calculate delay for a planet at the head of the queue */
    virtual int Delay( tern::cPlanet * planet );

    /** Schedule completion for the planet at the head of the queue */
    void ScheduleCompletion();

    /** Handle event */
    int Handle( tern::cEvent* e );

    virtual std::string FinalReportText();

    virtual void FinalReport()
    {
        std::cout << FinalReportText();
    }

    virtual void Clear()
    {
        cEventHandler::Clear();
        myQ = std::queue < tern::cPlanet * >();
        myQMax = 0;
    }

    /** Current queue length */
    int CurrentQLength()
    {
        return (int) myQ.size();
    }

    virtual void SaveRunStatsToReplicationStats();

    virtual void ReplicationReport();
};

}
}
}

