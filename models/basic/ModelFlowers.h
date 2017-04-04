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
#ifdef tern_build
namespace tern
{

bool ConstructModelFlowers( raven::sim::gui::cFlower * f );

}
#endif // tern_build
}
}
