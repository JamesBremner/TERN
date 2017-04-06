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
#ifdef tern_build
#include "tern.h"
namespace tern
{

bool ConstructModelFlowers( raven::sim::gui::cFlower * f );

class cStoppingMachine : public cEventHandler
{
public:
    cStoppingMachine( raven::sim::gui::cFlower * f )
};

}
#endif // tern_build
}
}
