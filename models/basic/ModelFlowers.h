#pragma once
#include "flower_types.h"

namespace raven
{
namespace sim
{
namespace gui
{

    // TODO Replace with class interfaces of the new flower types used by your model

    class cFirstModelFlower : public cFunnel
    {

    };

}
}
}

#ifdef vase

#include "tern.h"

namespace raven
{
namespace sim
{
namespace tern
{

bool ConstructModelFlowers( raven::sim::gui::cFlower * f );

// TODO: Add class interfaces for new flower types in vase

}
}
}
#endif // tern_build

