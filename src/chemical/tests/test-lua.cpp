
#include "yocto/chemical/lua.hpp"
#include "yocto/chemical/integrator.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/lua/lua-state.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/math/core/tao.hpp"

using namespace yocto;
using namespace chemical;
using namespace math;

namespace
{
    class Modifier
    {
    public:
        explicit Modifier()
        {
        }
        
        virtual ~Modifier() throw()
        {
        }
        
        void Compute( array<double> &dCdt, double t, const array<double> &C)
        {
            //std::cerr << "Compute@t=" << t << ", C=" << C << std::endl;
            tao::ld(dCdt,0);
        }
        
    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(Modifier);
    };
}

static inline void save( ios::ostream &fp,
                        const double   t,
                        const array<double> &C,
                        const library &lib)
{
    fp("%.15g",t);
    if( lib.search("H+") )
    {
        fp(" %.15g", lib.pH(C) );
    }
    for(size_t i=1;i<=C.size();++i)
    {
        fp(" %.15g", C[i]);
    }
    fp("\n");
}

YOCTO_UNIT_TEST_IMPL(lua)
{
    Lua::State::Pointer vm( new Lua::State() );
    
    if(argc>1)
    {
        vm->DoFile( argv[1] );
    }
    
    for(int i=2;i<argc;++i)
    {
        vm->DoString(argv[i]);
    }
    
    
    library lib("species");
    __lua::load(vm,lib);
    std::cerr << lib.name << ":" << std::endl;
    std::cerr << lib << std::endl;
    
    equilibria cs("cs");
    __lua::load(vm,cs,lib);
    std::cerr << cs << std::endl;
    
    vector<double> C0(cs.M,0);
    {
        boot loader("ini");
        __lua::load(vm,loader,lib);
        std::cerr << loader << std::endl;
        
        loader.guess(C0,cs,0.0);
        lib.display(C0);
    }
    
    if( vm->Has("add") )
    {
        std::cerr << "Start titration..." << std::endl;
        const bool has_proton = lib.search("H+") != NULL;
        if(has_proton)
        {
            std::cerr << "pH0=" << lib.pH(C0) << std::endl;
        }
        vector<double> C1(cs.M,0);
        {
            boot loader("add");
            __lua::load(vm,loader,lib);
            std::cerr << loader << std::endl;
            loader.guess(C1,cs,0);
        }
        std::cerr << "using" << std::endl;
        lib.display(C1);
        
        std::cerr << "performing full dosage" << std::endl;
        vector<double> Cmix(cs.M,0);
        const double V0 = 10;
        const size_t Nd = 1000;
        {
            ios::wcstream fp("dosage.dat");
            for(size_t i=0;i<=Nd;++i)
            {
                const double V1 = (2*V0*i)/Nd;
                
                if(!cs.mix(Cmix,C0,V0,C1,V1,0.0))
                {
                    throw exception("cannot mix @V1=%g", V1);
                }
                std::cerr << ".";
                fp("%g",V1);
                
                
                if(has_proton)
                {
                    fp(" %g", lib.pH(Cmix) );
                }
                
                for(size_t j=1;j<=cs.M;++j)
                {
                    fp(" %g", Cmix[j]);
                }
                fp("\n");
            }
            std::cerr << std::endl << std::endl;
        }
        
        std::cerr << "Start integration..." << std::endl;
        {
            ios::wcstream fp("intg.dat");
            Modifier   modif;
            ode_type   diffeq( &modif, & Modifier::Compute);
            integrator intg( 1e-7 );
            intg.start(cs.M);
            
            double t  = 0;
            double dt = 1;
            tao::set(C1,C0);
            save(fp,t,C1,lib);
            while(t<120)
            {
                const double t_new = t+dt;
                intg( diffeq, cs, C1, t, t_new);
                t=t_new;
                save(fp,t,C1,lib);
                //break;
            }
        }
    }
    
    
    
}
YOCTO_UNIT_TEST_DONE()

