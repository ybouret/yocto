#ifndef YOCTO_IPSO_XBUFFER_INCLUDED
#define YOCTO_IPSO_XBUFFER_INCLUDED 1

#include "yocto/ipso/ghosts.hpp"
#include "yocto/ipso/field.hpp"
#include "yocto/counted-object.hpp"

namespace yocto
{
    namespace ipso
    {
        class xbuffer : public object
        {
        public:
            typedef core::list_of_cpp<xbuffer> list;
            
            explicit xbuffer(const size_t num_bytes);
            virtual ~xbuffer() throw();
            
            void   reset() throw();
            size_t load() const throw(); //!< wptr-rptr, #active bytes
            size_t left() const throw(); //!< last-wptr, #available bytes
            

            template <typename T>
            inline void store( const T &data ) throw()
            {
                assert(wptr<=last-sizeof(T));
                memcpy(wptr,&data,sizeof(T));
                wptr += sizeof(T);
            }
            
            template <typename T>
            inline T query() throw()
            {
                assert(rptr<=wptr-sizeof(T));
                T ans(0);
                memcpy(&ans,rptr,sizeof(T));
                rptr += sizeof(T);
                return ans;
            }
            
            template <typename T>
            inline void query( T &ans ) throw()
            {
                assert(rptr<=wptr-sizeof(T));
                memcpy(&ans,rptr,sizeof(T));
                rptr += sizeof(T);
            }
            
            //! store data from field/ghosts.send
            template <typename T>
            inline void store(const ghosts &G, const field<T> &F)
            {
                const ghost &g = G.send; assert(g.size()==G.count);
                for(size_t i=G.count;i>0;--i)
                {
                    const coord1D j = g[i]; assert(j>=0); assert(j<coord1D(F.count));
                    store(F.entry[j]);
                }
            }
            
            //! query data into field/ghosts.recv
            template <typename T>
            inline void query(const ghosts &G, field<T> &F)
            {
                const ghost &g = G.recv; assert(g.size()==G.count);
                for(size_t i=G.count;i>0;--i)
                {
                    const coord1D j = g[i]; assert(j>=0); assert(j<coord1D(F.count));
                    query(F.entry[j]);
                }
            }
            
            
            
            
        private:
            size_t         size; //!< total size in bytes
            uint8_t       *base; //!< memory base
            uint8_t       *rptr; //!< next address to query from
            uint8_t       *wptr; //!< next address to store into
            const uint8_t *last; //!< base+size, first invalid byte
            
            YOCTO_DISABLE_COPY_AND_ASSIGN(xbuffer);

        public:
            xbuffer *next;
            xbuffer *prev;
        };
    }
}

#endif
