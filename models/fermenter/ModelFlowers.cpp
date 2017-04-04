#include <cmath>
#include "flower.h"
#include "ModelFlowers.h"

namespace raven
{
namespace sim
{
void cFlowerDict::InsertModelTypes()
{
    /* TODO: Add code here to ID the new flower types used by your model

    It should look like this:

     Insert( L"name_of_first_model_flower" );
     Insert( L"name_of_second_model_flower" );
    */

    Insert( L"Fermenter" );
    Insert( L"Adsorber" );

}
namespace gui
{

cFlower * cFlowerFactory::ConstructModelFlower( int flower_type_index )
{

    /* TODO: Add code here to call the constructors of the new flower types used by your model

     It should look like this:

    if( flower_type_index == myFlowerDict->Find( L"name_of_first_model_flower" ))
        return new cFirstModelFlower();
    else
        return nullptr;

    */

    if( flower_type_index == myFlowerDict->Find( L"Fermenter" ))
        return new cFermenter();
    else if( flower_type_index == myFlowerDict->Find( L"Adsorber" ))
        return new cAdsorber();
    else
        return nullptr;
}

cFermenter::cFermenter()
{
    myTypeName = L"Fermenter";
    setName();
    AddParam( L"EthanolGrowthRate", L" g/g " );
    AddParam( L"ButanolGrowthRate", L" g/g " );
    AddParam( L"SugarGrowthRate", L" g/g " );
}

cAdsorber::cAdsorber()
{
    myTypeName = L"Adsorber";
    setName();
    AddParam( L"EthanolAdsorptionRate", L"Fraction removed per hour" );
    AddParam( L"ButanolAdsorptionRate", L"Fraction removed per hour" );
}
}
#ifdef tern_build
namespace tern
{
bool ConstructModelFlowers( raven::sim::gui::cFlower * f )
{
    if( f->getType() == raven::sim::gui::cFlowerFactory::Index(L"Fermenter") )
    {
        new tern::cFermenter( f );
        return true;
    }
    else if( f->getType() == raven::sim::gui::cFlowerFactory::Index(L"Adsorber") )
    {
        new tern::cAdsorber( f );
        return true;
    }
    else
    {
        return false;
    }
}

cFermenter::cFermenter(  raven::sim::gui::cFlower * f  )
    : cFunnel( f )
    , specific_ethanol_growth_rate( f->getValue(L"EthanolGrowthRate") )
    , specific_butanol_growth_rate(  f->getValue(L"ButanolGrowthRate") )
    , specific_sugar_growth_rate(  f->getValue(L"SugarGrowthRate") )
    , specific_yeast_growth_rate( 0.0625 )
{

}

int cFermenter::Handle( tern::cEvent* e )
{
    if( e->myType == 2 )
    {
        // outflow event, occurs once per simulation clock tick

        // initialize quality indices
        static bool init = false;
        static int Ethanol, Butanol, Sugar, Yeast;
        if( ! init )
        {
            init = true;
            Ethanol = cQuality::getIndex( L"Ethanol" );
            Butanol = cQuality::getIndex( L"Butanol" );
            Sugar = cQuality::getIndex( L"Sugar" );
            Yeast = cQuality::getIndex( L"Yeast" );
        }

        // update concentrations due to activity of yeast

        double yeast = myQuality.getValue( Yeast );

        myQuality.setValue( Ethanol,
                            myQuality.getValue( Ethanol ) +
                            specific_ethanol_growth_rate * yeast );

        myQuality.setValue( Butanol,
                            myQuality.getValue( Butanol ) +
                            specific_butanol_growth_rate * yeast );

        myQuality.setValue( Sugar,
                            myQuality.getValue( Sugar ) +
                            specific_sugar_growth_rate * yeast );

        // yeast growth

        /* Values from table 1 */
        const double cellmass_stops_growth = 9.806;
        const double cellmass_inhibition_constant = 0.4583;
        const double butanol_stops_growth = 12.57;
        const double butanol_inhibition_constant = 0.0159;

        /* estimated values of μm, KS, and KI for the growth of
        the recombinant Clostridium acetobutylicum under substrate limiting
        conditions are 0.238 h−1, 0.357 g/L, and 272.3 g/L,
        respectively.
        */

        const double cell_growth_rate_max = 0.238;
        const double substrate_saturation_constant = 0.357;
        const double substrate_inhibition_constant = 272.3;

        // sugar concentration
        double sugar = myQuality.getValue( Sugar );

        // yeast growth
        double butane_inhibition_factor =
            pow( 1 - myQuality.getValue( Butanol ) / butanol_stops_growth, butanol_inhibition_constant );

        double cellmass_inhibition_factor = 0;
        if( ( yeast * myLevel ) < cellmass_stops_growth )
            cellmass_inhibition_factor = pow( 1 - ( yeast * myLevel ) / cellmass_stops_growth, cellmass_inhibition_constant );

        double substrate_inhibition_factor =
            1 / ( substrate_saturation_constant + sugar + sugar * sugar / substrate_inhibition_constant );

        double r =
            cell_growth_rate_max * sugar * butane_inhibition_factor * cellmass_inhibition_factor * substrate_inhibition_factor;


        myQuality.setValue( Yeast,
                            ( yeast * myLevel + r * myQuality.getValue( Sugar ) ) / myLevel );

    }

    cFunnel::Handle( e );

}

cAdsorber::cAdsorber(  raven::sim::gui::cFlower * f  )
    : cFunnel( f )
    , ethanol_adsorption_rate( f->getValue(L"EthanolAdsorptionRate") )
    , butanol_adsorption_rate( f->getValue(L"ButanolAdsorptionRate") )
    , ethanol_adsorbed( 0 )
    , butanol_adsorbed( 0 )
{

}

int cAdsorber::Handle( tern::cEvent* e )
{
    if( e->myType == 2 )
    {
        // outflow event, occurs once per simulation clock tick

        // initialize quality indices
        static bool init = false;
        static int Volume, Ethanol, Butanol, Sugar, Yeast;
        if( ! init )
        {
            init = true;
            Ethanol = cQuality::getIndex( L"Ethanol" );
            Butanol = cQuality::getIndex( L"Butanol" );
        }

        // Reduce concentrations due to adsorption
        ethanol_adsorbed += ethanol_adsorption_rate * myQuality.getValue( Ethanol );
        butanol_adsorbed += butanol_adsorption_rate * myQuality.getValue( Butanol ) ;
        myQuality.setValue( Ethanol,
                            ( 1 - ethanol_adsorption_rate ) * myQuality.getValue( Ethanol ) );
        myQuality.setValue( Butanol,
                            ( 1 - butanol_adsorption_rate ) * myQuality.getValue( Butanol ) );

    }

    cFunnel::Handle( e );
}

void cAdsorber::HandlePlotPointEvent()
{
    cFunnel::HandlePlotPointEvent();

    myPlot[ cQuality::size() ].myData.push_back( ethanol_adsorbed );
    myPlot[ cQuality::size() + 1 ].myData.push_back( butanol_adsorbed );
}
}
#endif

}
}


