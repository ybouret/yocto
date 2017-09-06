#ifndef YOCTO_LANG_GEN_GCOMPILER_INCLUDED
#define YOCTO_LANG_GEN_GCOMPILER_INCLUDED 1

#include "yocto/lang/gen/gparser.hpp"
#include "yocto/hashing/mph.hpp"

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

            

            class gCompiler
            {
            public:
                gParser        getAST;
                hashing::mperf RootHash;
                hashing::mperf RuleHash;
                bool           verbose;

                explicit gCompiler();
                virtual ~gCompiler() throw();
                Parser *encode(const Node *ast);

            private:

                YOCTO_DISABLE_COPY_AND_ASSIGN(gCompiler);
                
#if 0
                void collect(const Node *node);
                void onRULE(const Node *node);
                void onLXR(const Node *node);
#endif
            };

        }
    }

}

#endif
