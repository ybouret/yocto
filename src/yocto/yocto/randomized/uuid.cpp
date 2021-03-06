#include "yocto/randomized/uuid.hpp"
#include "yocto/randomized/bits.hpp"

#include "yocto/threading/singleton.hpp"
#include "yocto/string/env.hpp"
#include "yocto/hashing/sha1.hpp"
#include "yocto/sys/wtime.hpp"
#include "yocto/sys/pid.hpp"

#include <cstring>

#if defined(YOCTO_WIN)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace yocto
{
	
	namespace 
	{

		
		
		
		class uuidmgr : public singleton<uuidmgr>
		{
		public:
			typedef hashing::sha1 hasher;
			
			void build( void *dst, size_t len ) throw()
			{
				YOCTO_LOCK(access);
				
				hasher_.set();
				
				//-- node information
				hashing::function *pfn = &hasher_;
				try
				{
					environment::get( env_cb, pfn );
				}
				catch(...)
				{
					
				}
				
				//-- program information
				const pid_type my_pid = get_process_id();
				
				hasher_.run( &my_pid, sizeof(my_pid ) );
				
				//-- stamp random
                const uint32_t dw = Randomized::Bits::Crypto().next32();
				hasher_.run( &dw, sizeof(uint32_t) );
				
				// sha1-128
				hasher_.get( dst, len );
			}


		private:
			YOCTO_DISABLE_COPY_AND_ASSIGN(uuidmgr);
			friend class singleton<uuidmgr>;
			
			hasher                             hasher_;

			explicit uuidmgr() throw() :
			hasher_()
			{
								
			}
			
			virtual ~uuidmgr() throw()
			{
				
			}
			
			static void env_cb( const string &name, const string &value, void *args )
			{
				hashing::function &H = *(hashing::function *) args;
				H( name  );
				H( value );
			}
			
			static const threading::longevity life_time = object::provider::life_time - 1;
			static const char name[];
		};
		
		const char uuidmgr:: name[] = "uuidmgr";
		

	}
	

	
	
#if defined(_MSC_VER)
#pragma warning ( disable :  4351 )
#endif
	uuid:: uuid() :
	byte_()
	{
		uuidmgr::instance().build( byte_, sizeof(byte_)  );
	}
	
	uuid:: ~uuid() throw()
	{
	}
	
	uuid:: uuid( const uuid &other ) throw() : byte_()
	{
		memcpy( byte_, other.byte_, sizeof(byte_) );
	}
	
	size_t uuid:: length() const throw()
	{
		return __length;
	}
	
	const void * uuid:: get_address() const throw()
	{
		return (const void * ) byte_;
	}
	
	static
	inline void uuid2word( char * &dst, unsigned x )
	{
		static const char xch[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
		
		dst[0] = xch[ ( x >>  4  ) & 0x0f ];
		dst[1] = xch[ ( x & 0x0f )        ];
		dst += 2;
	}
	
	string uuid:: to_string() const
	{
		string ans( 36, ' ' );
		char  *dst = &ans[0];
		for( size_t i=0; i < 4; ++i )
		{
			uuid2word( dst, byte_[i] );
		}
		*(dst++) = '-';
		for( size_t i=4; i < 6; ++i )
		{
			uuid2word( dst, byte_[i] );
		}
		*(dst++) = '-';
		for( size_t i=6; i < 8; ++i )
		{
			uuid2word( dst, byte_[i] );
		}
		*(dst++) = '-';
		for( size_t i=8; i < 10; ++i )
		{
			uuid2word( dst, byte_[i] );
		}
		*(dst++) = '-';
		for( size_t i=10; i < 16; ++i )
		{
			uuid2word( dst, byte_[i] );
		}
		return ans;
	}
	
	
	
}
