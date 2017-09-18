#include "yocto/json/value.hpp"
#include "yocto/threading/singleton.hpp"
#include "yocto/lang/compiler.hpp"
#include "yocto/exception.hpp"

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
            Compiler( Syntax::Parser::Generate( jCode, sizeof(jCode) ) )
            {
            }

            virtual ~jCompiler() throw()
            {
            }

            Value value;

            virtual void initialize()
            {
                std::cerr << "<JSON>" << std::endl;
                value.clear();
            }

            virtual void onTerminal(const string &label,
                                    const int     hCode,
                                    const string &content)
            {
                __indent() << "push "; __output(label) << " '" << content << "'" << std::endl;

                switch(hCode)
                {
                    case JSON_number: break;
                    case JSON_string: break;
                    case JSON_null:   break;
                    case JSON_true:   break;
                    case JSON_false:  break;
                    default:
                        throw exception("%s: unexpected terminal %s", name, *label);
                }
            }

            virtual void onInternal(const string &label,
                                    const int     hCode,
                                    const size_t  nArgs)
            {
                __indent() << "call " << (label) << "/" << nArgs << std::endl;
                switch(hCode)
                {
                    case JSON_empty_object: break;
                    case JSON_heavy_object: break;
                    case JSON_pair:         break;
                    case JSON_empty_array:  break;
                    case JSON_heavy_array:  break;
                    case JSON_json:         break;
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
            std::cerr << "<JSON/>" << std::endl;

            return jLoader.value;
        }
    }
}
