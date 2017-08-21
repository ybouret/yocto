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
                        bestRule = r;
                        bestTokn.swap_with(tokn);
                        break;
                    }
                }

                if(!bestRule)
                {
                    // Ooops, nothing matched
                    const Char *ch = source.peek();
                    if(ch)
                    {
                        throw exception("%s: unexpected char '%s'", name.c_str(), ch->text() );
                    }
                    else
                    {
                        throw exception("%s: unexpected End Of '%s'", name.c_str(), source.moduleID() );
                    }
                }

                //______________________________________________________________
                //
                // scanning other rules for a longer match
                //______________________________________________________________
                for(Rule *r = bestRule->next; r; r=r->next )
                {
                    
                }


                return ActionRegular;
            }

        }

    }

}
