#include "yocto/lang/pattern/regexp.hpp"
#include "yocto/lang/pattern/basic.hpp"
#include "yocto/lang/pattern/joker.hpp"
#include "yocto/lang/pattern/logic.hpp"
#include "yocto/ptr/auto.hpp"

namespace yocto
{
    namespace Lang
    {
        ////////////////////////////////////////////////////////////////////////
        //
        // internal regular expression compiler
        //
        ////////////////////////////////////////////////////////////////////////

        class RegExpCompiler
        {
        public:
            const char        *curr;
            const char        *last;
            const PatternDict *dict;

            inline RegExpCompiler(const string      &usr_expr,
                                  const PatternDict *usr_dict) :
            curr( usr_expr.c_str()       ),
            last( curr + usr_expr.size() ),
            dict( usr_dict               )
            {
            }

            inline ~RegExpCompiler() throw()
            {
            }


            //==================================================================
            //
            //
            // Compile a sub-expression from curr
            //
            //
            //==================================================================
            inline Pattern *subExpr()
            {
                auto_ptr<AND> sub( new AND() );
                Logical      &sxp = *sub;

                while(curr<last)
                {
                    const char C = curr[0];
                    switch(C)
                    {

                        default:
                            sxp << new Single(C);
                    }
                    ++curr;
                }

                return sub.yield();
            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(RegExpCompiler);
        };
    }
}


namespace yocto
{
    namespace Lang
    {

        Pattern *RegExp( const string &expr, const PatternDict *dict )
        {
            RegExpCompiler compiler(expr,dict);

            return compiler.subExpr();
        }
        
    }
}
