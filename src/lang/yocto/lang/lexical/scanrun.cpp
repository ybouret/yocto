#include "yocto/lang/lexical/translator.hpp"
#include "yocto/exception.hpp"

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
                            if(translator)
                            {
                                throw exception("%s.%s: unexpected char '%s'", translator->name.c_str(), label.c_str(), ch->text() );
                            }
                            else
                            {
                                throw exception("%s: unexpected char '%s'", label.c_str(), ch->text() );
                            }
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
