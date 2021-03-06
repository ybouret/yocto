#include "yocto/ink/bitmap.hpp"
#include "yocto/fs/vfs.hpp"
#include "yocto/ios/ocstream.hpp"

namespace yocto
{
    namespace Ink
    {

#define Y_INK_IS_BMP    1
#define Y_INK_IS_PPM    2
#define Y_INK_IS_EPS    3

        static inline void __write4( ios::ocstream &fp, const uint32_t v )
        {
            const uint8_t b0(  v     &0xff );
            const uint8_t b1( (v>> 8)&0xff );
            const uint8_t b2( (v>>16)&0xff );
            const uint8_t b3( (v>>24)&0xff );
            fp.write(b0);
            fp.write(b1);
            fp.write(b2);
            fp.write(b3);

        }

        void Bitmap:: save( const string &filename, const bool in_color ) const
        {
            if(depth<3) throw exception("Bitmap::save(depth<3)");
            string ext = vfs::get_extension(filename); ext.to_lower();
            int    tid = 0;
            bool   swp = false;

            if("bmp"==ext)
            {
                tid = Y_INK_IS_BMP;
            }

            if("ppm"==ext)
            {
                tid = Y_INK_IS_PPM;
                swp = true;
            }

            if("eps"==ext)
            {
                tid = Y_INK_IS_EPS;
                swp = true;
            }

            if(tid<=0) throw exception("Bitmap::save(unhandled extension '%s')", *ext);

            // open file
            ios::wcstream fp(filename);
            const int     nx   = w;
            const int     ny   = h;
            int           size = 0;

            // PROLOG
            switch (tid) {
                case Y_INK_IS_PPM:
                    fp("P6\n%d %d\n255\n",nx,ny);
                    break;

                case Y_INK_IS_EPS:
                    fp("%%!PS-Adobe-3.0 EPSF-3.0\n");
                    fp("%%%%Creator: yocto::graphics\n");
                    fp("%%%%BoundingBox: %d %d %d %d\n",0,0,nx,ny);
                    fp("%%%%LanguageLevel: 2\n");
                    fp("%%%%Pages: 1\n");
                    fp("%%%%DocumentData: Clean7Bit\n");
                    fp("%d %d scale\n",nx,ny);
                    fp("%d %d 8 [%d 0 0 -%d 0 %d]\n",nx,ny,nx,ny,ny);
                    fp("{currentfile 3 %d mul string readhexstring pop} bind\n",nx);
                    fp("false %d colorimage\n", int(in_color?3:1));
                    break;

                case Y_INK_IS_BMP:
                    /* Header 10 bytes */
                    fp.write('B');
                    fp.write('M');
                    size = nx * ny * 3 + 14 + 40;
                    __write4(fp,size);
                    __write4(fp,0);

                    /* Offset to image data : 4 bytes*/
                    __write4(fp,14+40);

                    /* Information header 40 bytes */
                    __write4(fp,0x28);
                    __write4(fp,nx);
                    __write4(fp,ny);
                    fp.write(1);  fp.write(0); /* One plane */
                    fp.write(24); fp.write(0); /* 24 bits */
                    /* Compression type == 0 */
                    __write4(fp,0);
                    size = nx * ny * 3;
                    __write4(fp,size);
                    __write4(fp,1);
                    __write4(fp,1);
                    __write4(fp,0);
                    __write4(fp,0);
                    break;

                default:
                    break;
            }

            // BINARY DATA
            int       linelength = 0;
            const int top        = ny-1;
            for(int j=0;j<ny;++j)
            {
                const int rowindex = swp ? (top-j) : j;
                for(int i=0;i<nx;++i)
                {
                    const uint8_t *C = static_cast<const uint8_t *>(get(i,rowindex));
                    const uint8_t  r = C[0];
                    const uint8_t  g = C[1];
                    const uint8_t  b = C[2];
                    const uint8_t  m = Core::GreyScale1(r,g,b);
                    switch (tid) {
                        case Y_INK_IS_BMP:
                            if(in_color)
                            {
                               // std::cerr << "r=" << int(r) << std::endl;
                                fp.write(b); fp.write(g); fp.write(r);
                            }
                            else
                            {
                                fp.write(m); fp.write(m); fp.write(m);
                            }
                            break;

                        case Y_INK_IS_PPM:
                            if(in_color)
                            {
                                fp.write(r); fp.write(g); fp.write(b);
                            }
                            else
                            {
                                fp.write(m); fp.write(m); fp.write(m);
                            }
                            break;

                        case Y_INK_IS_EPS:
                            if(in_color)
                            {
                                fp("%02x%02x%02x",r,g,b);
                                linelength += 6;
                            }
                            else
                            {
                                fp("%02x",m);
                                linelength+=2;
                            }
                            if (linelength >= 72 || linelength >= nx) {
                                fp("\n");
                                linelength = 0;
                            }
                            break;

                        default:
                            break;
                    }
                }
            }

            // EPILOG
            switch(tid)
            {
                case Y_INK_IS_EPS:
                    fp("\n%%%%EOF\n");
                    break;

                default:
                    break;
            }

        }

    }

}
