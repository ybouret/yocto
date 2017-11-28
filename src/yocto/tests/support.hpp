#ifndef __SUPPORT_INCLUDED
#define __SUPPORT_INCLUDED 1

#include "yocto/code/alea.hpp"
#include "yocto/string.hpp"
#include "yocto/math/complex.hpp"

using namespace yocto;


template <typename T>
struct gen
{
	static inline T get() { return alea.full<T>(); }
};

template <>
struct gen<float> { static inline float get() { return alea.get<float>(); } };

template <>
struct gen<double> { static inline double get() { return alea.get<double>(); } };


template <>
struct gen<string> {
	static inline string get()
	{
		string ans;
		for( size_t i= 8 + alea.leq(10); i > 0; --i )
		{
			ans += char('a' + alea.lt(26) );
		}
		return ans;
	}	
};

template <>
struct gen< math::complex<float> >
{
	static inline math::complex<float> get() { return math::complex<float>( alea.get<float>(), alea.get<float>() ); }
};

template <>
struct gen< math::complex<double> >
{
	static inline math::complex<double> get() { return math::complex<double>( alea.get<double>(), alea.get<double>() ); }
};

#endif
