
#include "yocto/math/fit/fit.hpp"
#include "yocto/math/ztype.hpp"
#include "yocto/code/utils.hpp"

namespace yocto
{
    namespace math
    {
        namespace Fit
        {
            template <> SampleType<real_t>:: ~SampleType() throw() {}
            template <> SampleType<real_t>::  SampleType(const size_t n) :
            SampleInfo(n),
            u(n,as_capacity),
            beta(n,as_capacity),
            curv(),
            D2(0)
            {
            }

            template <>
            void SampleType<real_t>:: allocate(const size_t nvar)
            {
                u.   make(nvar);
                dFdu.make(nvar);
                beta.make(nvar);
                curv.make(nvar); //! large model
            }


            template <>
            void SampleType<real_t>:: display(std::ostream &os,
                                              const Array  &aorg,
                                              const Array  &aerr,
                                              const char   *prefix) const
            {
                const size_t nvar = variables.size();
                assert(aorg.size()==nvar);
                assert(aerr.size()==nvar);
                const size_t max_len = variables.getMaxNameLength();
                vector<string> vorg(nvar,as_capacity);
                vector<string> verr(nvar,as_capacity);
                size_t i=1;
                size_t pad_org = 0;
                size_t pad_err = 0;
                for(Variables::const_iterator j=variables.begin();j!=variables.end();++i,++j)
                {
                    {
                        string s = (*j).name;
                        for(size_t k = (*j).name.size(); k<=max_len;++k)
                        {
                            s += ' ';
                        }
                        s += ':';
                        s += ' ';

                        s += vformat("%.15g",aorg[i]);
                        pad_org = max_of(s.size(),pad_org);
                        vorg.push_back(s);
                    }

                    {
                        string s = "+/- ";
                        s += vformat("%.15g",aerr[i]);
                        pad_err = max_of(s.size(),pad_err);
                        verr.push_back(s);
                    }
                }

                const string pfx = prefix;
                for(i=1;i<=nvar;++i)
                {
                    os << pfx;
                    os << vorg[i]; for(size_t j=vorg[i].size();j<=pad_org;++j) os << ' ';
                    os << verr[i]; for(size_t j=verr[i].size();j<=pad_err;++j) os << ' ';
                    os << '(';
                    real_t       err = 0;
                    const real_t v   = Fabs(aerr[i]);
                    if(v>0)
                    {
                        err = v/max_of(Fabs(aorg[i]-v),Fabs(aorg[i]+v));
                    }
                    err *= 100;
                    const string s = vformat("%8.3f %%",err);
                    os << s;
                    os << ')';
                    os << std::endl;
                }

            }
        }
    }
}

namespace yocto
{
    namespace math
    {
        namespace Fit
        {

            template <>
            real_t Gradient<real_t>:: eval_f(const real_t x, const Array &a)
            {
                assert(var);
                assert(pfn);
                return (*pfn)(x,a,*var);
            }

            template <>
            Gradient<real_t>:: Gradient() :
            derivatives<real_t>(),
            scale( REAL(1.0e-4) ),
            pfn(0),
            var(0),
            dh(),
            f( this, & Gradient<real_t>::eval_f )
            {
            }

            template <>
            Gradient<real_t>:: ~Gradient() throw()
            {
            }

            template <>
            void Gradient<real_t>::  compute(Array           &dFda,
                                             Function        &F,
                                             const real_t     x,
                                             const Array     &a,
                                             const Variables &v)
            {
                derivatives<real_t> &drvs = *this;
                pfn = &F;
                var = &v;
                dh.make(a.size(),scale);
                drvs.gradient(dFda,f, x, a, dh);
            }

        }

    }
}

namespace yocto
{
    namespace math
    {
        namespace Fit
        {

            template <> Sample<real_t>:: ~Sample() throw() {}


            template <> Sample<real_t>:: Sample(const Array &userX,
                                                const Array &userY,
                                                Array       &userZ,
                                                const size_t capa) :
            SampleType<real_t>(capa),
            X(userX), Y(userY), Z(userZ)
            {

            }

            template <>
            void Sample<real_t>:: link()
            {
                link_to(variables);
                allocate(variables.size());
            }

            template <>
            void Sample<real_t>:: assign(const Array &a) const
            {
                assert(u.size()==variables.size()       || die("not linked") );
                assert(indices.size()==variables.size() || die("not linked") );
                for(size_t i=u.size();i>0;--i)
                {
                    const size_t j=indices[i];
                    assert(j>0        ||die("corrupted indices"));
                    assert(j<=a.size()||die("corrupted link")   );
                    u[i] = a[j];
                }
            }


            template <>
            real_t Sample<real_t>::computeD2(Function           &F,
                                             const Array        &a) const
            {
                assign(a);
                const size_t N = X.size();
                assert(Y.size()==N);
                assert(Z.size()==N);
                D2 = 0;
                for(size_t j=N;j>0;--j)
                {
                    const real_t del = Y[j] - (Z[j]=F(X[j],u,variables));
                    D2 += del*del;
                }
                return D2;
            }

            template <>
            real_t Sample<real_t>::computeD2(Function           &F,
                                             const Array        &a,
                                             Gradient<real_t>   &grad) const
            {
                assign(a);
                const size_t N = X.size();
                assert(Y.size()==N);
                assert(Z.size()==N);
                const size_t nvar = u.size();

                for(size_t k=nvar;k>0;--k)
                {
                    beta[k] = 0;
                    Matrix::row  &curv_k = curv[k];
                    for(size_t l=nvar;l>0;--l)
                    {
                        curv_k[l] = 0;
                    }
                }

                D2 = 0;
                for(size_t j=N;j>0;--j)
                {
                    const real_t Xj  = X[j];
                    const real_t del = Y[j] - (Z[j]=F(Xj,u,variables));
                    grad.compute(dFdu,F,Xj,u,variables);
                    for(size_t k=nvar;k>0;--k)
                    {
                        const real_t  dFdu_k = dFdu[k];
                        Matrix::row  &curv_k = curv[k];
                        beta[k] += del * dFdu_k;
                        for(size_t l=k;l>0;--l)
                        {
                            curv_k[l] += dFdu_k * dFdu[l];
                        }
                    }
                    D2 += del*del;
                }

                // symetrizing
                for(size_t k=nvar;k>0;--k)
                {
                    for(size_t l=k-1;l>0;--l)
                    {
                        curv[l][k] = curv[k][l];
                    }
                }
                return D2;
            }

            template <>
            size_t Sample<real_t>:: items() const throw()
            {
                assert( X.size() == Y.size() );
                assert( Y.size() == Z.size() );
                return X.size();
            }

            template <>
            size_t Sample<real_t>:: count() const throw()
            {
                return 1;
            }
            
            template <>
            real_t Sample<real_t>:: SStot() const throw()
            {
                const size_t N = Y.size();
                if(N>0)
                {
                    real_t ave = 0;
                    for(size_t i=N;i>0;--i)
                    {
                        ave += Y[i];
                    }
                    ave /= N;

                    real_t ans = 0;
                    for(size_t i=N;i>0;--i)
                    {
                        ans += Square(Y[i]-ave);
                    }
                    return ans;
                }
                else
                {
                    return 0;
                }
            }

            template <>
            real_t Sample<real_t>:: correlation() const throw()
            {
                assert(Y.size()==Z.size());
                const size_t N = Y.size();
                if(N>1)
                {
                    real_t ay=0;
                    real_t az=0;
                    for(size_t i=N;i>0;--i)
                    {
                        ay += Y[i];
                        az += Z[i];
                    }
                    ay /= N;
                    az /= N;

                    real_t sum_yy = 0;
                    real_t sum_zz = 0;
                    real_t sum_yz = 0;
                    for(size_t i=N;i>0;--i)
                    {
                        const real_t dy = Y[i] - ay;
                        const real_t dz = Z[i] - az;
                        sum_yy += dy*dy;
                        sum_zz += dz*dz;
                        sum_yz += dy*dz;
                    }

                    return sum_yz / ( Sqrt(sum_yy*sum_zz) + numeric<real_t>::tiny );
                }
                else
                {
                    return 1;
                }
            }

        }
    }
}

namespace yocto
{
    namespace math
    {
        namespace Fit
        {

            template <> Samples<real_t>:: ~Samples() throw() {}
            template <> Samples<real_t>::  Samples(const size_t capa_samples, const size_t capa_variables) :
            SampleType<real_t>(capa_variables),
            Sample<real_t>::Collection(capa_samples,as_capacity)
            {
            }

            template <>
            Sample<real_t> & Samples<real_t>::add(const Array &userX, const Array &userY, Array &userZ, const size_t capa_local)
            {
                Sample<real_t>::Pointer pS( new Sample<real_t>(userX,userY,userZ,capa_local) );
                push_back(pS);
                return *pS;
            }

            template <>
            void Samples<real_t>:: link()
            {
                array< Sample<real_t>::Pointer > &self = *this;
                link_to(variables);
                allocate(variables.size());
                for(size_t i=size();i>0;--i)
                {
                    Sample<real_t> &S = *self[i];
                    S.link_to(variables);
                    S.allocate( S.variables.size() );
                }
            }


            template <>
            real_t Samples<real_t>::computeD2(Function           &F,
                                              const Array        &a) const
            {
                const array< Sample<real_t>::Pointer > &self = *this;
                D2 = 0;
                for(size_t i=size();i>0;--i)
                {
                    D2 += self[i]->computeD2(F,a);
                }
                return D2;
            }

            template <>
            real_t Samples<real_t>::computeD2(Function          &F,
                                              const Array        &a,
                                              Gradient<real_t>   &grad) const
            {
                const array< Sample<real_t>::Pointer > &self = *this;
                const size_t nvar = u.size();

                for(size_t k=nvar;k>0;--k)
                {
                    beta[k] = 0;
                    for(size_t l=nvar;l>0;--l)
                    {
                        curv[k][l] = 0;
                    }
                }

                D2 = 0;
                for(size_t i=size();i>0;--i)
                {
                    const Sample<real_t> &sample = *self[i];
                    D2 += sample.computeD2(F,a,grad);

                    // dispatch values
                    const size_t svar = sample.indices.size();
                    for(size_t j=svar;j>0;--j)
                    {
                        const size_t jsub = sample.indices[j];
                        beta[ jsub ] += sample.beta[j];
                        for(size_t k=svar;k>0;--k)
                        {
                            const size_t ksub = sample.indices[k];
                            curv[jsub][ksub] += sample.curv[j][k];
                        }
                    }
                }
                return D2;

            }

            template <>
            size_t Samples<real_t>:: items() const throw()
            {
                const array< Sample<real_t>::Pointer > &self = *this;
                size_t ans = 0;
                for(size_t i=size();i>0;--i)
                {
                    const Sample<real_t> &sample = *self[i];
                    ans += sample.items();
                }
                return ans;
            }

            template <>
            real_t Samples<real_t>:: SStot() const throw()
            {
                const array< Sample<real_t>::Pointer > &self = *this;
                real_t ans = 0;
                for(size_t i=size();i>0;--i)
                {
                    const Sample<real_t> &sample = *self[i];
                    ans += sample.SStot();
                }
                return ans;
            }
            
            template <>
            size_t Samples<real_t>:: count() const throw()
            {
                return size();
            }

            template <>
            real_t Samples<real_t>:: correlation() const throw()
            {
                return 0;
            }
        }

    }
}


#include "yocto/math/types.hxx"
#include "yocto/code/utils.hpp"
#include "yocto/code/ipower.hpp"
#include "yocto/math/core/lu.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/ios/ocstream.hpp"

namespace yocto
{
    namespace math
    {
        namespace Fit
        {
            template <>
            LS<real_t>:: ~LS() throw()
            {
            }

            template <>
            real_t LS<real_t>:: eval1d(const real_t u)
            {
                assert(psm);
                assert(pfn);
                assert(pa0);
                tao::setprobe(atmp,*pa0,u,step);
                return psm->computeD2(*pfn,atmp);
            }

            template <>
            LS<real_t>:: LS() :
            Gradient<real_t>(),
            nvar(0),
            psm(0),
            pfn(0),
            pa0(0),
            atry(),
            step(),
            p10(0),
            lambda(0),
            min_p10( Floor(Log10(numeric<real_t>::epsilon)) ),
            max_p10( Floor(Log10(numeric<real_t>::maximum))/2 ),
            Rsq(0),
            cycle(0),
            verbose(false),
            correct(false)
            {
                //std::cerr << "min_p10=" << min_p10 << std::endl;
                //std::cerr << "max_p10=" << max_p10 << std::endl;
            }

            template <>
            void LS<real_t>:: compute_lambda() throw()
            {
                p10 = clamp(min_p10,p10,max_p10);
                if(p10<=min_p10)
                {
                    lambda = 0;
                }
                else
                {
                    if(p10>=0)
                    {
                        lambda = ipower(REAL(10.0),p10);
                    }
                    else
                    {
                        lambda = ipower(REAL(0.1),-p10);
                    }
                }

            }

            template <>
            bool LS<real_t>:: compute_cinv(const Matrix &curv)
            {
                assert(curv.is_square());
                assert(curv.rows>0);
                assert(cinv.is_square());
                assert(cinv.rows==curv.rows);

                const size_t nvar = curv.rows;


                for(;p10<max_p10;++p10)
                {
                    compute_lambda();
                    const real_t fac = REAL(1.0) + lambda;
                    for(size_t i=nvar;i>0;--i)
                    {
                        for(size_t j=nvar;j>i;--j)
                        {
                            cinv[i][j] = curv[i][j];
                        }
                        cinv[i][i] = fac * curv[i][i]; // TODO: add a NaN protection!!!!
                        for(size_t j=i-1;j>0;--j)
                        {
                            cinv[i][j] = curv[i][j];
                        }
                    }
                    if( LU<real_t>::build(cinv) )
                    {
                        return true;
                    }
                }
                return false;
            }

#define CALLBACK() do \
{\
if( (NULL!=cb) && (false==(*cb)(sample,aorg))) return false;\
} while(false)

            static inline
            void __regularize(matrix<real_t>    &curv,
                              array<real_t>     &beta,
                              const array<bool> &used ) throw()
            {
                const size_t nvar = beta.size();
                for(size_t k=nvar;k>0;--k)
                {
                    if(!used[k])
                    {
                        beta[k] = 0.0;
                        for(size_t l=nvar;l>0;--l)
                        {
                            curv[k][l] = curv[l][k] = 0;
                        }
                        curv[k][k] = 1;
                    }
                }
            }



            template <>
            bool LS<real_t>:: run(SampleType<real_t>   &sample,
                                  Function             &F,
                                  Array                &aorg,
                                  const array<bool>    &used,
                                  Array                &aerr,
                                  Callback             *cb)
            {

                //______________________________________________________________
                //
                // Initialize
                //______________________________________________________________
                const size_t nvar = sample.variables.size();
                assert(aorg.size()==nvar);
                assert(used.size()==nvar);
                assert(aerr.size()==nvar);
                
                if(nvar<=0)
                {
                    return true;
                }
                
                sample.link();
                step.make(nvar);
                atry.make(nvar);
                cinv.make(nvar);
                atmp.make(nvar);
                tao::ld(aerr,0);
                Matrix &curv = sample.curv;
                Array  &beta = sample.beta;
                p10          = min_p10;
                Rsq          = 0;
                cycle        = 0;
                psm          = &sample;
                pfn          = &F;
                pa0         = &aorg;
                
                //______________________________________________________________
                //
                // start cycle
                //______________________________________________________________
            CYCLE:
                {
                    ++cycle;
                    //__________________________________________________________
                    //
                    // computation of D2, beta and curv
                    //__________________________________________________________
                    const real_t D2_org = sample.computeD2(F,aorg,*this);

                    CALLBACK();
                    
                    //__________________________________________________________
                    //
                    // regularize curvature
                    //__________________________________________________________
                    __regularize(curv,beta,used);
                    if(verbose)
                    {
                        std::cerr << "fit.cycle = " << cycle  << std::endl;
                        std::cerr << "fit.D2    = " << D2_org << std::endl;
                        std::cerr << "curv      = " << curv   << std::endl;
                    }
                    //__________________________________________________________
                    //
                    // compute inverse curvature from current p10
                    //__________________________________________________________
                BACKTRACK:
                    {
                        if(!compute_cinv(curv))
                        {
                            return false;
                        }

                        //______________________________________________________
                        //
                        // compute step => ATRY
                        //______________________________________________________
                        tao::set(step,beta);
                        LU<real_t>::solve(cinv,step);
                        if( tao::norm_sq(step) <= REAL(0.0) )
                        {
                            goto EXTREMUM;
                        }
                        tao::setsum(atry,aorg,step);

                        //______________________________________________________
                        //
                        // check numerical convergence
                        //______________________________________________________
                        bool cvg = true;
                        for(size_t i=nvar;i>0;--i)
                        {
                            if( Fabs(atry[i]-aorg[i])>0 )
                            {
                                cvg = false;
                                break;
                            }
                        }

                        if(cvg)
                        {
                            goto EXTREMUM;
                        }

                        //______________________________________________________
                        //
                        // forward and check
                        //______________________________________________________
                        const real_t D2_try = sample.computeD2(F,atry);
                        if(D2_try<D2_org)
                        {
                            if(correct)
                            {
                                static const real_t phi  = numeric<real_t>::gold;
                                static const real_t phi2 = REAL(1.0)+phi;

                                static const real_t up   = phi;
                                static const real_t lo   = phi-1;

                                const real_t H0  = D2_org; // @0
                                const real_t H1  = D2_try; // @1
                                const real_t Hp  = eval1d(phi);
                                const real_t D1  = (H1-H0);
                                const real_t Dp  = (Hp-H0);
                                const real_t a   = (phi2* D1 -  Dp);
                                const real_t b   = (Dp - phi * D1);

                                ios::wcstream fp("H.dat");
                                for(double u=0;u<=phi;u+=0.02)
                                {
                                    fp("%g %g %g\n", u, eval1d(u), H0 +a*u+b*u*u);
                                }

                                if(a<0&&b>0)
                                {
                                    const real_t num    = -a;
                                    const real_t den    = b+b;
                                    const real_t opt    = (num <= den*lo) ? lo : ( (num>=den*up) ? up : num/den );
                                    assert(opt>=lo);assert(opt<=up);
                                    const real_t D2_opt = eval1d(opt);
                                    if(D2_opt<D2_try)
                                    {
                                        std::cerr << "take new min @" << opt << std::endl;
                                        tao::set(atry,atmp);
                                        //if(cycle>=2) exit(1);
                                    }
                                }
                            }


                            // set aorg to trial value
                            tao::set(aorg,atry);
                            --p10;
                            goto CYCLE;
                        }
                        else
                        {
                            ++p10;
                            goto BACKTRACK;
                        }
                    }
                }

            EXTREMUM:
                {
                    //__________________________________________________________
                    //
                    // compute covariance matrix
                    //__________________________________________________________
                    const real_t D2 = sample.computeD2(F,aorg,*this);
                    __regularize(curv, beta, used);
                    if( !LU<real_t>::build(curv) )
                    {
                        // singular matrix at extremum
                        if(verbose)
                        {
                            std::cerr << "fit.singular curvature" << std::endl;
                        }
                        return false;
                    }
                    CALLBACK();
                    Matrix &alpha = cinv;
                    alpha.ld1();
                    LU<real_t>::solve(curv,alpha);
                    
                    //__________________________________________________________
                    //
                    // counting variables
                    //__________________________________________________________
                    size_t user_nvar = nvar;
                    for(size_t i=nvar;i>0;--i)
                    {
                        if(!used[i]) --user_nvar;
                    }

                    //__________________________________________________________
                    //
                    // counting data
                    //__________________________________________________________
                    const size_t user_ndat = sample.items();
                    if(user_nvar>=user_ndat)
                    {
                        if(verbose)
                        {
                            std::cerr << "fit.not enough degrees of freedon" << std::endl;
                        }
                        return false;
                    }

                    //__________________________________________________________
                    //
                    // degrees of freedom
                    //__________________________________________________________
                    const size_t  dof = (user_ndat-user_nvar);

                    //__________________________________________________________
                    //
                    // compute the variances..
                    //__________________________________________________________
                    const real_t dS = D2/dof;
                    for(size_t i=nvar;i>0;--i)
                    {
                        if( used[i] )
                        {
                            aerr[i] = Sqrt( Fabs(alpha[i][i]) * dS );
                        }
                    }

                    //__________________________________________________________
                    //
                    // compute the determination coefficient
                    //__________________________________________________________
                    if(user_ndat>1)
                    {
                        const real_t SSres = D2;
                        const real_t SStot = sample.SStot();

                        Rsq = REAL(1.0) - (SSres/dof)/(SStot/(user_ndat-1));
                    }
                }

                return true;
            }


        }
    }
}




