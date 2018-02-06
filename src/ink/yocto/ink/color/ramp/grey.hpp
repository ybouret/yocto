#ifndef YOCTO_INK_grey
#define YOCTO_INK_grey 1
#include "yocto/ink/color/ramp.hpp"
namespace yocto { namespace Ink {

	class grey : public ramp {
		public:
		inline virtual ~grey() throw() {}
		inline explicit grey(const float vmin,const float vmax) throw():
			ramp(vmin,vmax,__colors) {}
		YOCTO_DISABLE_COPY_AND_ASSIGN(grey);
		static const RGBA __colors[256];
	};

} }
#endif
