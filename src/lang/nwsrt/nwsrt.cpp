#include "yocto/program.hpp"
#include "yocto/lang/compiler.hpp"
#include "yocto/core/list.hpp"
#include "yocto/core/node.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/container/tuple.hpp"

#include "nwsrt.def"

using namespace yocto;
using namespace Lang;

static const char nwsrt_grammar[] =
{
#include "nwsrt.inc"
};

class Int : public object
{
public:
    Int *next,*prev;
    const int value;

    inline explicit Int(const int v) throw() : next(0), prev(0), value(v) {}
    inline virtual ~Int() throw() {}

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Int);
};

typedef core::list_of_cpp<Int> Ints;

typedef core::node_of<string>   Info;
typedef core::list_of_cpp<Info> Infos;

class Name : public object
{
public:
    Name *next,*prev;
    const int    count;
    const string info;
    inline explicit Name(const int c,const string &i) : next(0), prev(0), count(c), info(i) {}
    inline virtual ~Name() throw() {}

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Name);
};
typedef core::list_of_cpp<Name> Names;

class Swap : public object
{
public:
    Swap *next,*prev;
    const int I,J;

    inline virtual ~Swap() throw() {}
    inline explicit Swap(const int i, const int j) throw() : next(0), prev(0), I(i), J(j) {}


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Swap);
};

typedef core::list_of_cpp<Swap> _Swaps;

class Swaps : public object, public _Swaps
{
public:
    Swaps *next, *prev;

    inline explicit Swaps() throw() : _Swaps(), next(0), prev(0) {}
    inline virtual ~Swaps() throw() {}

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Swaps);
};

typedef core::list_of_cpp<Swaps> SwapsList;

class Code : public object
{
public:
    Code *next, *prev;
    auto_ptr<Name>  name;
    auto_ptr<Swaps> swaps;

    inline explicit Code() : next(0), prev(0), name(0), swaps(0) {}
    inline virtual ~Code() throw() {}

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Code);
};

typedef core::list_of_cpp<Code> Codes;



class TheCompiler : public Compiler
{
public:
    Ints       intStack;
    Infos      infoStack;
    Names      nameStack;
    _Swaps     swapStack;
    SwapsList  swapsList;
    Codes      codes;

    explicit TheCompiler() : Compiler(Syntax::Parser::Generate(nwsrt_grammar,sizeof(nwsrt_grammar)))
    {
    }

    virtual ~TheCompiler() throw()
    {
    }

    virtual void onTerminal(const string &label,
                            const int     hCode,
                            const string &content)
    {
        //showTerminal(label,hCode,content);
        switch(hCode)
        {
            case nwsrt_info: infoStack.push_back( new Info(content) ); break;
            case nwsrt_int:   intStack.push_back( new Int( strconv::to_int(content,"Int") )); break;
            default: throw exception("%s: unexpected terminal '%s'", *parser().tag, *label);
        }
    }

    virtual void onInternal(const string &label,
                            const int     hCode,
                            const int     nArgs)
    {
        //showInternal(label,hCode,nArgs);
        switch(hCode)
        {
            case nwsrt_name:
            {
                assert(intStack.size>=1);
                assert(infoStack.size>=1);
                const int     count = intStack.tail->value;
                const string &info  = *infoStack.tail;
                nameStack.push_back( new Name(count,info) );
                delete intStack.pop_back();
                delete infoStack.pop_back();
            } break;

            case nwsrt_swap:
            {
                assert(intStack.size>=2);
                const int j = intStack.tail->value; delete intStack.pop_back();
                const int i = intStack.tail->value; delete intStack.pop_back();
                swapStack.push_back( new Swap(i,j) );
            } break;

            case nwsrt_swaps:
            {
                assert(swapStack.size>=size_t(nArgs));
                Swaps *swaps = new Swaps();
                swapsList.push_back(swaps);
                for(int i=0;i<nArgs;++i)
                {
                    swaps->push_front( swapStack.pop_back() );
                }
            } break;

            case nwsrt_code:
            {
                Code *code = new Code();
                assert(nameStack.size>=1);
                assert(swapsList.size>=1);
                code->name.reset( nameStack.pop_back() );
                code->swaps.reset(swapsList.pop_back()  );
                codes.push_back(code);
            } break;

            case nwsrt_codes:
            {
                std::cerr << "Found #" << codes.size << " codes!" << std::endl;
            } break;
            default: throw exception("%s: unexpected internal '%s'",*parser().tag, *label);
        }
    }

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(TheCompiler);

};



YOCTO_PROGRAM_START()
{
    TheCompiler nws;
    if(argc>1)
    {
        // read and compile codes
        const Module::Handle hModule( new Module(argv[1]) );
        Source               source( hModule );
        nws.ld(source);

        // ready to write
        for(const Code *code = nws.codes.head;code;code=code->next)
        {
            const Name   &name = *(code->name);
            const int     count = name.count;
            const string &info  = name.info;
            const Swaps &swaps = *(code->swaps);

            std::cerr << count << " <" << info << ">" << std::endl;
            for(const Swap *swap = swaps.head;swap; swap=swap->next )
            {
                //std::cerr << "\tswp(" << swap->I << "," << swap->J << ")" << std::endl;
            }
        }

    }

}
YOCTO_PROGRAM_END()

