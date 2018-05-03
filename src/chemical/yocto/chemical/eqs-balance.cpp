#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/adjoint.hpp"
#include "yocto/sort/quick.hpp"

namespace yocto
{
    using namespace math;
    
    namespace chemical
    {



        
        bool equilibria:: balance() throw()
        {
            std::cerr << "active=" << active << std::endl;

            vector<size_t> incr_index(M,as_capacity);
            vector<double> incr_value(M,as_capacity);

            //__________________________________________________________________
            //
            // construct the bad vector
            //__________________________________________________________________
            size_t nbad  = 0;
            for(size_t j=M;j>0;--j)
            {
                beta[j] = 0;
                const double Cj = C[j];
                if(active[j]&&(Cj<0))
                {
                    beta[j] = -Cj;
                    ++nbad;
                }
            }
            if(nbad<=0)
            {
                std::cerr << "balanced" << std::endl;
                return true;
            }
            else
            {
                //______________________________________________________________
                //
                // try to resorb it
                //______________________________________________________________
                std::cerr << "C   =" << C << std::endl;
                std::cerr << "beta=" << beta << std::endl;
                tao::mul_and_div(dC,Psi,beta,Det);
                std::cerr << "dC  =" << dC << std::endl;

                //______________________________________________________________
                //
                // analyze the step
                //______________________________________________________________
                size_t decr_index = 0;
                double decr_value = 0;
                incr_value.free();
                incr_index.free();
                for(size_t j=M;j>0;--j)
                {
                    //__________________________________________________________
                    //
                    // gather status
                    //__________________________________________________________
                    if(!active[j]) continue;
                    const double c = C[j];
                    const double d = dC[j];
                    std::cerr << "\t@" << j << " : C=" << c << ", dC=" << d << " => ";

                    if(d>0)
                    {
                        //______________________________________________________
                        //
                        // increasing a concentration:
                        //______________________________________________________
                        if(c<0)
                        {
                            //this is one of the bad!
                            const double tmp = (-c)/d;
                            std::cerr << "corrected, advance at least * " << tmp << std::endl;
                            incr_value.__push_back(tmp);
                            incr_index.__push_back(j);
                        }
                        else { std::cerr << "no effect..." << std::endl; }
                    }
                    else if(d<0)
                    {
                        //______________________________________________________
                        //
                        // decreasing a concentration: don't let
                        // them become invalid!
                        //______________________________________________________
                        if(c<=0)
                        {
                            //this is one of the bad: too bad...
                            std::cerr << "blocked!" << std::endl;
                            return false;
                        }
                        else
                        {
                            const double tmp = c/(-d);
                            std::cerr << "don't move more than * " << tmp << std::endl;
                            if( (decr_index<=0) || (tmp<decr_value) )
                            {
                                decr_index = j;
                                decr_value = tmp;
                            }
                        }
                    }
                }
                const size_t incr_count = incr_value.size();
                if(incr_count<=0)
                {
                    std::cerr << "unable to decrease!" << std::endl;
                    return false;
                }

                co_qsort(incr_value,incr_index);
                std::cerr << "incr_index: " << incr_index << std::endl;
                std::cerr << "incr_value: " << incr_value << std::endl;
                
                if(decr_index)
                {
                    std::cerr << "decrease  : " << decr_value << " @" << decr_index << std::endl;
                }
                else
                {
                    std::cerr << "no decreasing limit" << std::endl;
                }

                return false;
            }

        }
        
    }

}

