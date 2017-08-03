#include "yocto/lang/pattern/regexp.hpp"
#include "yocto/lang/pattern/basic.hpp"
#include "yocto/lang/pattern/joker.hpp"
#include "yocto/lang/pattern/logic.hpp"
#include "yocto/lang/pattern/posix.hpp"
#include "yocto/ptr/auto.hpp"
#include "yocto/exception.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/core/locate.hpp"
#include "yocto/string/conv.hpp"


namespace yocto
{
    namespace Lang
    {
        ////////////////////////////////////////////////////////////////////////
        //
        // internal regular expression compiler
        //
        ////////////////////////////////////////////////////////////////////////
        static const char fn[] = "RegExp: ";

        class RegExpCompiler
        {
        public:
            const char        *curr;
            const char        *last;
            int                depth;
            const PatternDict *dict;

            inline RegExpCompiler(const string      &usr_expr,
                                  const PatternDict *usr_dict) :
            curr( usr_expr.c_str()       ),
            last( curr + usr_expr.size() ),
            depth(0),
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
#define Y_LPAREN '('
#define Y_RPAREN ')'
#define Y_ALTERN '|'
#define Y_LBRACE '{'
#define Y_RBRACE '}'
#define Y_LBRACK '['
#define Y_RBRACK ']'

#define Y_ESC_CTRL()                                 \
case 'n': ops.push_back( new Single('\n') ); return; \
case 'r': ops.push_back( new Single('\r') ); return; \
case 't': ops.push_back( new Single('\t') ); return; \
case 'f': ops.push_back( new Single('\f') ); return; \
case 'v': ops.push_back( new Single('\v') ); return


#include "rx-sub-expr.hxx"
#include "rx-sub-jk.hxx"
#include "rx-sub-esc.hxx"




            //==================================================================
            //
            // convert a two-chars hexadecimal to a single code
            //
            //==================================================================
            Pattern *hexEscapeSequence()
            {
                if(curr>=last) throw exception("%smissing first hexa code",fn);
                const int hi = hex2dec(*curr);
                if(hi<0) throw exception("%sinvalid first hexa char '%c'",fn,*curr);

                if(++curr>=last) throw exception("%smissing second hexa code",fn);
                const int lo = hex2dec(*curr);
                if(lo<0) throw exception("%sinvalid second hexa char '%c'",fn,*curr);
                ++curr;

                const int ch = hi*16 + lo;
                return new Single(ch);
            }



#include "rx-grp.hxx"
#include "rx-grp-posix.hxx"
#include "rx-grp-esc.hxx"




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
            if(compiler.depth>0)
            {
                throw exception("%sunifinished sub expression",fn);
            }
            return Pattern::Optimize(compiler.subExpr());
        }
        
    }
}
