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

            vector<size_t> _incr_index(M,as_capacity);
            vector<double> _incr_value(M,as_capacity);

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
                size_t incr_index = 0;
                double incr_value = 0;
                _incr_value.free();
                _incr_index.free();
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
                        // increasing a concentration: stop at zero
                        //______________________________________________________
                        if(c<0)
                        {
                            //this is one of the bad!
                            const double tmp = (-c)/d;
                            std::cerr << "corrected, advance at least * " << tmp << std::endl;
                            _incr_value.__push_back(tmp);
                            _incr_index.__push_back(j);
                            if( (incr_index<=0) || (tmp<incr_value) )
                            {
                                incr_value = tmp;
                                incr_index = j;
                            }
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
                const size_t incr_count = _incr_value.size();
                if(incr_count<=0)
                {
                    std::cerr << "unable to decrease!" << std::endl;
                    return false;
                }


                co_qsort(_incr_value,_incr_index);
                std::cerr << "incr_index: " << _incr_index << std::endl;
                std::cerr << "incr_value: " << _incr_value << std::endl;

                if(incr_index<=0)
                {
                    std::cerr << "unable to decrease!" << std::endl;
                    return false;
                }
                std::cerr << "increase  : " << incr_value << " @" << incr_index << std::endl;
                assert(incr_value>0);

                double fac = incr_value;
                size_t idx = incr_index;
                if(decr_index)
                {
                    std::cerr << "decrease  : " << decr_value << " @" << decr_index << std::endl;
                    if(decr_value<incr_value)
                    {
                        std::cerr << "\ttakes precedence!" << std::endl;
                        fac = decr_value;
                        idx = decr_index;
                    }
                }

                // updating carefully
                for(size_t j=M;j>0;--j)
                {
                    if(!active[j])
                    {
                        assert(Fabs(dC[j])<=0);
                        continue;
                    }

                    const bool is_bad = (C[j]<0);
                    C[j] += fac * dC[j];
                    if(is_bad)
                    {
                         if(C[j]>=0) C[j]=0;
                    }
                    else
                    {
                        if(C[j]<=0) C[j]=0;
                    }
                }
                C[idx] = 0;





                return false;
            }

        }
        
    }

}

