#ifndef YOCTO_LANG_T_SOURCE_INCLUDED
#define YOCTO_LANG_T_SOURCE_INCLUDED 1

#include "yocto/lang/t-char.hpp"
#include "yocto/sequence/list.hpp"

namespace yocto
{
    namespace lang
    {
        class t_source : public object
        {
        public:
            virtual ~t_source() throw();
            explicit t_source( const input::pointer &in );


        private:
            class iNode : public object
            {
            public:
                iNode(const input::pointer &in);
                virtual ~iNode() throw();
                iNode         *next;
                iNode         *prev;
                input::pointer from;
                
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(iNode);
            };

            YOCTO_DISABLE_COPY_AND_ASSIGN(t_source);
        };
    }
}

#endif
