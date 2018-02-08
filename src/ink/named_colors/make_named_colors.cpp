#include "yocto/program.hpp"
#include "yocto/ios/icstream.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/string/tokenizer.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/ordered/sorted-vector.hpp"
#include "yocto/code/utils.hpp"

using namespace yocto;

static inline bool is_sep(char C) throw() { return (' ' == C) || ('\t' == C); }

class color
{
public:
    const string name;
    const int    r;
    const int    g;
    const int    b;
    inline color(const string &id,const int R, const int G, const int B) :
    name(id), r(R), g(G), b(B)
    {
    }
    
    inline ~color() throw()
    {
    }
    
    color(const color &other) : name(other.name), r(other.r), g(other.g), b(other.b) {}
    
    YOCTO_DISABLE_ASSIGN(color);
    inline friend bool operator<(const color &lhs, const color &rhs) throw()
    {
        return lhs.name < rhs.name;
    }
};

static int get_byte(const string &name, const string &s, const char *field)
{
    const string ctx = name + "." + field;
    const int ans = strconv::to_int(s,*ctx);
    if(ans<0||ans>255) throw exception("%s is not a byte", *ctx);
    return ans;
}

YOCTO_PROGRAM_START()
{
    if(argc<=1) throw exception("usage: %s colors.db",program);
    ios::wcstream hdr("colors-db.hpp");
    ios::wcstream src("colors-db.inc");
    ios::icstream fp(argv[1]);
    sorted_vector<color> colors(256,as_capacity);
    string line;
    vector<string> words(16,as_capacity);
    size_t max_len = 0;
    while( fp.gets(line) )
    {
        words.free();
        tokenizer::split(words, line, is_sep);
        if(words.size()<=4)
            continue;
        std::cerr << words << std::endl;
        words[1].to_upper();
        const string &name = words[1];
        const int     r    = get_byte(name, words[2], "r");
        const int     g    = get_byte(name, words[3], "g");
        const int     b    = get_byte(name, words[4], "b");
        const color   C(name,r,g,b);
        if(!colors.insert(C))
        {
            throw exception("multiple '%s'",*name);
        }
        max_len = max_of(max_len,name.size());
    }
    const size_t num_colors = colors.size();
    std::cerr << "#colors=" << num_colors << std::endl;
    hdr << "#ifndef YOCTO_INK_COLORS_DB_INCLUDED\n";
    hdr << "#define YOCTO_INK_COLORS_DB_INCLUDED 1\n";
    for(size_t i=1;i<=num_colors;++i)
    {
        const color &C = colors[i];
        hdr << "#define Y_" << C.name;
        src << "{ \"" << C.name << "\",";
        for(size_t j=C.name.size();j<=max_len;++j)
        {
            hdr << ' ';
            src << ' ';
        }
        const string hdr_rgb = vformat("0x%02x,0x%02x,0x%02x",C.r,C.g,C.b);
        hdr << hdr_rgb;
        const string src_rgb = vformat(" 0x%02x, 0x%02x, 0x%02x",C.r,C.g,C.b);
        src << src_rgb;
        src << " }";
        hdr << '\n';
        if(i<num_colors) src << ',';
        src << '\n';
    }
    hdr << "#endif\n";
}
YOCTO_PROGRAM_END()

