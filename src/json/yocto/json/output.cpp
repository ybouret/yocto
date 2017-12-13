
#include "yocto/json/value.hpp"
#include "yocto/code/utils.hpp"

namespace yocto
{
    namespace JSON
    {
        static inline void __indent( ios::ostream &fp, const size_t blanks ) throw()
        {
            for(size_t i=1;i<=blanks;++i)
            {
                fp.write(' ');
            }

        }

        static inline void
        __output_string( ios::ostream &fp, const String &s )
        {
            fp.write('\"');
            for(size_t i=0;i<s.length();++i)
            {
                const char C = s[i];
                switch(C)
                {
                    default:
                        fp << C;
                }
            }
            fp.write('\"');
        }

        void Value::output(ios::ostream &fp, int blanks) const
        {
            switch(type)
            {
                case IsNull   : fp << "null";  break;
                case IsTrue   : fp << "true";  break;
                case IsFalse  : fp << "false"; break;
                case IsString : __output_string( fp, as<String>() ); break;
                case IsNumber : fp("%.15g",double(as<Number>()));    break;
                case IsArray: {
                    const Array &a = as<Array>();
                    const size_t n = a.length();
                    fp << '[';
                    blanks+=2;
                    for(size_t i=1;i<=n;++i)
                    {
                        if(i>1)
                        {
                            __indent(fp,blanks);
                        }
                        fp.write(' ');a[i].output(fp,blanks);
                        if(i<n)
                        {
                            fp.write(',');
                            fp.write('\n');
                        }
                    }
                    fp << ' ' << ']';
                } break;

                case IsObject: {
                    const Object &o = as<Object>();
                    const size_t  n = o.length();
                    fp << '{' << '\n';
                    size_t kmax = 0;
                    for(Object::const_iterator i=o.begin();i!=o.end();++i)
                    {
                        const Pair &p = *i;
                        kmax = max_of(kmax,p.name.length());
                    }
                    blanks += 2;
                    size_t indx = 1;
                    for(Object::const_iterator i=o.begin();i!=o.end();++i,++indx)
                    {
                        const Pair &p = *i;
                        __indent(fp,blanks);
                        __output_string(fp,p.name);
                        fp << " : ";
                        p.value.output(fp,kmax+6);
                        if(indx<n)
                        {
                            fp << ',';
                        }
                        fp << '\n';
                    }
                    blanks-=2;
                    __indent(fp,blanks);
                    fp << '}';
                } break;
            }
        }
    }

}


