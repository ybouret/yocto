
#include "yocto/json/value.hpp"

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
            for(size_t i=0;i<s.length();++i)
            {
                const char C = s[i];
                switch(C)
                {
                    default:
                        fp << C;
                }
            }
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
                        //fp("(%d)", blanks);
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
            }
        }
    }

}


