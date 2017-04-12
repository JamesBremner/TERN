#include <iostream>
#include "cQuality.h"
#include "flower.h"

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
#ifdef tern_build
    cSource( const raven::sim::gui::cFlower* f )
        : cEventHandler( f->getName() )
        , myMean( f->getValue( "Mean" ))
        , myTotal( 0 )
    {
        myfSteady = false;
        if( f->getValue( "Steady" ) > 0.5 )
            myfSteady = true;
        for ( auto it : myQuality )
        {
            myQuality.setValue( it.second, f->getValue( it.first ) );
        }
    }

#endif // tern_build
#ifdef tern_console

    /** Construct from parameters */
    cSource( const string& name, double mean, bool steady )
        : cEventHandler( name )
        , myMean( mean )
        , myfSteady( steady )
        {

        }

#endif

    /// initialize simulation, automaticalled called by simulator
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

    void FinalReport()
    {
        cout << "Source " << myName;
        cout << " Tasks generated " << myTotal << endl;
    }

private:
    bool myfSteady;     /// true if steady generation, false if exponential
    double myMean;
    double myTotal;
    cQuality myQuality;
};
}
}
}
