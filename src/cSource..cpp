#include <vector>
#include <map>
#include "tern.h"
#include "cSource.h"
#include "probability.h"

namespace raven
{
namespace sim
{
namespace tern
{
    void cSource::ScheduleArrival()
    {
        // Calculate time of next arrival

        long long next_time = tern::theSimulationEngine.theTime;
        if( myMean < 0.9 )
        {
            // source is disabled
            return;
        }
        else
        {

            if( myfSteady )
            {
                // constant rate
                next_time += myMean;
            }
            else
            {
                // random time from now with specified mean
                next_time += (__int64)raven::sim::prob::cPoisson::ran( myMean );
            }
        }

        // construct the arriving planet
        tern::cPlanet * planet = new tern::cPlanet( tern::theSimulationEngine );
        planet->setQuality( myQuality );
        myTotal++;

        //cout << "cSource::ScheduleArrival " << myMean <<" " << next_time << endl;

        // schedule
        tern::theSimulationEngine.Add(
            planet,       // create a planet, which will wander through simulation recording progress
            1,            // event type
            this,         // handle the event here
            next_time
        );
    }
}
}
}

