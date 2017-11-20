#include "yocto/random/uniform.hpp"
#include <math.h>

namespace yocto {
	
	namespace Random  {
		
		Uniform::  Uniform() throw() {}
		Uniform:: ~Uniform() throw() {}

        void Uniform:: onCircle(double &x, double &y) throw()
        {
            Uniform &ran = *this;
            while(true)
            {
                const double r1  = ran();
                const double r2  = ran();
                const double x1  = r1+r1-1.0;
                const double x2  = r2+r2-1.0;
                const double x12 = x1*x1;
                const double x22 = x2*x2;
                const double s2  = (x12) + (x22);
                if(s2>=1.0||s2<=0.0) continue;
                x=(x12-x22)/s2;
                const double hp = x1*x2;
                y=(hp+hp)/s2;
                break;
            }
        }

        void Uniform:: onCircle(float &x, float &y) throw()
        {
            Uniform &ran = *this;
            while(true)
            {
                const float r1  = float(ran());
                const float r2  = float(ran());
                const float x1  = r1+r1-1.0f;
                const float x2  = r2+r2-1.0f;
                const float x12 = x1*x1;
                const float x22 = x2*x2;
                const float s2  = (x12) + (x22);
                if(s2>=1.0f||s2<=0.0f) continue;
                x=(x12-x22)/s2;
                const float hp = x1*x2;
                y=(hp+hp)/s2;
                break;
            }
        }

        void Uniform:: onSphere(double &x, double &y, double &z) throw()
        {
            Uniform &ran = *this;
            while(true)
            {
                const double r1 = ran();
                const double r2 = ran();
                const double x1 = r1+r1-1.0;
                const double x2 = r2+r2-1.0;
                const double s2 = (x1*x1) + (x2*x2);
                if(s2>=1.0) continue;
                const double sq = sqrt(1.0-s2);
                const double hx = x1*sq;
                const double hy = x2*sq;
                z = 1.0 - (s2+s2);
                x = hx+hx;
                y = hy+hy;
                break;
            }
        }

        void Uniform:: onSphere(float &x, float &y, float &z) throw()
        {
            Uniform &ran = *this;
            while(true)
            {
                const float r1 = float(ran());
                const float r2 = float(ran());
                const float x1 = r1+r1-1.0f;
                const float x2 = r2+r2-1.0f;
                const float s2 = (x1*x1) + (x2*x2);
                if(s2>=1.0f) continue;
                const float sq = sqrtf(1.0f-s2);
                const float hx = x1*sq;
                const float hy = x2*sq;
                z = 1.0f - (s2+s2);
                x = hx+hx;
                y = hy+hy;
                break;
            }
        }

        void Uniform:: inBall(double &x, double &y, double &z) throw()
        {
            Uniform &ran = *this;
            while(true)
            {
                double xx = ran();
                double yy = ran();
                double zz = ran();
                xx += xx - 1.0;
                const double xx2 = xx*xx;
                yy += yy - 1.0;
                const double yy2 = yy*yy;
                zz += zz - 1.0;
                if(xx2+yy2+(zz*zz)>=1.0) continue;
                x = xx;
                y = yy;
                z = zz;
                break;
            }
        }
	}
	
}
