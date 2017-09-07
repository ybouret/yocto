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
                hashing::mperf   lexrHash; //!< drop, endl, comment
                hashing::mperf   termHash; //!< ID, RX, RS
                hashing::mperf   walkHash; //!< ID, RX, RS, ALT, SUB, OPT, OOM, ZOM
                gRuleDB          ruleDB;
                gTermDB          termDB;
                Rule            &find(const string &id); //!< in rules/terms
                bool             has(const string &id) const throw();  //!< in rules/terms

                bool           verbose;

                explicit gCompiler();
                virtual ~gCompiler() throw();
                Parser *encode(const Node *ast);

                // level 1
                void    registerTopLevelRules(const Node *topNode);
                void    detectPlugin(const Node *lxr);
                void    registerNewRule(const Node *node);

                // level 2
                void      registerTermsAndCheckRules(const Node *topNode);
                void      collect(const Node *node);
                Terminal &registerNewTermRX(const string &expr);
                Terminal &registerNewTermRS(const string &expr);

                // level 3
                void link(const Node *topNode);
                void linkRule(const Node *node);
                void walkRule(Aggregate &parent, const Node *node);
                void addTermTo(Aggregate &parent, const string &name);
                Rule &getSubFrom(const string &label, const Node *node);

            private:

                YOCTO_DISABLE_COPY_AND_ASSIGN(gCompiler);
                
                
            };
            
        }
    }
    
}

#endif
