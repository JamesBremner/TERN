#include <cstdlib>
#include <cmath>
#include "Probability.h"

using namespace std;

namespace raven {
    namespace sim {
        namespace prob {

/**

return sample from Poisson distribution.

@param[in] mean  expected mean of samples

@return  sample

This function is declared static, so it can be called without constructing the class.

 e.g.

 double r = cPoisson::ran( 1.0e6 );

 The function is declared inline, so calling this causes no function overhead.

 The output is clipped to the range 0 to 1e6 * mean

*/
 double cPoisson::ran( double mean )
{
	double p = - 1.0 * mean * log ( ( float ) rand() / RAND_MAX );
	if( p < 0 )
		p = 0;
	if( p > 1e6 * mean )
		p = 1e6 * mean;
	return p;
}

/**
  unit tests

  @return  true if passed all tests

*/
 int cPoisson::test()
{
	double total = 0;
	double mean = 15.0;
	for( int k = 0; k < 1000; k++ ) {
		total += ran( mean );
	}
	if( fabs( ( (total / 1000) - mean ) / mean ) > 0.03 )
		return 0;

	total = 0;
	 mean = 0.01;
	for( int k = 0; k < 1000; k++ ) {
		total += ran( mean );
	}
	if( fabs( ( (total / 1000) - mean ) / mean ) > 0.04 )
		return 0;

	return 1;
}

double cNormal::ran( double mean, double dev )		///< return sample from  distribution.
{
	double  r = 0;
	for( int k = 0; k< 12; k++ ) {
		r += rand();
	}
	// [ 0, 12 * RAND_MAX ]
	r -= 6 * RAND_MAX;
	// [ - 6 * RAND_MAX, 6 * RAND_MAX ]
	r *= dev  /  RAND_MAX ;
	// [ - 3 * dev, 3 * dev ]
	r += mean;
	// [ mean - 2 * dev, mean + 2 * dev ];
	return r;
}

int cNormal::test()
{
	double total = 0;
	double mean = 15.0;
	double dev = 1.0;
	for( int k = 0; k < 1000; k++ ) {
		total += ran( mean, dev );
	}
	if( fabs( ( (total / 1000) - mean ) / mean ) > 0.02 )
		return 0;

	//FILE * fp = fopen("normal.txt", "w" );
	//for( int k = 0; k < 10000; k++ ) {
	//	fprintf(fp,"%f\n",ran( mean, dev ) );
	//}
	//fclose(fp);


	return 1;
}

        }
    }
}
