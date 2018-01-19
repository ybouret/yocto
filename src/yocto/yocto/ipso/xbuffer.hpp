#ifndef YOCTO_IPSO_XBUFFER_INCLUDED
#define YOCTO_IPSO_XBUFFER_INCLUDED 1

#include "yocto/ipso/ghosts.hpp"
#include "yocto/counted-object.hpp"

namespace yocto
{
    namespace ipso
    {
        class exchange_buffer : public counted_object
        {
        public:
            explicit exchange_buffer(const size_t num_bytes);
            virtual ~exchange_buffer() throw();
            
            void   reset() throw();
            size_t load() const throw(); //!< wptr-rptr, #active bytes
            size_t left() const throw(); //!< last-wptr, #available bytes
            
            template <typename T>
            inline void store( const T &data ) throw()
            {
                assert(wptr<base-sizeof(T));
                memcpy(wptr,&data,sizeof(T));
                wptr += sizeof(T);
            }
            
            template <typename T>
            inline T query() throw()
            {
                T ans(0);
                memcpy(&ans,rptr,sizeof(T));
                rptr += sizeof(T);
                return ans;
            }
            
            
        private:
            size_t         size; //!< total size in bytes
            uint8_t       *base; //!< memory base
            uint8_t       *rptr; //!< next address to query from
            uint8_t       *wptr; //!< next address to store into
            const uint8_t *last; //!< base+size, first invalid byte
            
            YOCTO_DISABLE_COPY_AND_ASSIGN(exchange_buffer);
        };
    }
}

#endif

