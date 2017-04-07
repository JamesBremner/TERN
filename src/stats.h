
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/variance.hpp>

namespace raven {
namespace sim {
namespace stats {

            // statistics accumulator
    typedef boost::accumulators::accumulator_set<int, boost::accumulators::stats<
    boost::accumulators::tag::min,
    boost::accumulators::tag::max,
    boost::accumulators::tag::mean,
    boost::accumulators::tag::variance,
    boost::accumulators::tag::count> > stats_t;

double poisson( double mean );		            ///< return sample from possion distribution with specified mean.
double normal( double mean, double dev );		///< return sample from normal distribution.
}
}
}
