#include "yocto/program.hpp"
#include "yocto/ios/icstream.hpp"
#include "yocto/string/tokenizer.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/string/conv.hpp"

#include <cstring>

using namespace yocto;

struct RGB
{
    int indx;
    int r;
    int g;
    int b;
};

static RGB color[256];

static inline bool is_sep(const char C) throw() { return C==' ' || C=='\t' ; }

static inline int get_byte(const string &word, const char *name, const unsigned iline)
{
    const int value = strconv::to_int(word,name);
    if(value<0||value>255) throw exception("line %u: %s=%d is not a valid byte",iline,name,value);
    return value;
}

YOCTO_PROGRAM_START()
{

    for(size_t i=0;i<256;++i)
    {
        color[i].indx = -1;
        color[i].r = color[i].g = color[i].b = 0;
    }

    if(argc<=2) throw exception("usage: %s ramp.dat output_dir",program);
    const string ramp_file  = argv[1];
    const string output_dir = argv[2];

    // loading file
    {
        string line;
        vector<string> words(32,as_capacity);
        ios::icstream fp(ramp_file);
        unsigned iline = 0;
        while( fp.gets(line) )
        {
            ++iline;
            words.free();
            tokenizer::split(words,line,is_sep);
            const unsigned nw = words.size();
            if( 0 != (nw%4) ) throw exception("%u%s: not a multiple of 4 words", iline, *ramp_file);
            const unsigned nc = nw/4;
            unsigned       j  = 0;
            for(unsigned i=1;i<=nc;++i)
            {
                const int indx = get_byte(words[++j],"indx",iline);
                //std::cerr << "set color[" << indx << "]" << std::endl;
                RGB      &C = color[indx];
                if(C.indx>=0) throw exception("%u:%s: color[%d] already set!",iline,*ramp_file,indx);
                C.indx = indx;
                C.r = get_byte(words[++j],"r",iline);
                C.g = get_byte(words[++j],"g",iline);
                C.b = get_byte(words[++j],"b",iline);
            }
        }
    }

    for(unsigned i=0;i<256;++i)
    {
        if(color[i].indx<0) throw exception("%s: missing color[%u]", *ramp_file,i);
    }

    // preparing files
    

}
YOCTO_PROGRAM_END()

