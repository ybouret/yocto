#include "yocto/json/value.hpp"
#include "yocto/threading/singleton.hpp"
#include "yocto/lang/compiler.hpp"

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
                std::cerr << "JSON: Start Loading..." << std::endl;
                value.clear();
            }
            

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(jCompiler);
            friend class singleton<jCompiler>;

            static const char name[];
            static const threading::longevity life_time = -100;
        };

        const char jCompiler::name[] = "JSON";


        Value & Value:: LoadFrom( Lang::Source &source)
        {
            static jCompiler & jLoader = jCompiler::instance();

            jLoader.ld(source);

            return jLoader.value;
        }
    }
}
