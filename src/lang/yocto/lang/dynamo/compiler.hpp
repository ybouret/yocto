#ifndef YOCTO_LANG_DYNAMO_COMPILER_INCLUDED
#define YOCTO_LANG_DYNAMO_COMPILER_INCLUDED 1

#include "yocto/lang/dynamo/parser.hpp"
#include "yocto/associative/map.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {
            typedef Aggregate          *dynRule;
            typedef Terminal           *dynTerm;
            typedef map<string,dynRule> dynRuleDB;
            typedef map<string,dynTerm> dynTermDB;

            //! create a parser
            /**
             - Raw String are Unique or Hollow, automatically detected
             - Top level rules are automatically sets to the right flag
             */
            class DynamoCompiler : public DynamoParser
            {
            public:


                auto_ptr<Parser> parser;  //!< the current built parser
                bool             verbose; //!< to display/save output
                dynRuleDB        ruleDB;  //!< the top level aggregates
                dynTermDB        termDB;  //!< the terminals
                Hasher           topHash; //!< RULE, ALIAS, PLUG, LEXR, SEMR
                Hasher           strHash; //!< RX, RS
                Hasher           lnkHash; //!< ID, RX, RS, SUB, ALT, ZOM, OOM, OPT
                Hasher           lxrHash; //!< drop, endl, comment

                explicit DynamoCompiler();
                virtual ~DynamoCompiler() throw();

                //! create a parser from a master node, with checkIncludes
                /**
                 master node is deleted
                 */
                Parser *encode( Node *master );

                //! save a compiled version of the tree, with checkIncludes
                /**
                 master node is deleted
                 */
                void    serialize( Node *master, ios::ostream &fp );

                //______________________________________________________________
                //
                // check includes in the master node
                //______________________________________________________________
                void checkIncludes( Node *master );
                
                //______________________________________________________________
                //
                // prolog
                //______________________________________________________________
                void createTopLevelFrom( Node *master );
                void  __newRule(const char *fn, dynRule pR, const Tag &t);
                void  __newTerm(const char *fn, dynTerm pT, const Tag &t);

                //______________________________________________________________
                //
                // gather
                //______________________________________________________________
                void       gatherFrom( Node *master );
                Aggregate &findRule(const char *fn, const string &label);
                Terminal  &findTerm(const char *fn, const string &label);
                Rule      &find(const char *fn, const string &label);
                Rule      &walk(const Node *node);
                void       lexr(const Node &node);
                void       semr(const Node &node);
                void       onRPN(const Node *code);
                
                //______________________________________________________________
                //
                // Finalize
                //______________________________________________________________
                void finalize();
                //! detecte what kind of merging is expected
                void __detectPropertiesOf( Aggregate &agg );
                
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(DynamoCompiler);
                static string RS2Expr(const string &RS);
                string String2Expr(const Node *node) const;
                size_t top_max_size;
            };

        }
        
    }
    
}


#endif
