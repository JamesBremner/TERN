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
}
}

#ifdef tern_build

#include "tern.h"

namespace raven
{
namespace sim
{
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
    , myDurationStops( f->getValue( "DurationStops"))
    , myTimeBetweenStops( f->getValue( "TimeBetweenStops" ))
{

}

void cStoppingMachine::Start()
{
    myfRunning = true;
    myNextStop = poisson_distribution( myTimeBetweenStops );
}

int cStoppingMachine::Handle( tern::cEvent* e )
{
//    int dbg = e->myType;
//    std::cout << "cStoppingMachine::Handle " << e->myType << "\n";

    if( cDelay::Handle( e ))
        return 1;

    switch( e->myType )
    {
    case 3:
        // restart
        myfRunning = true;

        // next stop
        myNextStop = tern::theSimulationEngine.theTime + poisson_distribution( myTimeBetweenStops );

        // process anything on queue
        ScheduleCompletion();

        std::cout << getName() << " restarted at " << tern::theSimulationEngine.theTime << "\n";

        return 1;

    default:
        return 0;
    }

}

int cStoppingMachine::Delay( tern::cPlanet * planet )
{
    const int delay_when_running = 1;

    long long T = tern::theSimulationEngine.theTime;

    if( ! myfRunning )
    {
        return -1;
    }

    if( T < myNextStop )
    {
        // normal operation
        return delay_when_running;
    }
    else
    {
        // stop;
        myfRunning = false;

        // schedule restart
        int NextStart = T + poisson_distribution( myDurationStops );
        if( NextStart == T )
            NextStart += 1;

        std::cout << getName() << " stopped until " << NextStart << "\n";

        tern::theSimulationEngine.Add(
            new tern::cPlanet( tern::theSimulationEngine ),
            3,
            this,
            NextStart
        );

        return -1;
    }
}


}

}

}

#endif
