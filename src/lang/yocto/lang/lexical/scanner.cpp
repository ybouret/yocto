#include "yocto/lang/lexical/scanner.hpp"
#include "yocto/exception.hpp"
#include "yocto/code/utils.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {
            Scanner:: ~Scanner() throw()
            {
            }

#define Y_SCANNER_CTOR() \
name(id),                 \
rules(),                   \
module(0),                  \
max_label_length(0),         \
sharedDict(0),                \
translator(0)

            Scanner:: Scanner(const string &id) :
            Y_SCANNER_CTOR()
            {
            }

            Scanner:: Scanner(const char *id) :
            Y_SCANNER_CTOR()
            {
            }


            const string & Scanner:: key() const throw()
            {
                return name;
            }

            void Scanner:: checkRuleName(const string &label) const
            {
                for(const Rule *r = rules.head;r;r=r->next)
                {
                    if(label==r->label) throw exception("%s: multiple rule '%s'", name.c_str(), label.c_str() );
                }
                (size_t &)max_label_length = max_of(max_label_length,label.length());
            }



        }
    }
}

#include <iostream>

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {

            Unit *  Scanner:: probe( Source &source, bool &isRegular )
            {

                //______________________________________________________________
                //
                // initialize
                //______________________________________________________________
                isRegular = true;
                Rule   *bestRule = NULL;
                Token   bestTokn;

                //______________________________________________________________
                //
                // try to find a first matching rule
                //______________________________________________________________
                for(Rule *r = rules.head; r; r=r->next)
                {
                    Token tokn;
                    if(r->motif->match(source,tokn))
                    {
                        source.store_copy(tokn);     // restore tokn for other rules
                        bestRule = r;                // keep the rule
                        bestTokn.swap_with(tokn);    // and take the token
                        break;
                    }
                }

                if(!bestRule)
                {
                    //__________________________________________________________
                    //
                    // Ooops, nothing matched
                    //__________________________________________________________
                    const Char *ch = source.peek();
                    if(ch)
                    {
                        throw exception("%s: unexpected char '%s'", name.c_str(), ch->text() );
                    }
                    else
                    {
                        return NULL; // EOF
                    }
                }

                //______________________________________________________________
                //
                // scanning other rules for a longer match
                //______________________________________________________________
                for(Rule *r = bestRule->next; r; r=r->next )
                {
                    Token tokn;
                    if(r->motif->match(source,tokn))
                    {
                        //______________________________________________________
                        //
                        // we got another candidate !
                        //______________________________________________________
                        source.store_copy(tokn); // restore tokn for other rules
                        if(tokn.size>bestTokn.size)
                        {
                            // new winner !
                            bestRule = r;
                            bestTokn.swap_with(tokn);
                        }
                    }
                }

                //______________________________________________________________
                //
                // update source
                //______________________________________________________________
                assert( source.in_cache() >= bestTokn.size );
                source.forward(bestTokn.size);

                //______________________________________________________________
                //
                // take action
                //______________________________________________________________


                YOCTO_FAILSAFE(module    = source.module.__get();
                               isRegular = bestRule->action(bestTokn)
                               ,module=0);

                if( isRegular )
                {
                    // a regular unit
                    Unit *u = new Unit(bestRule->stamp);
                    u->swap_with(bestTokn);
                    return u;
                }
                else
                {
                    // unit was used to trigger something...
                    return NULL;
                }
            }
            
        }
        
    }
    
}
