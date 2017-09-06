#ifndef YOCTO_LANG_GEN_GCOMPILER_INCLUDED
#define YOCTO_LANG_GEN_GCOMPILER_INCLUDED 1

#include "yocto/lang/gen/gparser.hpp"
#include "yocto/hashing/mph.hpp"
#include "yocto/associative/map.hpp"

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


            typedef Aggregate        *gRule;
            typedef map<string,gRule> gRuleDB;

            typedef Terminal         *gTerm;
            typedef map<string,gTerm> gTermDB;

            class gCompiler
            {
            public:
                gParser          getAST;
                auto_ptr<Parser> parser;
                hashing::mperf   rootHash; //!< ID, LXR, SEM
                hashing::mperf   ruleHash; //!< ID, RX, RS, RB, ALT, SUB, OPT, OOM, ZOM
                hashing::mperf   lexrHash; //!< drop, endl, comment
                gRuleDB          ruleDB;
                gTermDB          termDB;
                Rule            &find(const string &id); // in rules/terms

                bool           verbose;

                explicit gCompiler();
                virtual ~gCompiler() throw();
                Parser *encode(const Node *ast);

                void    registerTopLevelRules(const Node *node);
                void    detectPlugin(const Node *lxr);
                void    registerNewRule(const Node *node);
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
