#pragma once
namespace raven {
    namespace sim {
        namespace prob {
/**
  The Poisson distribution
  */
class cPoisson
{
public:
	static double
		ran( double mean );		///< return sample from  distribution.
	static int test();
};
/**
  The Normal distribution
*/
class cNormal
{
public:
	static double
		ran( double mean, double dev );		///< return sample from  distribution.
	static int test();
};
        }
    }
}
