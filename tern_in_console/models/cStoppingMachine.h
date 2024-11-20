/** Simulate a machine that stops from time to time */

class cStoppingMachine : public raven::sim::tern::cDelay
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
        @param[in] Threshold of output buffer that stops machine
     */
     cStoppingMachine(
        const string& name,
        int MeanSecsBetweenStops,
        int MeanSecsStopDuration,
        int DevSecsStopDuration,
        int myThreshold );

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
    Return -1 if external stop

    */

    virtual int Delay( raven::sim::tern::cPlanet * planet );

    virtual int Handle( raven::sim::tern::cEvent* e );

    virtual std::string FinalReportText();
    virtual void FinalReport() { std::cout << FinalReportText(); }


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
    int myThreshold;                                            // level of output buffer which stops machine
    int myNextStop;                                             // time when next stip will occur
    int myTotalStoppedTime;
    int myTotalBlockedTime;
    int myTimeStartBlocked;

    bool IsOutputClear();                                       // true if output buffer threshold does not exceeed threshold

};

//class cStoppingMachineSource : public raven::sim::task::cSource
//{
//    public:
//    cStoppingMachineSource( const string& name, int TimeBetweenInputs )
//    : raven::sim::task::cSource( name )
//    , myTimeBetweenInputs( TimeBetweenInputs )
//    {
//
//    }
//
//    int Delay()
//    {
//       return myTimeBetweenInputs;
//    }
//
//    private:
//        int myTimeBetweenInputs;
//};
