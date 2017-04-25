#pragma once
#include "flower_types.h"
#ifdef tern_vase
#include "tern.h"
#include "cDelay.h"
#endif

namespace raven
{
namespace sim
{
namespace gui
{

// TODO Replace with class interfaces of the new flower types used by your model

class cStoppingMachine : public cFlower
{
public:
    cStoppingMachine();
    void Start();


};


}
}
}

#ifdef tern_vase

#include "tern.h"

namespace raven
{
namespace sim
{
namespace tern
{

bool ConstructModelFlowers( raven::sim::gui::cFlower * f );

class cStoppingMachine : public tern::cDelay
{
public:
    cStoppingMachine( raven::sim::gui::cFlower * f );

    virtual void Start();

    virtual int Delay( tern::cPlanet * planet );

    virtual int Handle( tern::cEvent* e );

private:
    bool myfRunning;
    int myNextStop;
    int myTimeBetweenStops;
    int myDurationStops;
};

}

}
}
#endif // tern_build
