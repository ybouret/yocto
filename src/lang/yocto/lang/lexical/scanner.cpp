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

#define Y_SCANNER_CTOR()  \
stamp( new string(id) ),   \
label( *stamp),             \
rules(),                     \
module(0),                    \
max_label_length(0),           \
sharedDict(0),                  \
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
                return label;
            }

            void Scanner:: checkRuleName(const string &rule_label) const
            {
                for(const Rule *r = rules.head;r;r=r->next)
                {
                    if(rule_label==r->label) throw exception("<%s>: multiple rule '%s'", label.c_str(), rule_label.c_str() );
                }
                (size_t &)max_label_length = max_of(max_label_length,rule_label.length());
            }

            Unit * Scanner:: newUnit() const
            {
                return new Unit(stamp);
            }

            Unit * Scanner:: newUnit( Token &content ) const
            {
                Unit *u = new Unit(stamp);
                u->swap_with(content);
                return u;
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

            Unit *  Scanner:: probe( Source &source, Result &kind )
            {

                while(true)
                {
                    //__________________________________________________________
                    //
                    // initialize
                    //__________________________________________________________
                    kind             = Forward;
                    Rule   *bestRule = NULL;
                    Token   bestTokn;

                    //__________________________________________________________
                    //
                    // try to find a first matching rule
                    //__________________________________________________________
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
                        //______________________________________________________
                        //
                        // Ooops, nothing matched
                        //______________________________________________________
                        const Char *ch = source.peek();
                        if(ch)
                        {
                            throw exception("%s: unexpected char '%s'", label.c_str(), ch->text() );
                        }
                        else
                        {
                            return NULL; // EOF
                        }
                    }

                    //__________________________________________________________
                    //
                    // scanning other rules for a longer match
                    //__________________________________________________________
                    for(Rule *r = bestRule->next; r; r=r->next )
                    {
                        Token tokn;
                        if(r->motif->match(source,tokn))
                        {
                            //__________________________________________________
                            //
                            // we got another candidate !
                            //__________________________________________________
                            source.store_copy(tokn); // restore tokn for other rules
                            if(tokn.size>bestTokn.size)
                            {
                                // new winner !
                                bestRule = r;
                                bestTokn.swap_with(tokn);
                            }
                        }
                    }

                    //__________________________________________________________
                    //
                    // update source
                    //__________________________________________________________
                    assert( source.in_cache() >= bestTokn.size );
                    source.forward(bestTokn.size);

                    //__________________________________________________________
                    //
                    // take action
                    //__________________________________________________________


                    YOCTO_FAILSAFE(module    = source.module.__get();
                                   kind      = bestRule->action(bestTokn)
                                   ,module=0);
                    
                    switch(kind)
                    {
                        case Forward: {
                            Unit *u = new Unit(bestRule->stamp);
                            u->swap_with(bestTokn);
                            return u; }
                            
                        case Discard:
                            // do nothing...but try to get next unit...
                            break;

                        case Control:
                            // would signal to translator...
                            return NULL;
                    }

                    //__________________________________________________________
                    //
                    // goto next unit
                    //__________________________________________________________
                }
            }



        }
        
    }
    
}
