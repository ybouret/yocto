#ifndef YOCTO_GEMS_ATOMS_INCLUDED
#define YOCTO_GEMS_ATOMS_INCLUDED 1

#include "yocto/gems/library.hpp"
#include "yocto/gems/binary.hpp"
#include "yocto/sequence/addr-list.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/exception.hpp"
#include "yocto/ptr/auto.hpp"
#include "yocto/ios/ostream.hpp"

namespace yocto
{
    namespace gems
    {

        //______________________________________________________________________
        //
        //! a node for one atom
        //______________________________________________________________________
        template <typename T>
        class atom_node
        {
        public:
            typedef typename atom<T>::pointer atom_ptr;

            inline atom_node( const atom_ptr &p) throw() :
            pAtom(p),
            next(0),
            prev(0)
            {
            }

            inline atom_node( const atom<T> *p ) throw() :
            pAtom(p),
            next(0),
            prev(0)
            {
            }

            inline ~atom_node() throw() {}

            atom_ptr   pAtom;
            atom_node *next;
            atom_node *prev;

            YOCTO_MAKE_OBJECT;

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(atom_node);
        };

        //______________________________________________________________________
        //
        //! a dynamic set of atoms
        //______________________________________________________________________
        template <typename T>
        class atoms
        {
        public:
            typedef typename atom<T>::pointer    atom_ptr;
            typedef atom_node<T>                 node_type;
            typedef core::list_of_cpp<node_type> list_type;
            typedef addr_node<node_type>         hook_type;
            typedef core::list_of_cpp<hook_type> slot_type;

            //! default ctor
            explicit atoms() : alist(), nslot(0), smask(0), slots(0)
            {
            }

            //! default dtor
            virtual ~atoms() throw() { release(); }

            //! free atoms, keep slots and hooks
            inline void free() throw()
            {
                alist.clear();
                for(size_t i=0;i<nslot;++i)
                {
                    slots[i].clear();
                }
            }

            //! release all resources
            inline void release() throw()
            {
                free();
                memory::kind<memory::global>::release_as(slots,nslot);
                smask = 0;
            }

            //! check memory for load factor
            inline void expand_slots()
            {

                const size_t nmin = max_of<size_t>(1,(alist.size+1)/4);

                //______________________________________________________________
                //
                // allocate new global memory
                //______________________________________________________________
                size_t new_nslot = max_of<size_t>(8,next_power_of_two(nmin));
                if(new_nslot<=nslot)
                {
                    return;
                }

                assert(is_a_power_of_two(new_nslot));
                slot_type   *new_slots = memory::kind<memory::global>::acquire_as<slot_type>(new_nslot);
                const size_t new_smask = new_nslot - 1;

                //______________________________________________________________
                //
                //transfert hooks
                //______________________________________________________________
                for(size_t i=0;i<nslot;++i)
                {
                    slot_type &slot = slots[i];
                    while(slot.size)
                    {
                        hook_type *hook = slot.pop_back();
                        new_slots[ ihash32(hook->addr->pAtom->uuid) & new_smask ].push_back(hook);
                    }
                }

                //______________________________________________________________
                //
                //update memory
                //______________________________________________________________
                memory::kind<memory::global>::release_as(slots,nslot);
                slots = new_slots;
                nslot = new_nslot;
                smask = new_smask;
            }

            //! insert an atom ptr
            inline void insert( const atom_ptr &ptr )
            {
                expand_slots();
                slot_type *slot = 0;
                node_type *node = lookup_node(ptr->uuid, &slot);
                if(node)
                {
                    throw exception("multiple atom %u",unsigned(ptr->uuid));
                }

                //______________________________________________________________
                //
                // create node for list
                //______________________________________________________________
                node = new node_type(ptr);

                //______________________________________________________________
                //
                // create hook : node is taken care of in case of failure
                //______________________________________________________________
                hook_type *hook = create_hook(node);

                //______________________________________________________________
                //
                // dual insertion
                //______________________________________________________________
                slot->push_front(hook);
                alist.push_back(node);

            }

            //! helper
            inline void insert( atom<T> *a )
            {
                const atom_ptr ptr(a);
                insert(ptr);
            }

            //! 'slow' transfer
            inline void transfer_to( atoms &other, const word_t uuid )
            {
                if(nslot>0)
                {
                    slot_type *slot = slot_for(uuid);
                    for(hook_type *hook = slot->head;hook;hook=hook->next)
                    {
                        node_type *node = hook->addr;
                        if( uuid == node->pAtom->uuid)
                        {
                            other.expand_slots();
                            other.slot_for(uuid)->push_front( slot->unlink(hook) );
                            other.alist.push_back( alist.unlink(node) );
                            return;
                        }
                    }
                }
                throw exception("atoms.tranfer: no uuid %u", unsigned(uuid) );
            }

            //! fast transfer, action on atoms
            template <typename FUNC>
            inline void transfer_if( FUNC &is_matching, atoms &destination )
            {
                if(nslot>0)
                {
                    //__________________________________________________________
                    //
                    // first pass: extracting matching, no throw
                    //__________________________________________________________
                    slot_type matching;
                    for(size_t i=0;i<nslot;++i)
                    {
                        slot_type  stk;
                        slot_type *slot = &slots[i];
                        while(slot->size>0)
                        {
                            hook_type     *hook = slot->pop_back();
                            node_type     *node = hook->addr;
                            const atom<T> &a    = *(node->pAtom);
                            if( is_matching(a) )
                            {
                                matching.push_back(hook);
                                (void) alist.unlink(node);
                            }
                            else
                            {
                                stk.push_front( hook );
                            }
                        }
                        slot->swap_with(stk);
                    }

                    //__________________________________________________________
                    //
                    // second pass: effective transfer
                    //__________________________________________________________
                    try
                    {
                        while(matching.size>0)
                        {
                            destination.expand_slots();
                            hook_type   *hook = matching.pop_back();
                            node_type   *node = hook->addr;
                            const word_t uuid = node->pAtom->uuid;
                            destination.slot_for(uuid)->push_back(hook);
                            destination.alist.push_back(node);
                        }
                    }
                    catch(...)
                    {
                        while(matching.size)
                        {
                            hook_type *hook = matching.pop_back();
                            delete hook->addr;
                            delete hook;
                        }
                        throw;
                    }

                }
            }


            inline
            size_t size() const throw() { return alist.size; }

            //! get atom in 0..size-1
            inline atom<T> & operator[](size_t indx) throw()
            {
                assert(indx<size());
                return *(alist.fetch(indx)->pAtom);
            }

            inline
            void encode( binary_atoms<T> &data ) const
            {
                data.build(alist.size);
                for(const node_type *node=alist.head;node;node=node->next)
                {
                    data.store( *(node->pAtom) );
                }
            }

            inline void decode(const binary_atoms<T> &data, library &lib)
            {
                for(size_t i=0;i<data.size;++i)
                {
                    const uint8_t    *p    = data.query(i);

                    // create an atom
                    const word_t      uuid = query_be<word_t>(p); p+=sizeof(uuid);
                    const word_t      puid = query_be<word_t>(p); p+=sizeof(puid);
                    const properties &ppty = lib(puid);
                    atom<T>          *Atom = lib.create<T>(uuid,ppty);
                    insert(Atom);

                    // fill data
                    (T&)(Atom->mass) = query_be<T>(p); p+=sizeof(T);
                    (T&)(Atom->invm) = query_be<T>(p); p+=sizeof(T);
                    fill_v3d(Atom->r,p);
                    fill_v3d(Atom->v,p);
                    fill_v3d(Atom->a,p);
                }
            }

            node_type *head() throw() { return alist.head; }
            node_type *tail() throw() { return alist.tail; }
            const node_type *head() const throw() { return alist.head; }
            const node_type *tail() const throw() { return alist.tail; }

            void save_xyz( ios::ostream &fp, const string &title ) const
            {
                fp("%lu\n", (unsigned long)alist.size);
                fp << title << "\n";
                for(const node_type *node=alist.head;node;node=node->next)
                {
                    const atom<T>  &a = *(node->pAtom);
                    const v3d<T>   &r = a.r;
                    fp("%s %g %g %g\n", a.ppty.name.c_str(),r.x,r.y,r.z);
                }
            }

        private:
            list_type          alist;
            size_t             nslot; //!< a power of two
            size_t             smask; //!< nslot-1
            mutable slot_type *slots;

            inline slot_type *slot_for( const word_t uuid ) const throw()
            {
                assert(nslot>0);
                assert(slots!=0);
                assert(nslot-1==smask);
                return (slot_type *)&slots[ ihash32(uuid) & smask ];
            }

            //! safely create a hook node
            inline
            hook_type *create_hook(node_type *node)
            {
                assert(node);

                auto_ptr<node_type> guard(node);
                hook_type *hook = new hook_type(node);
                guard.forget();
                return hook;

            }


            inline
            node_type *lookup_node(const word_t uuid,
                                   slot_type  **ppSlot) const throw()
            {
                assert(ppSlot);
                assert(0== *ppSlot);

                if(nslot>0)
                {
                    assert(nslot>0);
                    assert(nslot-1==smask);
                    assert(slots!=0);

                    slot_type *slot = slot_for(uuid);
                    *ppSlot =  slot;
                    for(hook_type *hook = slot->head;hook;hook=hook->next)
                    {
                        node_type *node = hook->addr;
                        if(uuid == node->pAtom->uuid)
                        {
                            slot->move_to_front(hook);
                            return node;
                        }
                    }
                }
                return 0;
            }

            static inline void fill_v3d( v3d<T> &v, const uint8_t * &p ) throw()
            {
                v.x = query_be<T>(p); p+=sizeof(T);
                v.y = query_be<T>(p); p+=sizeof(T);
                v.z = query_be<T>(p); p+=sizeof(T);
            }
            
            YOCTO_DISABLE_COPY_AND_ASSIGN(atoms);
        };
        
        
    }
}

#endif
