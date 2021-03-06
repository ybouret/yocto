#include "yocto/mpi/mpi.hpp"

#include <cstring>
#include <cstdarg>

namespace yocto
{
	
       
    
    
    //==========================================================================
    //
    // mpi exceptions
    //
    //==========================================================================
#if defined(_MSC_VER)
	// init of string_
#pragma warning ( disable : 4351 )
#endif
	mpi:: exception:: exception( int err, const char *fmt, ... ) throw() :
	yocto::exception(),
	code( err ),
	string_()
	{
		va_list ap;
		va_start(ap,fmt);
		format(fmt,&ap);
		memset( string_, 0, sizeof(string_) );
		
	}
	
	mpi:: exception:: exception( const mpi::exception &e ) throw() :
	yocto::exception(e),
	code( e.code ),
	string_()
	{
		memcpy( string_, e.string_, sizeof(string_) );
	}
	
	mpi:: exception:: ~exception() throw()
	{
	}
	
	const char * mpi:: exception:: what() const throw()
	{
		return string_;
	}
	
	//==========================================================================
	namespace
	{
		static int *    mpi_argc_ = NULL;
		static char *** mpi_argv_ = NULL;
        static int      mpi_level_ = -1;
        
		static void mpi_reset_() throw()
		{
			mpi_argc_ = NULL;
			mpi_argv_ = NULL;
            mpi_level_ = -1;
		}
	}
	
	void mpi:: clearProcessorName() throw()
	{
		*(int *) &ProcessorNameLength = 0;
		memset( (void *) ProcessorName, 0, sizeof(ProcessorName) );
	}
	
	void mpi:: onFinalize() throw()
	{
        *(int *) & CommWorldSize    = 0;
		*(int *) & CommWorldRank    = 0;
		*(int *) & CommWorldLast    = 0;
		clearProcessorName();
		(void) MPI_Finalize();
	}
	
    const char *mpi:: ThreadLevelName() const throw()
    {
        switch( ThreadLevel )
        {
            case MPI_THREAD_SINGLE:        return "MPI_THREAD_SINGLE";
            case MPI_THREAD_FUNNELED:      return "MPI_THREAD_FUNNELED";
            case MPI_THREAD_SERIALIZED:    return "MPI_THREAD_SERIALIZED";
            case MPI_THREAD_MULTIPLE:      return "MPI_THREAD_MULTIPLE";
            default:
                break;
        }
        return "UNKNOWN";
        
    }
    
    
	mpi:: mpi() :
	CommWorldSize(0),
	CommWorldRank(0),
	CommWorldLast(0),
    IsFirst(true),
    IsFinal(true),
    IsParallel(false),
    IsSerial(true),
    CommTime(0),
	ProcessorNameLength(0),
	ProcessorName(),
    ThreadLevel(0),
    CommWorldID(),
    db(32,as_capacity),
	hasher()
	{
		if( NULL == mpi_argc_ || NULL == mpi_argv_ )
		{
			mpi_reset_();
			throw yocto::exception("no possible direct mpi::instance()");
		}
		
		
		try
		{
            
            //==================================================================
            // MPI basic setup
            //==================================================================
			int err = MPI_Init_thread( mpi_argc_, mpi_argv_ , mpi_level_, (int*)&ThreadLevel);
			if( err != MPI_SUCCESS )
			{
				throw mpi::exception( err, "MPI_Init_thread()");
			}
			
			err = MPI_Comm_size( MPI_COMM_WORLD, (int *) & CommWorldSize );
			if( err != MPI_SUCCESS )
			{
				throw mpi::exception( err, "MPI_Comm_size(MPI_COMM_WORLD)");
			}
			
			(int  &) CommWorldLast = CommWorldSize - 1;
			(bool &) IsParallel    = CommWorldSize > 1;
            (bool &) IsSerial      = !IsParallel;
            
			err = MPI_Comm_rank( MPI_COMM_WORLD, (int *) & CommWorldRank );
			if( err != MPI_SUCCESS )
			{
				throw mpi::exception( err, "MPI_Comm_rank(MPI_COMM_WORLD)");
			}
			assert( CommWorldRank <= CommWorldLast );
			(bool &)IsFirst = (CommWorldRank == 0);
			(bool &)IsFinal = (CommWorldRank == CommWorldLast);
            
            //==================================================================
            // MPI advanced setup
            //==================================================================

            // get the processor's name
			clearProcessorName();
			err = MPI_Get_processor_name( (char *) ProcessorName, (int *) & ProcessorNameLength );
			if( err != MPI_SUCCESS )
			{
				clearProcessorName();
                // TODO: throw an exception ?
			}

            // generate the database of types
            gendb();
            
            (string &)CommWorldID = vformat("%d.%d", CommWorldSize, CommWorldRank);
        }
        catch(...)
        {
            mpi_reset_();
            onFinalize();
            throw;
        }
    }
    
    mpi & mpi:: init( int * argc, char ***argv, int requestedThreadLevel)
    {
        YOCTO_LOCK(access);
        if( NULL == argc || NULL == argv )
        {
            throw yocto::exception("yocto::mpi::Init( INVALID ARGS )");
        }
        
        mpi_argc_  = argc;
        mpi_argv_  = argv;
        mpi_level_ = requestedThreadLevel;
        mpi & ans = mpi::instance();
        mpi_reset_();
        return ans;
    }
    
    
    void mpi:: Finalize() throw()
    {
        destroy();
    }
    
    mpi:: ~mpi() throw()
    {
        onFinalize();
    }
    
    
    int mpi:: CommWorldNext() const throw()
    {
        assert( CommWorldSize > 0 );
        return CommWorldRank >= CommWorldLast ? 0 : CommWorldRank+1;
    }
    
    int mpi:: CommWorldPrev() const throw()
    {
        assert( CommWorldSize > 0 );
        return CommWorldRank > 0 ? CommWorldRank-1 : CommWorldLast;
    }
    
    const char mpi::name[] = "MPI";
    
    int mpi:: Comm_rank( MPI_Comm comm ) const
    {
        int       r   = -1;
        const int err = MPI_Comm_rank(comm, &r);
        if( err != MPI_SUCCESS )
            throw mpi::exception( err, "MPI_Comm_rank");
        return r;
    }
    
    double mpi::Wtime() const throw()
    {
        return MPI_Wtime();
    }
    
    void  mpi:: WaitFor( double nsec) const throw()
    {
        const double stamp = Wtime();
        while( Wtime() - stamp < nsec )
            ;
    }
}

#include "yocto/string/env.hpp"


namespace yocto
{
    int mpi:: EnvThreadLevel()
    {
        string       value;
        const string name = "YOCTO_MPI";
        
        if(!environment::get(value,name))
        {
            return MPI_THREAD_SINGLE;
        }
        
        if( "SINGLE"     == value ) return MPI_THREAD_SINGLE;
        if( "MULTIPLE"   == value ) return MPI_THREAD_MULTIPLE;
        if( "FUNNELED"   == value ) return MPI_THREAD_FUNNELED;
        if( "SERIALIZED" == value ) return MPI_THREAD_SERIALIZED;
        
        throw yocto::exception("unknown MPI_THREAD_%s", value.c_str());
    }
    
}
