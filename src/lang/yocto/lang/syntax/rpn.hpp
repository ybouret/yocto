#ifndef YOCTO_LANG_SYNTAX_RPN_INCLUDED
#define YOCTO_LANG_SYNTAX_RPN_INCLUDED 1

#include "yocto/hashing/mph.hpp"
#include "yocto/associative/set.hpp"
#include "yocto/ptr/intr.hpp"

namespace yocto
{
    namespace Lang
    {
        typedef hashing::mperf Hasher;

        namespace Syntax
        {
            class Node; //!< forward declaration
            
            class RPN :
            public counted_object,
            public Hasher
            {
            public:
                typedef intr_ptr<string,RPN> Pointer;
                typedef set<string,Pointer>  Set;

                const string label;     //!< node label to apply

                
                explicit RPN(const string &id);
                explicit RPN(const char   *id);
                virtual ~RPN() throw();

                const string & key() const throw();

                void op(const string &opID);
                void op(const char   *opID);


            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(RPN);
            };

            class RPN_Set : public RPN::Set
            {
            public:
                explicit RPN_Set() throw();
                virtual ~RPN_Set() throw();

                RPN & operator()(const string &id);
                RPN & operator()(const char   *id);

                void operator()( Node *node );

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(RPN_Set);
            };

        }

    }
}

#endif

