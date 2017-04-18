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
    {

    }

    virtual void Clear()
    {
        cEventHandler::Clear();
        myAccumulator = stats::stats_t();
    }

    int Handle( tern::cEvent* e )
    {
        if( cEventHandler::Handle( e ))
            return 1;

        // switch on event type
        switch ( e->myType )
        {
        case 1:

            // planet has arrived at sink

            // add to accumulated statistics
            myAccumulator( e->myPlanet->getLifetime() );

            // delete the planet
            tern::theSimulationEngine.Delete( e->myPlanet );

            // all done
            return 1;


        default:
            return 0;
        }
    }

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

    // run stats
    stats::stats_t myAccumulator;

    // replication stats
    stats::stats_t myRepCount;
    stats::stats_t myRepMin;
    stats::stats_t myRepAver;
    stats::stats_t myRepMax;
    stats::stats_t myRepDev;
};

}
}
}
