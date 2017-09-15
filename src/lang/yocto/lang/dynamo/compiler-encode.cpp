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

                
                inline void write(uint8_t C)
                {
                    assert(ic<WRAP);
                    const char c(C);
                    if( (C>=32&&C<127) && (c!='\\') && (c!='\"') && (c!='\'') )
                    {
                        line[ic++] = C;
                        fp("'%c' , ",C);
                    }
                    else
                    {
                        line[ic++] = '.';
                        fp("0x%02x, ",C);
                    }
                    if(ic>=WRAP) flush();
                }

                inline void close()
                {
                    assert(ic<WRAP);
                    line[ic++] = 0;
                    fp("0x00");
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

            void Parser:: Encode(const string &filename,
                                 ios::ostream &fp)
            {
                // prefetching lexemes
                Lexical::Units           lexemes;
                auto_ptr<DynamoCompiler> compiler( new DynamoCompiler() );
                Hasher                  &H = compiler->strHash;
                {
                    const Module::Handle     hModule( new Module(filename) );
                    Source                   source(hModule);

                    compiler->getAll(lexemes,source);
                    compiler->ungetCopyOf(lexemes);
                    auto_ptr<Parser> parser( compiler->encode(compiler->parse(source)));
                }

                //
                std::cerr << "#lexemes=" << lexemes.size << std::endl;
                Emitter em(fp);
                for(const Lexical::Unit *u=lexemes.head;u;u=u->next)
                {
                    const string &label = u->label;
                    const int     h     = H(label);
                    switch(h)
                    {
                        case 0: assert("RX"==label); em.write('\"'); break;
                        case 1: assert("RS"==label); em.write('\''); break;
                        default:
                            break;
                    }
                    for(const Char *ch = u->head;ch;ch=ch->next)
                    {
                        em.write(ch->code);
                    }
                    switch(h)
                    {
                        case 0: assert("RX"==label); em.write('\"'); break;
                        case 1: assert("RS"==label); em.write('\''); break;
                        default:
                            break;
                    }


                }
                em.close();

            }

        }
    }
}
