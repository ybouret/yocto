#include "yocto/json/value.hpp"
#include "yocto/threading/singleton.hpp"
#include "yocto/lang/compiler.hpp"
#include "yocto/exception.hpp"
#include "yocto/stock/stack.hpp"
#include "yocto/string/conv.hpp"

#include "json.def"

namespace yocto
{
    using namespace Lang;

    namespace JSON
    {

        static const char jCode[] =
        {
#include "json.inc"
        };

        class jCompiler : public singleton<jCompiler>, public Compiler
        {
        public:
            explicit jCompiler() :
            Compiler( Syntax::Parser::CompileData("embedded JSON grammar",jCode, sizeof(jCode) ) ),
            value(),
            vStack(),
            pStack(),
            nil()
            {
            }

            virtual ~jCompiler() throw()
            {
            }

            typedef vector<Value> ValueHolder;
            typedef vector<Pair>  PairHolder;

            Value                     value;
            stack<Value,ValueHolder>  vStack;
            stack<Pair,PairHolder>    pStack;
            const Value               nil;

            virtual void initialize()
            {
                std::cerr << "<JSON>" << std::endl;
                value.clear();
                vStack.free();
                pStack.free();
            }

            virtual void onTerminal(const string &label,
                                    const int     hCode,
                                    const string &content)
            {
                __indent() << "push "; __output(label) << " '" << content << "'" << std::endl;

                switch(hCode)
                {
                    case JSON_number: {
                        const Value number = strconv::to<double>(content,"JSON.Number");
                        vStack.push(number);
                    } break;

                    case JSON_string: {
                        const Value str = content;
                        vStack.push(str);
                    } break;

                    case JSON_null:   { const Value v(JSON::IsNull);  vStack.push(v); } break;
                    case JSON_true:   { const Value v(JSON::IsTrue);  vStack.push(v); } break;
                    case JSON_false:  { const Value v(JSON::IsFalse); vStack.push(v); } break;
                    default:
                        throw exception("%s: unexpected terminal %s", name, *label);
                }
            }

            virtual void onInternal(const string &label,
                                    const int     hCode,
                                    const int     nArgs)
            {
                __indent() << "call " << (label) << "/" << nArgs << std::endl;
                switch(hCode)
                {
                    case JSON_empty_object: {
                        assert(0==nArgs);
                        Value v(JSON::IsObject);
                        vStack.push(nil);
                        vStack.peek().swap_with(v);
                    } break;

                    case JSON_heavy_object:
                    {
                        assert(nArgs>0);
                        assert(pStack.size()>=size_t(nArgs));
                        Value v(JSON::IsObject);
                        {
                            Object &obj = v.as<Object>();
                            for(int i=-nArgs;i<0;++i)
                            {
                                const Pair &p = pStack[i];
                                if( !obj.insert(p) )
                                {
                                    throw exception("%s: multiple pair with name '%s'", name, * p.name );
                                }
                            }
                            pStack.popn(nArgs);
                        }
                        vStack.push(nil);
                        vStack.peek().swap_with(v);
                    } break;
                        
                    case JSON_pair: {
                        assert(nArgs==2);
                        assert(vStack.size()>=2);
                        //std::cerr << "@pair: vstack=" << vStack << std::endl;
                        // fetch value
                        Value v = nil; v.swap_with(vStack.peek()); vStack.pop();
                        // fetch name
                        Value n = nil; n.swap_with(vStack.peek()); vStack.pop();
                        assert(n.isString());
                        Pair p(n.as<String>());
                        p.value.swap_with(v);
                        pStack.push(p);
                    } break;

                    case JSON_empty_array:
                    {
                        assert(0==nArgs);
                        Value v(JSON::IsArray);
                        vStack.push(nil);
                        vStack.peek().swap_with(v);
                    }   break;

                    case JSON_heavy_array:{
                        assert(nArgs>0);
                        Value v(JSON::IsArray);
                        for(int i=-nArgs;i<0;++i)
                        {
                            v.as<Array>().push_back( vStack[i] );
                        }
                        vStack.popn(nArgs);
                        vStack.push(nil);
                        vStack.peek().swap_with(v);
                    }break;

                    case JSON_json:
                    {
                        assert(1==nArgs);
                        assert(1==vStack.size());
                        assert(0==pStack.size());
                        value.swap_with(vStack.peek());
                        vStack.free();
                    } break;

                    default:
                        throw exception("%s: unexpected internal %s", name, *label);
                }

            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(jCompiler);
            friend class singleton<jCompiler>;

            static const char name[];
            static const threading::longevity life_time = -100;
        };

        const char jCompiler::name[] = "JSON.Loader";


        Value & Value:: LoadFrom( Lang::Source &source)
        {
            static jCompiler & jLoader = jCompiler::instance();

            jLoader.ld(source);
            return jLoader.value;
        }
    }
}
