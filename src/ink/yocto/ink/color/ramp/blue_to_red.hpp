#ifndef YOCTO_INK_blue_to_red
#define YOCTO_INK_blue_to_red 1
#include "yocto/ink/color/ramp.hpp"
namespace yocto { namespace Ink {

	class ramp_blue_to_red : public ramp {
		public:
		inline virtual ~ramp_blue_to_red() throw() {}
		inline explicit ramp_blue_to_red(const float vmin=0.0f,const float vmax=1.0f) throw():
			ramp(vmin,vmax,__colors) {}
		YOCTO_DISABLE_COPY_AND_ASSIGN(ramp_blue_to_red);
		static const RGBA __colors[256];
	};

} }
#endif
