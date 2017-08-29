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

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {

            InlineComment:: ~InlineComment() throw() {}

            InlineComment:: InlineComment(const char *id,
                                          const char *exprIni,
                                          const char *exprEnd,
                                          Translator &trans):
            Plugin(id,trans),
            __ini(exprIni)
            {
                back(exprEnd, this, & InlineComment::onLeave);
                
                make("endl","[:endl:]", translator, &Translator::newline);
                make("any1",".",        translator, &Translator::discard);
            }

            const char * InlineComment::trigger() const throw()
            {
                return __ini.c_str();
            }

            void InlineComment:: startUp(const Token &)
            {
                // do nothing...
            }
            
            void InlineComment:: onLeave(const Token &) throw()
            {
                // do nothing
            }

        }

    }
}
