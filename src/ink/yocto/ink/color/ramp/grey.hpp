#ifndef YOCTO_INK_grey
#define YOCTO_INK_grey 1
#include "yocto/ink/color/ramp.hpp"
namespace yocto { namespace Ink {

	class ramp_grey : public ramp {
		public:
		inline virtual ~ramp_grey() throw() {}
		inline explicit ramp_grey(const float vmin=0.0f,const float vmax=1.0f) throw():
			ramp(vmin,vmax,__colors) {}
		YOCTO_DISABLE_COPY_AND_ASSIGN(ramp_grey);
		static const RGBA __colors[256];
	};

} }
#endif
