#ifndef YOCTO_INK_BLOB_INCLUDED
#define YOCTO_INK_BLOB_INCLUDED 1

#include "yocto/ink/pixmap.hpp"
#include "yocto/ink/color/named.hpp"
#include "yocto/ink/color/pixel.hpp"
#include "yocto/ink/parallel.hpp"
#include "yocto/core/node.hpp"
namespace yocto
{
    namespace Ink
    {

        class Blob : public Pixmap<size_t>
        {
        public:
            typedef core::dnode_of<size_t>   BNode;
            typedef core::list_of_cpp<BNode> BList;

            explicit Blob( const Bitmap &bmp );
            virtual ~Blob() throw();

            template <typename T>
            inline size_t build(const Pixmap<T> &src,
                                const bool       connectFull,
                                Engine          &engine)
            {
                //______________________________________________________________
                //
                // main algorithm is sequential
                //______________________________________________________________
                Pixmap<size_t>   &self = *this;
                const size_t     nconn  = connectFull ? 8 : 4;
                Vertex::Provider cache;
                Vertex::List     vList;
                BList            bList;

                YOCTO_INK_AREA_LIMITS(self);
                self.ldz();
                for(unit_t j=ymax;j>=ymin;--j)
                {
                    Row                           &B_j = self[j];
                    const typename Pixmap<T>::Row &S_j = src[j];
                    for(unit_t i=xmax;i>=xmin;--i)
                    {
                        //______________________________________________________
                        //
                        //! skip zero pixel
                        //______________________________________________________
                        if( Pixel<T>::IsZero(S_j[i]) )
                        {
                            assert(0==B_j[i]);
                            continue;
                        }

                        //______________________________________________________
                        //
                        //! not a zero pixel
                        //______________________________________________________
                        if(B_j[i]>0)
                        {
                            // already in a blob!
                            continue;
                        }

                        //______________________________________________________
                        //
                        //! start a new blob!
                        //______________________________________________________
                        assert(0==vList.size);
                        BNode *node = new BNode(0);
                        bList.push_back(node);
                        const size_t iBlob = bList.size;
                        vList.push_back( cache.create(i,j) );
                        while(vList.size>0)
                        {
                            Vertex     *pV = vList.pop_back();
                            const coord p0 = pV->pos; assert(self.has(p0));
                            cache.store(pV);
                            // test boundary
                            if(Pixel<T>::IsZero( src[p0] ))
                            {
                                continue;
                            }
                            // not visited or already in blob
                            assert(0==self[p0]||iBlob==self[p0]);
                            size_t &B = self[p0];
                            if(B<=0)
                            {
                                // add to this
                                self[p0] = iBlob;
                                ++(node->data);
                            }

                            // populate list of not visited neighbours
                            for(size_t k=0;k<nconn;++k)
                            {
                                const coord p=p0+Core::Shift[k];
                                if(self.has(p)&&(0==self[p]))
                                {
                                    vList.push_back( cache.create(p) );
                                }
                            }
                        }
                    }
                }
                std::cerr << "cache.size=" << cache.size << std::endl;
                for(const BNode *node=bList.head;node;node=node->next)
                {
                    std::cerr << "#pixels=" << node->data << std::endl;
                }
                return bList.size;
            }
            

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Blob);


        };

    }
}

#endif

