/** Simulate a machine that stops from time to time */

class cStoppingMachine : public raven::sim::task::cDelay
{
public:

    /** Construct
        @param[in) name of the machine
        @param[in) stopschedule json formatted stopping schedule
    */

    cStoppingMachine(
        const string& name,
        const string& stopschedule );

    /** Initialize

    This converts the stopping schedule into simulation time
    It is automatically called just before simulation run starts
    */
    void Start();

    /** Calculate delay at machine

    Called by the simulation engine

    Return 1 second delay if machine is running
    Return delay until machine restarts if in stop period
    according to schedule

    */

    virtual int Delay( raven::sim::tern::cPlanet * planet );

private:
    vector< raven::sim::tern::date_t > myStop;                    // stop times ( real clock )
    vector< raven::sim::tern::date_t > myStart;                   // start times ( real clock )
    vector< long long > myStopInSimTime;        // stop times ( simulation clock )
    vector< long long > myStartInSimTime;       // start times ( simulation clock )
    static raven::sim::tern::date_t theEarliestStop;              // earlist stop of any machine ( real clock )
};

