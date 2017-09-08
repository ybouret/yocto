#ifndef YOCTO_LANG_GEN_GCOMPILER_INCLUDED
#define YOCTO_LANG_GEN_GCOMPILER_INCLUDED 1

#include "yocto/lang/gen/gparser.hpp"
#include "yocto/associative/map.hpp"
#include "yocto/hashing/mph.hpp"

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
                hashing::mperf   kindHash; //!< RX,RS
                
                gRuleDB          ruleDB;
                gTermDB          termDB;
                Rule            &find(const string &id); //!< in rules/terms
                bool             has(const string &id) const throw();  //!< in rules/terms

                bool           verbose;

                explicit gCompiler();
                virtual ~gCompiler() throw();
                Parser  *encode(const Node *ast);
                Parser  *createFrom(Source &source);



                //______________________________________________________________
                //
                // level 1
                //______________________________________________________________

                void    registerTopLevelRules(const Node *topNode);
                void    detectPlugin(const Node *lxr);
                void    registerNewRule(const Node *node);
                //______________________________________________________________
                //
                // level 2
                //______________________________________________________________
                void      registerTermsAndCheckRules(const Node *topNode);
                void      collect(const Node *node);
                Terminal &registerNewTermRX(const string &expr);
                Terminal &registerNewTermRS(const string &expr);

                //______________________________________________________________
                //
                // level 3
                //______________________________________________________________
                void  link(const Node *topNode);
                void  linkRule(const Node *node);
                Rule &walkRule(const Node *node);
                Rule &getTerm(const string &name);
                Rule &getRule(const string &name);
                void  fillCompound(Compound &cmp, const Node *from);

                void linkLxr(const Node *node);
                void linkSmr(const Node *node);

                //! convert the content RX/RS into regexp
                static string StringToExpr(const Node *node);
                int    StringKind(const Node *node) const throw();

                //______________________________________________________________
                //
                // level 4
                //______________________________________________________________
                void finalize();

            private:

                YOCTO_DISABLE_COPY_AND_ASSIGN(gCompiler);
                
                
            };
            
        }
    }
    
}

#endif
