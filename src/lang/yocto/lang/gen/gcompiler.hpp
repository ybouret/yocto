#ifndef YOCTO_LANG_GEN_GCOMPILER_INCLUDED
#define YOCTO_LANG_GEN_GCOMPILER_INCLUDED 1

#include "yocto/lang/gen/gparser.hpp"
#include "yocto/threading/singleton.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {
            class gNode;
            typedef core::list_of_cpp<gNode> gList;

            struct gType
            {
                static const uint32_t TERM = YOCTO_FOURCC('T','E','R','M');
                static const uint32_t RULE = YOCTO_FOURCC('R','U','L','E');
                static const uint32_t ALT  = YOCTO_FOURCC(' ','|','|',' ');
                static const uint32_t OPT  = YOCTO_FOURCC(' ',' ',' ','?');
                static const uint32_t ZOM  = YOCTO_FOURCC(' ',' ',' ','*');
                static const uint32_t OOM  = YOCTO_FOURCC(' ',' ',' ','+');

            };

            class gNode : public object
            {
            public:
                virtual ~gNode() throw();
                explicit gNode(const string   &id,
                               const uint32_t  t);

                gNode         *next;
                gNode         *prev;
                const string   label;
                const uint32_t uuid;


            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(gNode);
            };

            

            class gCompiler : public singleton<gCompiler>
            {
            public:
                gParser getAST;
                Parser *encode(const Node *ast);
                bool    verbose;


            private:
                explicit gCompiler();
                virtual ~gCompiler() throw();
                YOCTO_DISABLE_COPY_AND_ASSIGN(gCompiler);
                static const char                 name[];
                static const threading::longevity life_time = -10;
                friend class singleton<gCompiler>;

                void collectAggregate(const Node *node);
                void prepareTopLevel(const Node *node);
            };

        }
    }

}

#endif
