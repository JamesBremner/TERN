/** Simulate a machine that stops from time to time */

class cStoppingMachine : public raven::sim::task::cDelay
{
public:

    /** Construct from historical stopping schedule
        @param[in) name of the machine
        @param[in) stopschedule json formatted stopping schedule
    */

    cStoppingMachine(
        const string& name,
        const string& stopschedule );

     /** Construct to use randomly generated stopping schedule
        @param[in] name of the machine
        @param[in] MeanSecsBetweenStops Mean time between a restart and the next stop
        @param[in] MeanSecsStopDuration
        @param[in] DevSecsStopDuration  Standard deviation of stop duration
     */
     cStoppingMachine(
        const string& name,
        int MeanSecsBetweenStops,
        int MeanSecsStopDuration,
        int DevSecsStopDuration );

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
    vector< raven::sim::tern::date_t > myStop;                    // stop times ( calendar )
    vector< raven::sim::tern::date_t > myStart;                   // start times ( calender )
    vector< long long > myStopInSimTime;                        // stop times ( simulation clock )
    vector< long long > myStartInSimTime;                       // start times ( simulation clock )
    static raven::sim::tern::date_t theEarliestStop;              // earliest stop of any machine ( calendar )
    bool myfHistorical;                                          // true if using historical stopping schedule
    int myMeanSecsBetweenStops;
    int myMeanSecsStopDuration;
    int myDevSecsStopDuration;
    int myNextStop;

};

class cStoppingMachineSource : public raven::sim::task::cSource
{
    public:
    cStoppingMachineSource( const string& name, int TimeBetweenInputs )
    : raven::sim::task::cSource( name )
    , myTimeBetweenInputs( TimeBetweenInputs )
    {

    }

    int Delay()
    {
       return myTimeBetweenInputs;
    }

    private:
        int myTimeBetweenInputs;
};
