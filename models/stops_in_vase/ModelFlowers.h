#pragma once
#include "flower_types.h"

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
};


}
}
}

#ifdef tern_build

#include "tern.h"
#include "task.h"

namespace raven
{
namespace sim
{
namespace tern
{

bool ConstructModelFlowers( raven::sim::gui::cFlower * f );

class cStoppingMachine : public task::cDelay
{
public:
    cStoppingMachine( raven::sim::gui::cFlower * f );

  //  virtual int Delay();
};

}

}
}
#endif // tern_build
