
#include "yocto/lang/compiler.hpp"

#include "yocto/program.hpp"
#include "yocto/ios/graphviz.hpp"
#include "yocto/container/tuple.hpp"
#include "yocto/core/node.hpp"
#include "yocto/core/list.hpp"
#include "yocto/string/conv.hpp"

using namespace yocto;
using namespace Lang;

static const char nscode[] =
{
#include "netsort.inc"
};

#include "netsort.def"

class Size : public object
{
public:
    Size *next, *prev;
    const int value;
    inline explicit Size(const int v) throw() : next(0), prev(0), value(v) {}
    inline virtual ~Size() throw() {}

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Size);
};
typedef core::list_of_cpp<Size> Sizes;

YOCTO_PAIR_DECL(YOCTO_TUPLE_STANDARD,_Pair,int,I,int,J);
inline _Pair() throw() : I(0), J(0) {}
YOCTO_PAIR_END();

typedef core::node_of<_Pair>      Pair;
typedef core::list_of_cpp<Pair>  _Line;

typedef core::node_of<_Line>      Line;
typedef core::list_of_cpp<Line>   Code;

class _Algo : public Code
{
public:
    const size_t dim;
    inline _Algo(const size_t n) throw() : Code(), dim(n)  {}
    inline virtual ~_Algo() throw() {}

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(_Algo);
};

typedef core::node_of<_Algo>    Algo;
typedef core::list_of_cpp<Algo> AlgoList;

class NetSort_Compiler : public Compiler
{
public:
    Sizes sizes; // sizes stack
    _Line pairs; // pairs stack
    Code  lines; // lines stack

    explicit NetSort_Compiler() :
    Compiler(Syntax::Parser::Generate(nscode,sizeof(nscode))),
    sizes(),
    pairs(),
    lines()
    {
    }

    virtual ~NetSort_Compiler() throw()
    {
    }


    virtual void initialize()
    {
        std::cerr << "Initializing..." << std::endl;
    }

    virtual void onTerminal(const string &label,
                            const int     hCode,
                            const string &content)
    {
        __indent() << "push "; __output(label) << " '" << content << "'" << std::endl;

        switch(hCode)
        {
            case netsort_size:
                sizes.push_back( new Size(strconv::to_int(content,"size")) );
                break;

            default:
                throw exception("unexpected terminal %s", *label);
        }
    }

    virtual void onInternal(const string &label,
                            const int     hCode,
                            const int     nArgs)
    {
        __indent() << "call " << (label) << "/" << nArgs << std::endl;
        switch(hCode)
        {
            case netsort_pair:{
                assert(2==nArgs);
                assert(sizes.size>=2);
                Pair *p = new Pair();
                p->J = sizes.tail->value; delete sizes.pop_back();
                p->I = sizes.tail->value; delete sizes.pop_back();
                //std::cerr << "Pair: " << p->I << ", " << p->J << std::endl; exit(1);
                pairs.push_back(p);
            } break;

            case netsort_line: {
                assert(pairs.size>=nArgs);
                Line *l = new Line();
                for(size_t i=0;i<nArgs;++i)
                {
                    l->push_front( pairs.pop_back() );
                }
                lines.push_back(l);
            } break;

            case netsort_network: {
                assert(lines.size>=nArgs);
                assert(sizes.size>=1);
                const size_t nn=0;
            } break;

            default:
                break;
        };
    }


};

YOCTO_PROGRAM_START()
{
    NetSort_Compiler     nsCompiler;
    const Module::Handle hModule( new Module() );
    Source               source( hModule );

    nsCompiler.ld(source);
}
YOCTO_PROGRAM_END()

