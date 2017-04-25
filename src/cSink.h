namespace raven
{
namespace sim
{
namespace tern
{


///  Ultimate destination for packets

class cSink
    : public tern::cEventHandler
{
public:
    cSink(  const std::string& name )
        : cEventHandler( name )
        , myTotal( 0 )
        , myPlotTotal( 0 )
    {
        AddPlot( "Total" );
    }

    virtual void Clear()
    {
        cEventHandler::Clear();
#ifdef tern_console
        myAccumulator = stats::stats_t();
#endif // tern_console
    }

    int Handle( tern::cEvent* e );

    virtual void HandlePlotPointEvent();

    virtual void FinalReport()
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

    int myTotal;
    int myPlotTotal;

#ifdef tern_console

    // run stats
    stats::stats_t myAccumulator;

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
