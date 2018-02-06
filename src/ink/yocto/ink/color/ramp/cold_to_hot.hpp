#ifndef YOCTO_INK_cold_to_hot
#define YOCTO_INK_cold_to_hot 1
#include "yocto/ink/color/ramp.hpp"
namespace yocto { namespace Ink {

	class cold_to_hot : public ramp {
		public:
		inline virtual ~cold_to_hot() throw() {}
		inline explicit cold_to_hot(const float vmin,const float vmax) throw():
			ramp(vmin,vmax,__colors) {}
		YOCTO_DISABLE_COPY_AND_ASSIGN(cold_to_hot);
		static const RGBA __colors[256];
	};

} }
#endif
