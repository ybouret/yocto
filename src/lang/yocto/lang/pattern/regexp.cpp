#include "yocto/lang/pattern/regexp.hpp"
#include "yocto/lang/pattern/basic.hpp"
#include "yocto/lang/pattern/joker.hpp"
#include "yocto/lang/pattern/logic.hpp"
#include "yocto/lang/pattern/posix.hpp"
#include "yocto/ptr/auto.hpp"
#include "yocto/exception.hpp"
#include "yocto/code/utils.hpp"

#include <iostream>

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

            inline Pattern *subExpr()
            {
                auto_ptr<AND> sub( new AND() );
                Logical      &sxp = *sub;

                while(curr<last)
                {
                    const char C = curr[0];
                    switch(C)
                    {

                            //__________________________________________________
                            //
                            // grouping
                            //__________________________________________________
                        case Y_LPAREN:
                            ++curr; // skip left parenthesis
                            ++depth;
                            sxp << subExpr();
                            break;

                        case Y_RPAREN:
                            if(--depth<0) throw exception("%sunexpected '%c'",fn,Y_RPAREN);
                            ++curr; // skip right parenthesis
                            goto END_SXP;

                            //__________________________________________________
                            //
                            // alternating
                            //__________________________________________________
                        case Y_ALTERN: {
                            ++curr; // skip alternation sign
                            auto_ptr<Logical> alt( new OR() );
                            *alt << Pattern::Optimize(sub.yield()); //!< LHS
                            *alt << subExpr();   //!< RHS
                            return Pattern::Optimize(alt.yield()); }

                            //__________________________________________________
                            //
                            // simple jokers
                            //__________________________________________________
                        case '?':
                        case '*':
                        case '+':
                            if(sxp.operands.size<=0) throw exception("%sunexpected joker '%c'",fn,C);
                            createSimpleJoker(sxp.operands,C);
                            ++curr; // skip joker sign
                            break;
                            
                            //__________________________________________________
                            //
                            // dot
                            //__________________________________________________
                            case '.':
                            sxp << posix::dot();
                            ++curr;
                            break;
                            
                            //__________________________________________________
                            //
                            // escape sequence
                            //__________________________________________________
                            case '\\':
                            mainEscapeSequence(sxp.operands);
                            break;

                            //__________________________________________________
                            //
                            // curly braced jokers
                            //__________________________________________________
                        case Y_LBRACE:
                            createBracedJoker(sxp.operands);
                            break;

                            //__________________________________________________
                            //
                            // start a new group
                            //__________________________________________________
                        case Y_LBRACK:
                            createGroup(sxp.operands);
                            break;

                            //__________________________________________________
                            //
                            // default
                            //__________________________________________________
                        default:
                            sxp << new Single(C);
                            ++curr;
                    }
                }

            END_SXP:
                //______________________________________________________________
                //
                // sanity checks
                //______________________________________________________________
                if( sxp.operands.size <= 0 )
                {
                    throw exception("%sempty sub expression",fn);
                }

                return Pattern::Optimize(sub.yield());
            }



            //==================================================================
            //
            //
            // create a simple Joker
            //
            //
            //==================================================================
            inline void createSimpleJoker(Patterns &ops, const char C)
            {
                assert(ops.size>0);
                switch(C)
                {
                    case '+': ops.push_back( OneOrMore(ops.pop_back()));        return;
                    case '?': ops.push_back( Optional::Create(ops.pop_back())); return;
                    case '*': ops.push_back( ZeroOrMore(ops.pop_back()));       return;
                    default : break;
                }
                throw exception("%scorrupted code in createSimpleJoker!",fn);
            }


            //==================================================================
            //
            //
            // parse an escape sequence from sub-expression
            //
            //
            //==================================================================
            inline void mainEscapeSequence(Patterns &ops)
            {
                assert('\\'==*curr);
                if(++curr>=last) throw exception("%sunfinished escape sequence",fn);
                const char E = curr[0];
                ++curr; // skip escape char
                
                switch(E)
                {
                    case '.':
                    case '\\':
                    case '/':
                    case '{':
                    case '}':
                    case '(':
                    case ')':
                    case '[':
                    case ']':
                    case '"':
                    case '\'':
                    case '+':
                    case '*':
                    case '?':
                        ops.push_back( new Single(E) );
                        return;
                        
                    case 'n': ops.push_back( new Single('\n') ); return;
                    case 'r': ops.push_back( new Single('\r') ); return;
                    case 't': ops.push_back( new Single('\t') ); return;
                    case 'f': ops.push_back( new Single('\f') ); return;
                    case 'v': ops.push_back( new Single('\v') ); return;
                        
                    case 'x':
                        ops.push_back( hexEscapeSequence() );
                        return;
                        
                    default:
                        break;
                }
                
                throw exception("%sunexpected escape char '%c'",fn,E);
                
            }

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

            //==================================================================
            //
            //
            // extract joker from braced expression: reference or counting
            //
            //
            //==================================================================
            inline void createBracedJoker( Patterns &ops )
            {
                assert(Y_LBRACE== *curr);
                const char *ini = ++curr;
                while(*(curr++)!=Y_RBRACE)
                {
                    if(curr>=last) throw exception("%smissing '%c'",fn,Y_RBRACE);
                }
                const char *end = curr;
                assert(end>ini);
                const size_t n = static_cast<size_t>(end-ini)-1;
                if(n<=0) throw exception("%sempty braces",fn);
                string info(ini,n);
                std::cerr << "using '" << info << "'" << std::endl;

                const char C = info[0];
                if( PatternDict::IsValidFirstChar(C) )
                {
                    // assuming reference
                    if(!dict) throw exception("%smissing dictionary for '%s'", fn, info.c_str() );
                    ops.push_back( (*dict)[info] );
                }
                else
                {
                    // assuming counting joker
                }

            }


            //==================================================================
            //
            //
            // create a group and add it to operands
            //
            //
            //==================================================================
            void createGroup( Patterns &ops )
            {
                assert(Y_LBRACK==*curr);
                if(++curr>last) throw exception("%sunfinished group",fn);

                
                // here is the first char of the group
                char C = *curr;

                // detect a posix reference
                if(':'==C)
                {
                    std::cerr << "POSIX" << std::endl;
                    ops.push_back(findPosixExpression());
                    return;
                }

                auto_ptr<Logical> grp;
                switch(C)
                {
                    default:
                        grp.reset( new OR() );
                }

                while(true)
                {
                    if(curr>=last)
                    {
                        throw exception("%sunfinished group",fn);
                    }

                    C = *curr;
                    switch(C)
                    {

                        case Y_LBRACK:
                            createGroup(grp->operands);
                            break;

                        case Y_RBRACK:
                            ++curr;
                            goto END_GRP;

                        default:
                            grp->operands.push_back( new Single(C) );
                            ++curr;
                    }


                }

            END_GRP:
                ops.push_back( grp.yield() );

            }

            //==================================================================
            //
            //
            // extract a posix sub-expression
            //
            //
            //==================================================================
            Pattern * findPosixExpression()
            {
                assert(':'==*curr);
                const char *ini = ++curr; // skip ':'
                while(true)
                {
                    if(curr>=last) throw exception("%sunfinished posix expression",fn);
                    if(curr[0]==':')
                    {
                        if(curr[1]!=Y_RBRACK) throw exception("%sexpecting ':%c'",fn,Y_RBRACK);
                        break;
                    }
                    ++curr;
                }
                assert(':'      == curr[0]);
                assert(Y_RBRACK == curr[1]);
                const char *end = curr;
                ++curr;
                ++curr;

                const size_t n = static_cast<size_t>(end-ini);
                if(n<=0) throw exception("%sempty posix expression",fn);

                const string id(ini,n);
#define Y_LANG_POSIX(NAME) if(#NAME==id) return posix::NAME()

                Y_LANG_POSIX(upper);
                Y_LANG_POSIX(lower);
                Y_LANG_POSIX(digit);
                Y_LANG_POSIX(alpha);
                Y_LANG_POSIX(alnum);
                Y_LANG_POSIX(xdigit);
                Y_LANG_POSIX(blank);
                Y_LANG_POSIX(space);
                Y_LANG_POSIX(punct);

                Y_LANG_POSIX(word);
                Y_LANG_POSIX(endl);
                Y_LANG_POSIX(dot);

                throw exception("%sunknown posix :%s:",fn,id.c_str());
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
            if(compiler.depth>0)
            {
                throw exception("%sunifinished sub expression",fn);
            }
            return compiler.subExpr();
        }
        
    }
}
