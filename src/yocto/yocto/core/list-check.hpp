#ifndef YOCTO_CORE_LIST_CHECK_INCLUDED
#define YOCTO_CORE_LIST_CHECK_INCLUDED 1

#include "yocto/core/list.hpp"
#include "yocto/error.hpp"
#include <cstdio>
#include <cstring>

namespace yocto
{
    namespace core
    {
        template <typename LIST>
        inline bool check_list( const LIST &the ) throw()
        {
            char buffer[1024];
            memset(buffer,0,sizeof(buffer));
#define YOCTO_CORE_LIST_CHECK(CONDITION) do { \
if(!(CONDITION)) {\
snprintf(buffer,sizeof(buffer)-1,"list.size=%d: invalid '" #CONDITION "'", int(the.size));\
fatal_error(buffer);\
}\
} while(false)
            switch(the.size)
            {
                case 0:
                    YOCTO_CORE_LIST_CHECK(the.head==NULL);
                    YOCTO_CORE_LIST_CHECK(the.tail==NULL);
                    break;

                case 1:
                    YOCTO_CORE_LIST_CHECK(the.head!=NULL);
                    YOCTO_CORE_LIST_CHECK(the.tail!=NULL);
                    YOCTO_CORE_LIST_CHECK(the.head->prev==NULL);
                    YOCTO_CORE_LIST_CHECK(the.tail->next==NULL);
                    YOCTO_CORE_LIST_CHECK(the.head==the.tail);
                    break;

                default:
                    YOCTO_CORE_LIST_CHECK(the.head!=NULL);
                    YOCTO_CORE_LIST_CHECK(the.tail!=NULL);
                    YOCTO_CORE_LIST_CHECK(the.head!=the.tail);
                    YOCTO_CORE_LIST_CHECK(the.head->prev==NULL);
                    YOCTO_CORE_LIST_CHECK(the.tail->next==NULL);
                {
                    size_t forward_count = 1;
                    for(const typename LIST::node_type *node=the.head->next;node!=NULL;node=node->next)
                    {
                        YOCTO_CORE_LIST_CHECK(node->prev!=NULL);
                        if(node!=the.tail) { YOCTO_CORE_LIST_CHECK(node->next!=NULL); }
                        ++forward_count;
                    }
                    YOCTO_CORE_LIST_CHECK(forward_count==the.size);
                }

                {
                    size_t reverse_count = 1;
                    for(const typename LIST::node_type *node=the.tail->prev;node!=NULL;node=node->prev)
                    {
                        YOCTO_CORE_LIST_CHECK(node->next!=NULL);
                        if(node!=the.head) { YOCTO_CORE_LIST_CHECK(node->prev!=NULL); }
                        ++reverse_count;
                    }
                    YOCTO_CORE_LIST_CHECK(reverse_count==the.size);
                }

            }
            return true;
        }
    }
}

#endif

