#ifndef YOCTO_SEQUENCE_LIST_INCLUDED
#define YOCTO_SEQUENCE_LIST_INCLUDED 1

#include "yocto/container/sequence.hpp"
#include "yocto/core/list.hpp"
#include "yocto/core/pool.hpp"
#include "yocto/code/bswap.hpp"
#include "yocto/container/iter-linked.hpp"
#include "yocto/sort/merge.hpp"
#include "yocto/bitwise.hpp"
#include "yocto/code/bzset.hpp"
#include <iostream>

namespace yocto
{
	namespace hidden { extern const char list_name[]; }
	
	template <typename T>
	class list : public sequence<T>
	{
	public:
		YOCTO_ARGUMENTS_DECL_T;
		
		class node_type
		{
		public:
			node_type    *next;
			node_type    *prev;
			mutable_type  data;
			static inline node_type *acquire() { return object::acquire1<node_type>(); }
			static inline void       release(node_type *node) throw() { object::release1<node_type>(node);       } 
			static inline void       destroy(node_type *node) throw() { destruct( & node->data ); release(node); }
		private:
			node_type(); ~node_type()throw(); 
			YOCTO_DISABLE_COPY_AND_ASSIGN(node_type);
		};
		
		explicit list() throw() : list_(), pool_() {}
		virtual ~list() throw() { this->kill(); }
		
		virtual const char *name() const throw() { return hidden::list_name; }
		virtual void   free() throw() { while(list_.size>0) keep( list_.pop_back() ); }
		virtual void   release() throw() { kill(); }
		virtual size_t size()     const throw()     { return list_.size; }
		virtual size_t capacity() const throw()     { return pool_.size + list_.size; }		
		virtual void   push_back( param_type obj )  { list_.push_back( make(obj) ); }
		virtual void   pop_back() throw()           { assert(list_.size>0); keep(list_.pop_back()); }
		virtual void   push_front( param_type obj ) { list_.push_front( make(obj) ); }
		virtual void   pop_front() throw()          { assert(list_.size>0); keep(list_.pop_front()); }
		
		virtual void   reserve( size_t n ) 
		{
			size_t m = 0;
			try
            {
				while( n-- > 0 )
				{
					pool_.store( node_type::acquire() );
					++m;
				}
			}
			catch(...)
			{
				while(m-->0) node_type::release( pool_.query() );
				throw;
			}
		}
		
		inline void swap_with( list &other ) throw()
		{
			bswap( list_, other.list_ );
			bswap( pool_, other.pool_ );
		}
		
		inline list( const list &other ) : list_(), pool_()
		{
			try
			{
				for( const node_type *node = other.list_.head; node != NULL; node = node->next )
				{
					push_back( node->data );
				}
			}
			catch(...)
			{
				list_.delete_with( node_type::destroy );
				throw;
			}
		}
		
		inline list( size_t n, const as_capacity_t & ) : list_(), pool_()
		{
			try
            {
				while( pool_.size < n ) pool_.store( node_type::acquire() );
			}
			catch (...) {
				pool_.delete_with( node_type::release );
				throw;
			}
		}
        
        inline list( size_t n, param_type obj) : list_(), pool_()
        {
            try
            {
                for(size_t i=n;i>0;--i)
                    list_.push_back( make(obj) );
            }
            catch(...)
            {
                kill();
                throw;
            }
        }

        //! assuming an empty ctor is available
        inline list( size_t n ) : list_(), pool_()
        {
            try
            {
                for(size_t i=n;i>0;--i)
                    list_.push_back( make( int2type<support_c_style_ops<mutable_type>::value>() ) );
            }
            catch(...)
            {
                kill();
                throw;
            }
        }
		
		inline list & operator=( const list & other )
		{
			if( this != &other )
			{
				list tmp( other );
				swap_with( tmp  );
			}
			return *this;
		}
		
		//======================================================================
		// iterators
		//======================================================================
		typedef iterating::linked<type,node_type,iterating::forward> iterator;
		inline iterator begin() throw() { return iterator( list_.head ); }
		inline iterator end()   throw() { return iterator( NULL );       }
		
		typedef iterating::linked<const_type,const node_type,iterating::forward> const_iterator;
		inline const_iterator begin() const throw() { return const_iterator( list_.head ); }
		inline const_iterator end()   const throw() { return const_iterator( NULL );       }
		
		typedef iterating::linked<type,node_type,iterating::reverse> reverse_iterator;
		inline reverse_iterator rbegin() throw() { return reverse_iterator( list_.tail ); }
		inline reverse_iterator rend()   throw() { return reverse_iterator( NULL );       }
		
		typedef iterating::linked<const_type,const node_type, iterating::reverse> const_reverse_iterator;
		inline const_reverse_iterator rbegin() const throw() { return const_reverse_iterator( list_.tail ); }
		inline const_reverse_iterator rend()   const throw() { return const_reverse_iterator( NULL   );     }
		
		//======================================================================
		// misc
		//======================================================================
		inline void max_available( size_t n ) throw()
		{
			while( pool_.size > n ) node_type::release( pool_.query() );
		}
        
        inline virtual void reverse() throw()
        {
            list_.reverse();
        }

        inline virtual void reverse_last(const size_t n) throw()
        {
            list_.reverse_last(n);
        }

        
        inline friend std::ostream & operator<<( std::ostream &os, const list &a )
		{
			os << "[ ";
            for( const node_type *node = a.list_.head; node; node=node->next)
            {
                os << node->data << ' ';
            }
			os << "]'";
			return os;
		}
        
        template <typename FUNC>
        inline void sort( FUNC &cmp )
        {
            void *args = (void *)&cmp;
            core::merging<node_type>:: template sort<core::list_of>(list_,__compare_nodes<FUNC>,args);
        }

        template <typename FUNC>
        inline void uniq( FUNC &cmp ) throw()
        {
            list_type tmp;
            while( list_.size )
            {
                node_type *n     = list_.pop_front();
                bool       found = false;
                for(const node_type *i=tmp.head;i;i=i->next)
                {
                    if( 0 == cmp(i->data,n->data) )
                    {
                        found = true;
                        break;
                    }
                }

                if( found )
                {
                    keep(n);
                }
                else
                {
                    tmp.push_back(n);
                }

            }
            list_.swap_with(tmp);
        }

        inline node_type       *head()  throw()      { return list_.head; }
        inline const node_type *head() const throw() { return list_.head; }
        
	protected:
        typedef core::list_of<node_type> list_type;
        typedef core::pool_of<node_type> pool_type;

		list_type list_;
		pool_type pool_;
		
	private:
		inline void kill() throw() { list_.delete_with( node_type::destroy ); pool_.delete_with( node_type::release ); }
		inline void keep( node_type *node ) throw() { destruct( &node->data ); pool_.store(node); }
		inline node_type *make( param_type obj )
		{
			node_type *node = pool_.size ? pool_.query() : node_type::acquire();
			try
			{
				new ( & node->data ) mutable_type( obj );
			}
			catch(...)
			{
				pool_.store(node);
                throw;
			}
            return node;
		}

        //! C style ops
        inline node_type *make( int2type<true> )
        {
            node_type *node = pool_.size ? pool_.query() : node_type::acquire();
            bzset(node->data);
            return node;
        }

        //! C style ops
        inline node_type *make( int2type<false> )
        {
            node_type *node = pool_.size ? pool_.query() : node_type::acquire();
            try
            {
                new ( & node->data ) mutable_type();
            }
            catch (...)
            {
                pool_.store(node);
                throw;
            }
            return node;
        }

		
		virtual const_type &get_front() const throw() { assert(list_.size>0); return list_.head->data; }
		virtual const_type &get_back()  const throw() { assert(list_.size>0); return list_.tail->data; }
        
        template <typename FUNC>
        static inline
        int __compare_nodes( const node_type *lhs, const node_type *rhs, void *args)
        {
            assert(args);
            FUNC &cmp = *(FUNC *)args;
            return cmp(lhs->data,rhs->data);
        }
        
        virtual const type &get_data(size_t indx) const throw()
        {
            assert(indx>0);
            assert(indx<=size());
            return list_.fetch(--indx)->data;
        }
	};
	
}

#endif
