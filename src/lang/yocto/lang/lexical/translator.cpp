#include "yocto/lang/lexical/translator.hpp"
#include "yocto/exception.hpp"

#include <iostream>

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {
            Translator:: ~Translator() throw() {}

#define Y_TRANS_CTOR()        \
name(transID),                 \
__root( new Scanner(scanrID) ), \
units(),                         \
root( *__root ),                  \
current( &root ),                  \
scanners(1,as_capacity),            \
history(),                           \
dict()



            void Translator:: linkTo(Lexical::Scanner &s) throw()
            {
                s.sharedDict = &dict;
                s.translator =  this;
            }

            void Translator:: onInit()
            {
                if( ! scanners.insert( __root ) )
                    throw exception("[%s]: unable to initialize scanner <%s>", name.c_str(), root.name.c_str() );
                linkTo( * __root );
            }

            Translator:: Translator(const string &transID,
                                    const string &scanrID) :
            Y_TRANS_CTOR()
            {
                onInit();
            }

            Translator:: Translator(const char *transID,
                                    const char *scanrID) :
            Y_TRANS_CTOR()
            {
                onInit();
            }

            Scanner & Translator:: declare(const string &scanrID)
            {
                Scanner::Handle s( new Scanner(scanrID) );
                if(!scanners.insert(s))
                {
                    throw exception("[%s]: multiple scanner <%s>", name.c_str(),scanrID.c_str());
                }
                linkTo(*s);
                return *s;
            }

            Scanner & Translator:: declare(const char *scanrID)
            {
                const string sid(scanrID);
                return declare(sid);
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

            Translator:: sNode:: sNode(Scanner *s) throw() :
            next(0),
            addr(s)
            {
                assert(addr!=NULL);
            }

            Translator:: sNode:: ~sNode() throw()
            {
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

            namespace
            {

                typedef void (Translator::*Cntrl)(const string &);

                class Exec
                {
                public:
                    inline Exec(Translator     &usrTrans,
                                const string   &usrScanr,
                                const Callback &usrOnExe,
                                const Cntrl     usrCntrl
                                ) :
                    trans(usrTrans),
                    scanr(usrScanr),
                    onExe(usrOnExe),
                    cntrl(usrCntrl)
                    {
                    }

                    inline Exec(const Exec &other) :
                    trans(other.trans),
                    scanr(other.scanr),
                    onExe(other.onExe),
                    cntrl(other.cntrl)
                    {
                    }


                    inline ~Exec() throw()
                    {
                    }

                    Translator   &trans;
                    const string  scanr;
                    Callback      onExe;
                    Cntrl         cntrl;

                    inline bool operator()( const Token &tokn )
                    {
                        (trans.*cntrl)(scanr);
                        onExe(tokn);
                        return false; //! a control token
                    }

                private:
                    YOCTO_DISABLE_ASSIGN(Exec);
                };
            }



            void Scanner:: jump(const string   &scanr,
                                Pattern        *motif,
                                const Callback &onjmp)
            {
                assert(motif!=NULL);
                const Pattern::Handle handle(motif);

                if(!translator)
                {
                    throw exception("<%s>.jump: not linked to a Translator", name.c_str());
                }

                const Exec   jmp(*translator,scanr,onjmp, &Translator::jump);
                const Action action(jmp);
                const string motifID = motif->toString();
                const string label   = "jump@" + scanr + '@' + motifID;
                checkRuleName(label);
                rules.push_back( new Rule(label,handle,action) );
            }

            


            void Translator:: jump(const string &id)
            {
                assert(current);
                Scanner::Handle *ppTarget = scanners.search(id);
                if(!ppTarget) throw exception("[%s]: not <%s> to jump to from <%s>", name.c_str(), id.c_str(), current->name.c_str());
                current = & (**ppTarget);
            }

        }

    }

}

