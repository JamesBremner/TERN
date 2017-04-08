#include <iostream>
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

    Insert( "StoppingMachine");

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

    if( flower_type_index == myFlowerDict->Find( "StoppingMachine" ))
        return new cStoppingMachine();
    else

        return nullptr;
}

cStoppingMachine::cStoppingMachine()
{
    myTypeName = "StoppingMachine";
    setName();
    AddParam("TimeBetweenStops", " secs ", 10 );
    AddParam("DurationStops", " secs ", 3 );
    AddParam("StdDevStops", " secs ", 1 );
}

}

#ifdef tern_build
namespace tern
{
bool ConstructModelFlowers( raven::sim::gui::cFlower * f )
{
    if( f->getType() == raven::sim::gui::cFlowerFactory::Index("StoppingMachine") )
    {
        new tern::cStoppingMachine( f );
        return true;
    }
    else
    {
        return false;
    }

}
cStoppingMachine::cStoppingMachine( raven::sim::gui::cFlower * f )
    : cDelay( f->getName() )
{

}

//     int Delay()
//    {
//        return 1;
//    }

}
#endif
}
}
