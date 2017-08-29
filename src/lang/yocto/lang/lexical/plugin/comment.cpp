#include "yocto/lang/lexical/plugin/comment.hpp"
#include "yocto/lang/lexical/translator.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {

            Comment:: ~Comment() throw() {}

            Comment:: Comment(const char *id,
                              const char *expr,
                              Translator &trans) :
            Plugin(id,trans),
            data(expr)
            {
                back("[:endl:]", this, & Comment::onLeave);
                make("any1",".", translator, & Translator::discard);
            }

            const char * Comment:: trigger() const throw()
            {
                return data.c_str();
            }

            void Comment:: startUp(const Token &)
            {
                // do nothing...
            }

            void Comment:: onLeave(const Token &) throw()
            {
                translator->newLine();
            }

        }

    }

}
