#ifndef YOCTO_INK_orange
#define YOCTO_INK_orange 1
#include "yocto/ink/color/ramp.hpp"
namespace yocto { namespace Ink {

	class orange : public ramp {
		public:
		inline virtual ~orange() throw() {}
		inline explicit orange(const float vmin,const float vmax) throw():
			ramp(vmin,vmax,__colors) {}
		YOCTO_DISABLE_COPY_AND_ASSIGN(orange);
		static const RGBA __colors[256];
	};

} }
#endif
