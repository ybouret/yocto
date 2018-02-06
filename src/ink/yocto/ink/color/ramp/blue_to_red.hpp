#ifndef YOCTO_INK_blue_to_red
#define YOCTO_INK_blue_to_red 1
#include "yocto/ink/color/ramp.hpp"
namespace yocto { namespace Ink {

	class blue_to_red : public ramp {
		public:
		inline virtual ~blue_to_red() throw() {}
		inline explicit blue_to_red(const float vmin,const float vmax) throw():
			ramp(vmin,vmax,__colors) {}
		YOCTO_DISABLE_COPY_AND_ASSIGN(blue_to_red);
		static const RGBA __colors[256];
	};

} }
#endif
