#pragma once
#include "flower_types.h"
#ifdef tern_build
#include "cFunnel.h"
#endif // tern_build

namespace raven
{
namespace sim
{
namespace gui
{
class cFermenter : public cFunnel
{
public:
    cFermenter();
};

class cAdsorber : public cFunnel
{
public:
    cAdsorber();
};

}

#ifdef tern_build
namespace tern
{
class cFermenter : public cFunnel
{
public:
    cFermenter(  raven::sim::gui::cFlower * f  );

    int Handle( tern::cEvent* e );

    double specific_ethanol_growth_rate;
    double specific_butanol_growth_rate;
    double specific_sugar_growth_rate;
    double specific_yeast_growth_rate;

};

class cAdsorber : public cFunnel
{
public:
    cAdsorber(  raven::sim::gui::cFlower * f  );

    int Handle( tern::cEvent* e );

    void HandlePlotPointEvent();

    double ethanol_adsorption_rate;
    double butanol_adsorption_rate;
    double ethanol_adsorbed;
    double butanol_adsorbed;
};

bool ConstructModelFlowers( raven::sim::gui::cFlower * f );

}
#endif // tern_build
}
}
