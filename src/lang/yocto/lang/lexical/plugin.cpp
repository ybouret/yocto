#include "yocto/lang/lexical/translator.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {
            Plugin:: ~Plugin() throw()
            {
            }

            Plugin:: Plugin(const string &id, Translator &trans) :
            Scanner(id)
            {
                trans.linkTo(*this);
            }

            Plugin:: Plugin(const char  *id, Translator &trans) :
            Scanner(id)
            {
                trans.linkTo(*this);
            }


            void Plugin:: onEnter(const Token &tkn)
            {
                startUp(tkn);
            }

            void Scanner:: call(const string &pluginID)
            {
                if(!translator)
                {
                    throw exception("<%s>.call(plugin '%s'): not linked!", label.c_str(),pluginID.c_str());
                }

                Plugin::Handle *ppPlugin = translator->plugins.search(pluginID);
                if(!ppPlugin)
                {
                    throw exception("<%s> no plugin '%s' to call!",label.c_str(),pluginID.c_str());
                }
                Plugin &       plug= **ppPlugin;
                const Callback enter(&plug, & Plugin::onEnter);
                Pattern       *motif = RegExp(plug.trigger(),sharedDict);
                call(pluginID,motif,enter);
            }
        }
    }
}
