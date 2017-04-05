#pragma once
/**
  The Poisson distribution
  */
class cPoisson
{
public:
	static inline double 
		ran( double mean );		///< return sample from  distribution.
	static int test();
};
/**
  The Normal distribution
*/
class cNormal
{
public:
	static inline double 
		ran( double mean, double dev );		///< return sample from  distribution.
	static int test();
};
