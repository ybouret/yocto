    #ifndef YOCTO_RANDOM_INCLUDED
#define YOCTO_RANDOM_INCLUDED 1

#include "yocto/random/bits.hpp"

namespace yocto {
	
	namespace Random 
	{
				
		class Uniform : public object
		{
		public:
			virtual ~Uniform() throw();
			
			virtual double operator()(void)     throw() = 0;
			virtual void   seed( Bits &   )     throw() = 0;

            void onSphere(double &x, double &y, double &z) throw();
            void onSphere(float  &x, float  &y, float  &z) throw();

            void inBall(double &x, double &y, double &z) throw();

            void onCircle(double &x, double &y) throw();
            void onCircle(float  &x, float  &y) throw();

            template<typename POINT3D>
            inline POINT3D getUnit3D() throw()
            {
                POINT3D p;
                onSphere(p.x,p.y,p.z);
                return p;
            }

            template<typename POINT2D>
            inline POINT2D getUnit2D() throw()
            {
                POINT2D p;
                onCircle(p.x,p.y);
                return p;
            }

		protected:
			explicit Uniform() throw();
			
		private:
			YOCTO_DISABLE_COPY_AND_ASSIGN(Uniform);
		};
		
		
	}
	
}

#endif
