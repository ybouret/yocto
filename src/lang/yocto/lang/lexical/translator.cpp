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
_root( new Scanner(scanrID) ),  \
units(),                         \
root( *_root ),                   \
cache(),                           \
current( &root ),                   \
scanners(1,as_capacity),             \
history(),                            \
dict()



            void Translator:: linkTo(Lexical::Scanner &s) throw()
            {
                s.sharedDict = &dict;
                s.translator =  this;
            }

            void Translator:: onInit()
            {
                if( ! scanners.insert( _root ) )
                    throw exception("[%s]: unable to initialize scanner <%s>", name.c_str(), root.label.c_str() );
                linkTo( * _root );
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


            ////////////////////////////////////////////////////////////////////
            //
            // jump of call wrappers
            //
            ////////////////////////////////////////////////////////////////////
            namespace
            {

                typedef void (Translator::*Cntrl)(const string &);

                class Exec : public object
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


                    inline virtual ~Exec() throw()
                    {
                    }

                    Translator   &trans;
                    const string  scanr;
                    Callback      onExe;
                    Cntrl         cntrl;

                    inline Lexical::Result operator()( const Token &tokn )
                    {
                        (trans.*cntrl)(scanr);
                        onExe(tokn);
                        return Lexical::Control; //! a control token
                    }

                private:
                    YOCTO_DISABLE_ASSIGN(Exec);
                };
            }


            ////////////////////////////////////////////////////////////////////
            //
            // jump
            //
            ////////////////////////////////////////////////////////////////////
            void Scanner:: jump(const string   &scanr,
                                Pattern        *motif,
                                const Callback &onJump)
            {
                assert(motif!=NULL);
                const Pattern::Handle handle(motif);

                if(!translator)
                {
                    throw exception("<%s>.jump: not linked to a Translator", label.c_str());
                }

                const Exec   __jump(*translator,scanr,onJump, &Translator::__jump);
                const Action action(__jump);
                const string motifID = motif->signature();
                const string label   = "jump@" + scanr + '@' + motifID;
                checkRuleName(label);
                rules.push_back( new Rule(label,handle,action) );
            }

            void Translator:: __jump(const string &id)
            {
                assert(current);
                Scanner::Handle *ppTarget = scanners.search(id);
                if(!ppTarget) throw exception("[%s]: no <%s> to jump to from <%s>", name.c_str(), id.c_str(), current->label.c_str());
                current = & (**ppTarget);
            }

            ////////////////////////////////////////////////////////////////////
            //
            // call
            //
            ////////////////////////////////////////////////////////////////////
            void Scanner:: call(const string   &scanr,
                                Pattern        *motif,
                                const Callback &onCall)
            {
                assert(motif!=NULL);
                const Pattern::Handle handle(motif);

                if(!translator)
                {
                    throw exception("<%s>.call: not linked to a Translator", label.c_str());
                }

                const Exec   __call(*translator,scanr,onCall, &Translator::__call);
                const Action action(__call);
                const string motifID = motif->signature();
                const string label   = "call@" + scanr + '@' + motifID;
                checkRuleName(label);
                rules.push_back( new Rule(label,handle,action) );
            }


            void Translator:: __call(const string &id)
            {
                assert(current);
                Scanner::Handle *ppTarget = scanners.search(id);
                if(!ppTarget) throw exception("[%s]: no <%s> to call from <%s>", name.c_str(), id.c_str(), current->label.c_str());
                history.store( new sNode(current) );
                current = & (**ppTarget);
            }


            ////////////////////////////////////////////////////////////////////
            //
            // back
            //
            ////////////////////////////////////////////////////////////////////
            namespace
            {
                class Back : public object
                {
                public:
                    Translator   &trans;
                    Callback      onExe;

                    inline Back(Translator     &usrTrans,
                                const Callback &usrOnExe) :
                    trans(usrTrans),
                    onExe(usrOnExe)
                    {
                    }

                    inline Back(const Back &other) :
                    trans(other.trans),
                    onExe(other.onExe)
                    {
                    }


                    inline virtual ~Back() throw()
                    {

                    }

                    inline Lexical::Result operator()(const Token &tokn)
                    {
                        trans.__back();
                        onExe(tokn);
                        return Lexical::Control; // control
                    }
                    
                private:
                    YOCTO_DISABLE_ASSIGN(Back);
                };
                
            }
            
            void Translator:: __back()
            {
                assert(current);
                if(!history.top) throw exception("[%s]: no possible back from <%s>",name.c_str(),current->label.c_str());
                current = history.top->addr;
                delete history.query();
            }

            void Scanner:: back(Pattern        *motif,
                                const Callback &onBack)
            {
                assert(motif!=NULL);
                const Pattern::Handle handle(motif);

                if(!translator)
                {
                    throw exception("<%s>.back: not linked to a Translator", label.c_str());
                }

                const Back   __back(*translator,onBack);
                const Action action(__back);
                const string motifID = motif->signature();
                const string label   = "back@" + motifID;
                checkRuleName(label);
                rules.push_back( new Rule(label,handle,action) );
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

            Unit * Translator:: run( Source &source )
            {
                if(cache.size)
                {
                    // done...
                    return cache.pop_front();
                }
                else
                {

                    while(true)
                    {
                        Result res = Forward;
                        Unit  *lex = current->probe(source,res);

                        switch(res)
                        {
                            case Forward:
                                if(!lex) throw exception("[%s].<%s>: no unit on Forward result", name.c_str(),current->label.c_str());
                                return lex;

                            case Discard:
                                if(lex) delete lex;
                                throw exception("[%s].<%s>: unexpected Discard result", name.c_str(), current->label.c_str());

                            case Control:
                                if(lex) delete lex;
                                throw exception("[%s].<%s>: Control pattern shouldn't return unit", name.c_str(),current->label.c_str());
                                break; // for new cycle, current may have changed!
                        }

                        return NULL;
                    }
                }
            }

        }

    }

}

