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

        return nullptr;
}

/* TODO: Add code here to implement constructor of new flower in VASE

  It should look like this

  cFirstModelFlower::cFirstModelFlower()
  {
      myTypeName = L"name_of_first_model_flower";
      setName();
      AddParam(L"FirstParameterName", L" Description ", default_value );
  }
 */

}

#ifdef vase
namespace tern
{
bool ConstructModelFlowers( raven::sim::gui::cFlower * f )
{
	/* TODO: Add code here to call the constructors of new flowers in TERN

	It should lookm like this

	    if( f->getType() == raven::sim::gui::cFlowerFactory::Index(L"name_of_first_model_flower") )
	    {
	        new tern::cFirstModelFlower( f );
	        return true;
	    }
	    else
	    {
	        return false;
    }

    */

        return false;

}
}
#endif
}
}
