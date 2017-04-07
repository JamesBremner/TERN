#pragma once
#include <iostream>
#include "tern.h"

namespace raven
{
namespace sim
{
namespace tern
{
class cFunnel
    : public tern::cEventHandler
{
public:
    /** Construct

    @param[in] name
    @param[in] capacity maximum voume held without spilling
    @param[in] output maximum output

    */
    cFunnel(  const std::string& name,
              double capacity,
              double output  )
        : cEventHandler( name )
        , myCapacity( capacity )
        , myOutput( output )
        , myLevel( 0 )
        , full_time( 0 )
        , myTotalInput( 0 )
    {
        myQualityIndexVolume = cQuality::getIndex( "Volume");
    }
    cFunnel( raven::sim::gui::cFlower * f )
        : cEventHandler( f->getName() )
        , myCapacity( f->getValue("Capacity") )
        , myOutput( f->getValue("Output") )
        , myLevel( f->getValue("StartVolume") )
        , full_time( 0 )
        , myTotalInput( 0 )
    {
        myQualityIndexVolume = cQuality::getIndex( "Volume");

        // loop over defined qualities
        for( auto q : myQuality )
        {
            // skip volume, already handled
            if( q.first == "Volume" )
                continue;

            // Initial value of quality
            myQuality.setValue( q.second, f->getValue( q.first ) );

            // Add plot for quality
            AddPlot( q.first );
        }

        // add plots for adsorbed
        AddPlot( "adsorbed_ethanol" );
        AddPlot( "adsorbed_butanol" );
    }

    /// initialize simulation, automatically called by simulator
    void Start()
    {
        // Sanity test
        try
        {
            tern::theSimulationEngine.Find( myDstID );
        }
        catch ( std::runtime_error& e)
        {
            tern::theSimulationEngine.HandleFatalError( myName + " output is unconnected" );
        }
        try
        {
            tern::theSimulationEngine.Find( myDstID2 );
        }
        catch ( std::runtime_error& e)
        {
            tern::theSimulationEngine.HandleFatalError( myName + " overflow is unconnected" );
        }

        // schedule first outpur
        ScheduleOutput();
    }
    /// Planet has arrived at handler
    int Handle( tern::cEvent* e )
    {
        // base class handles some standard events
        if( cEventHandler::Handle( e ))
            return 1;

        switch( e->myType )
        {

        case 2:
            // output
            if( myLevel )
            {

                // calculate outflow
                double outflow = myOutput;
                if( outflow > myLevel)
                    outflow = myLevel;

                // send outflow on port #1
                tern::cPlanet * planet = new tern::cPlanet( tern::theSimulationEngine );
                // set concentrations
                planet->setQuality( myQuality );
                // set volume
                planet->setQuality( myQualityIndexVolume, outflow );
                tern::theSimulationEngine.Add(
                    planet,
                    1,
                    myDstID,
                    tern::theSimulationEngine.theTime );
                myLevel -= outflow;

                // calculate spillage
                double spillage = 0;
                if( myLevel > myCapacity )
                {
                    spillage = myLevel - myCapacity;
                    planet = new tern::cPlanet( tern::theSimulationEngine );
                    planet->setQuality( myQuality );
                    planet->setQuality( myQualityIndexVolume, spillage );
                    tern::theSimulationEngine.Add(
                        planet,
                        1,
                        myDstID2,
                        tern::theSimulationEngine.theTime );
                    myLevel -= spillage;
                }

                std::cout << myName << " output " << outflow << " spill " << spillage <<  " level " << myLevel << " at " << tern::theSimulationEngine.theTime << std::endl;

            }
            ScheduleOutput();
            break;

        case 1:

            // add arriving flow to level in vessel
            double oldLevel = myLevel;
            double inputVolume = e->myPlanet->getQuality( myQualityIndexVolume );
            myLevel += inputVolume;
            myTotalInput += inputVolume;

            cQuality Q;
            for( auto q : Q )
            {
                if( q.first == "Volume" )
                    continue;

                std::cout << myName << " before input " << q.first
                           <<" " << myQuality.getValue( q.second )
                           <<" "<< e->myPlanet->getQuality( q.second ) << std::endl;

                //update concentrations
                if( myLevel < 0.01 )
                    myQuality.setValue( q.second, 0 );
                else
                    myQuality.setValue( q.second,
                                        ( oldLevel * myQuality.getValue( q.second ) +
                                          inputVolume * e->myPlanet->getQuality( q.second ) ) /
                                        myLevel );

                std::cout << myName << " after input " << q.first
                           <<" " << myQuality.getValue( q.second ) << std::endl;
            }

            // no output, so we are done with the input
            tern::theSimulationEngine.Delete( e->myPlanet );

            // if vessel has become full, record time
            if( myLevel > myCapacity && full_time == 0)
            {
                full_time = tern::theSimulationEngine.theTime;
            }
            break;
        }
        return 1;
    }
    ///  Schedule next output
    void ScheduleOutput()
    {
        tern::theSimulationEngine.Add(
            new tern::cPlanet( tern::theSimulationEngine ),
            2,            // event type
            this,         // handle the event here
            tern::theSimulationEngine.theTime + 1
        );
    }
    void HandlePlotPointEvent()
    {
        double plotlevel = myLevel;
        if( plotlevel > myCapacity )
            plotlevel = myCapacity;
        myPlot[0].myData.push_back( plotlevel );

        // loop over  defined qualities
        int index = 1;
        for( auto q : myQuality )
        {
            // skip volume, already handled
            if( q.first == "Volume" )
                continue;
            myPlot[ index++ ].myData.push_back( myQuality.getValue( q.second ));
        }
    }

    void FinalReport();

private:

    double myOutput;        ///  Maximum rate of output from port #1

    double myTotalInput;
    long long full_time;
    int myQualityIndexVolume;

protected:
    double myCapacity;      ///  Maximum volume held before spill from port #2
    double myLevel;         ///  Current volume held
    cQuality myQuality;
};

}
}
}
