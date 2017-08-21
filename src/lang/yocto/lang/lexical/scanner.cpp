#include "yocto/lang/lexical/scanner.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {
            Scanner:: ~Scanner() throw()
            {
            }

            Scanner:: Scanner(const string &id) :
            name(id),
            rules()
            {
            }

            Scanner:: Scanner(const char *id) :
            name(id),
            rules()
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

            ActionType Scanner:: probe( Source &source, Unit * &lexeme )
            {
                assert(NULL==lexeme);


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
                        return ActionRegular;
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

                lexeme = new Unit(bestRule->stamp);
                lexeme->swap_with(bestTokn);

                return ActionRegular;
            }

        }

    }

}
