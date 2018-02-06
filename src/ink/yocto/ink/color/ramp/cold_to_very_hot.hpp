#ifndef YOCTO_INK_cold_to_very_hot
#define YOCTO_INK_cold_to_very_hot 1
#include "yocto/ink/color/ramp.hpp"
namespace yocto { namespace Ink {

	class ramp_cold_to_very_hot : public ramp {
		public:
		inline virtual ~ramp_cold_to_very_hot() throw() {}
		inline explicit ramp_cold_to_very_hot(const float vmin=0.0f,const float vmax=1.0f) throw():
			ramp(vmin,vmax,__colors) {}
		YOCTO_DISABLE_COPY_AND_ASSIGN(ramp_cold_to_very_hot);
		static const RGBA __colors[256];
	};

} }
#endif
