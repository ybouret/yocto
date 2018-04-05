#if 0
#include "yocto/lang/dynamo/compiler.hpp"
#include "yocto/exception.hpp"
#include <cstring>

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            class Emitter
            {
            public:
                static const size_t WRAP = 15;

                ios::ostream &fp;
                size_t        ic;
                size_t        nc;
                char          line[WRAP+1];

                inline Emitter( ios::ostream &user_fp ) :
                fp(user_fp),
                ic(0),
                nc(0),
                line()
                {
                    clear();
                }

                inline ~Emitter() throw() {}

                inline void clear() throw()
                {
                    ic = 0;
                    memset(line,0,sizeof(line));
                }


                static inline bool isSpecial(const int C) throw()
                {
                    switch(C)
                    {
                        case '\\':
                        case '\'':
                        case '\"': return true;
                        default:
                            break;
                    }
                    return false;
                }

                inline void write(uint8_t C, const bool isLast )
                {
                    assert(ic<WRAP);
                    const char c(C);
                    if( (C>=32&&C<127) && !isSpecial(c) )
                    {
                        line[ic++] = C;
                        fp("'%c' ",C); // 4 chars

                    }
                    else
                    {
                        if(isSpecial(c))
                        {
                            line[ic++] = c;
                        }
                        else
                        {
                            line[ic++] = '.';
                        }
                        fp("0x%02x",C); // 4 chars
                    }

                    if(!isLast)
                    {
                        fp(", ");
                    }
                    else
                    {
                        fp("  ");
                    }

                    if(ic>=WRAP) flush();
                }

                inline void close()
                {
                    assert(ic<WRAP);
                    for(size_t i=ic;i<WRAP;++i)
                    {
                        fp("      ");
                    }
                    flush();
                    fp << '\n';
                }


                inline void flush()
                {
                    const size_t n = length_of(line);
                    fp << "/* ";
                    for(size_t i=0;i<n;++i)    fp << line[i];
                    for(size_t i=n;i<WRAP;++i) fp << ' ';
                    fp << "*/\n";
                    clear();
                }


            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Emitter);
            };


#if 0
            static inline
            void WriteString(Emitter             &em,
                             const Lexical::Unit *u,
                             const char           delim,
                             const bool           lastUnit)
            {
                em.write(delim,false);
                for(const Char *ch = u->head;ch;ch=ch->next)
                {
                    em.write(ch->code,false);
                }
                em.write(delim,lastUnit);
            }

            void Parser:: Encode(const string &filename,
                                 ios::ostream &fp)
            {
                //______________________________________________________________
                //
                // prefetching lexemes
                //______________________________________________________________
                Lexical::Units           lexemes;
                auto_ptr<DynamoCompiler> compiler( new DynamoCompiler() );
                Hasher                  &H = compiler->strHash;
                {
                    Source source( Module::OpenFile(filename) );

                    compiler->getAll(lexemes,source);
                    compiler->ungetCopyOf(lexemes);
                    auto_ptr<Parser> parser( compiler->encode(compiler->parse(source)));
                }

                //______________________________________________________________
                //
                // OK, the grammar seems valid, now write a compact version
                //______________________________________________________________
                Emitter em(fp);
                for(const Lexical::Unit *u=lexemes.head;u;u=u->next)
                {
                    const string &label    = u->label;
                    const int     h        = H(label);
                    const bool    lastUnit = (NULL==u->next);

                    switch(h)
                    {
                        case 0: assert("RX"==label); WriteString(em,u,'\"',lastUnit); break;
                        case 1: assert("RS"==label); WriteString(em,u,'\'',lastUnit); break;
                        default:
                            for(const Char *ch = u->head;ch;ch=ch->next)
                            {
                                const bool lastChar = (NULL==ch->next);
                                em.write(ch->code,lastUnit&&lastChar);
                            }
                    }
                    if(!lastUnit)
                    {
                        if(h<0)
                        {
                            if(label!=":")
                            {
                                if(label==";")
                                {
                                    em.write('\n',false);
                                }
                                else
                                {
                                    em.write(' ',false);
                                }
                            }
                        }
                    }
                }
                em.close();
            }
#endif

        }
    }
}
#endif

