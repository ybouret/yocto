#ifndef YOCTO_INK_orange
#define YOCTO_INK_orange 1
#include "yocto/ink/color/ramp.hpp"
namespace yocto { namespace Ink {

	class ramp_orange : public ramp {
		public:
		inline virtual ~ramp_orange() throw() {}
		inline explicit ramp_orange(const float vmin=0.0f,const float vmax=1.0f) throw():
			ramp(vmin,vmax,__colors) {}
		YOCTO_DISABLE_COPY_AND_ASSIGN(ramp_orange);
		static const RGBA __colors[256];
	};

} }
#endif
