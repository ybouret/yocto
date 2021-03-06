#include "yocto/json/value.hpp"
#include <cstring>

namespace yocto
{
    namespace JSON
    {
#define Y_JSON_IMPL_CONV(TYPE) \
template <> TYPE & Value:: as<TYPE>() throw()\
{\
assert(Is##TYPE==type);\
return *static_cast<TYPE *>(impl);\
}\
template <> const TYPE & Value:: as<TYPE>() const throw()\
{\
assert(Is##TYPE==type);\
return *static_cast<const TYPE *>(impl);\
}
        Y_JSON_IMPL_CONV(String)
        Y_JSON_IMPL_CONV(Number)
        Y_JSON_IMPL_CONV(Array)
        Y_JSON_IMPL_CONV(Object)

        Value:: ~Value() throw()
        {
            switch(type)
            {
                case IsNull:
                case IsTrue:
                case IsFalse:
                    assert(NULL==impl);
                    break;

                case IsNumber:
                    object::release1( (Number *)impl );
                    break;

                case IsString:
                    delete static_cast<String *>(impl);
                    break;

                case IsArray:
                    delete static_cast<Array *>(impl);
                    break;

                case IsObject:
                    delete static_cast<Object *>(impl);
                    break;
                    
            }
            impl         = NULL;
            (Type &)type = IsNull;
        }

        Value:: Value() throw() :
        type(IsNull),
        impl(0)
        {
        }

        Value:: Value(const Type t) :
        type(t),
        impl(0)
        {
            switch(type)
            {
                case IsNull:                          break;
                case IsTrue:  (Type &)type = IsTrue;  break;
                case IsFalse: (Type &)type = IsFalse; break;

                case IsNumber:
                    impl        = object::acquire1<Number>();
                    (Type&)type = IsNumber;
                    break;

                case IsString:
                    impl        = new String();
                    (Type&)type = IsString;
                    break;

                case IsObject:
                    impl        = new Object();
                    (Type&)type = IsObject;
                    break;

                case IsArray:
                    impl = new Array();
                    (Type&)type = IsArray;
                    break;
            }

        }

        Value & Value:: make(const Type t)
        {
            Value tmp(t);
            swap_with(tmp);
            return *this;
        }


        Value:: Value(const String &s) :
        type(IsNull),
        impl( new String(s) )
        {
            (Type&)type = IsString;
        }

        Value:: Value(const Number x) :
        type(IsNull),
        impl( object::acquire1<Number>() )
        {
            (Type&)type = IsNumber;
            as<Number>()  = x;
        }


        void Value:: swap_with(Value &other) throw()
        {
            cswap(impl,other.impl);
            cswap_const(type,other.type);
        }

        void Value:: clear() throw()
        {
            Value tmp;
            swap_with(tmp);
        }


        Value:: Value( const Value &other ) :
        type(other.type),
        impl(0)
        {
            switch(type)
            {
                case IsNull:
                case IsTrue:
                case IsFalse:
                    assert(NULL==impl);
                    break;

                case IsNumber:
                    impl = object::acquire1<Number>();
                    memcpy(impl,other.impl,sizeof(Number));
                    break;

                case IsString:
                    impl = new String( *static_cast<String *>(other.impl) );
                    break;

                case IsArray:
                    impl = new Array( *static_cast<Array *>(other.impl) );
                    break;

                case IsObject:
                    impl = new Object( *static_cast<Object *>(other.impl) );
                    break;
                    
            }
        }

        Value & Value:: operator=(const Value &other)
        {
            Value tmp(other);
            swap_with(tmp);
            return *this;
        }

        size_t Value:: length() const throw()
        {
            switch(type)
            {
                case IsArray:  return static_cast<Array*>(impl)->length();
                case IsObject: return static_cast<Object*>(impl)->length();
                default:
                    break;
            }
            return 1;
        }


        static inline
        string jstr2cstr(const String &jstr )
        {
            string ans = jstr;

            return ans;
        }

        void Value:: display( std::ostream &os, int depth ) const
        {
            switch(type)
            {
                case IsFalse:  os << "false";     break;
                case IsTrue:   os << "true";      break;
                case IsNull:   os << "null";      break;
                case IsNumber: { const string s = vformat("%.15g",as<Number>()); os << s; } break;
                case IsString: { const string s = jstr2cstr(as<String>());      os << '\"' << s << '\"'; } break;
                case IsArray:  as<Array>().display(os,depth);  break;
                case IsObject: as<Object>().display(os,depth); break;
            }
        }



    }
}

#include "yocto/exception.hpp"
namespace yocto
{
    namespace JSON
    {

        Array:: ~Array() throw()
        {
        }

        Array:: Array() throw()
        {
        }


        Array:: Array(const Array &other) : ArrayBase(other) {}

        void Array:: display(std::ostream &os, int depth) const
        {
            os << '[' << std::endl;
            ++depth;
            for(size_t i=1;i<=size();++i)
            {
                for(int j=0;j<depth;++j) os << ' ';
                (*this)[i].display(os,depth);
                if(i<size()) os << ',' << std::endl;
            }
            os << ' ' << ']';
        }

#define Y_JSON_ARRAY_APPEND(TYPE)\
template <> TYPE & Array::append<TYPE>() {\
sequence<Value> &self = *this;\
return self.back().as<TYPE>();\
}\

        Y_JSON_ARRAY_APPEND(String)
        Y_JSON_ARRAY_APPEND(Number)
        Y_JSON_ARRAY_APPEND(Object)
        Y_JSON_ARRAY_APPEND(Array)

        Array & operator<<( Array &a, const Type t)
        {
            const Value v(t);
            a.push_back(v);
            return a;
        }


    }
}

namespace yocto
{
    namespace JSON
    {

        Pair:: ~Pair() throw()
        {
        }

        Pair:: Pair(const string &n) :
        name(n),
        value()
        {
        }

        Pair:: Pair(const string &n, const Value &v) :
        name(n),
        value(v)
        {
        }


        Pair:: Pair(const Pair &p) :
        name(p.name),
        value(p.value)
        {
        }

        const string & Pair:: key() const throw() { return name; }


        Object:: Object() throw()
        {
        }

        Object:: ~Object() throw()
        {
        }

        Object:: Object(const Object &other) :
        ObjectBase(other)
        {
        }

        Value & Object:: operator[](const string &name)
        {

            Pair *p = search(name);
            if(p)
            {
                return p->value;
            }
            else
            {
                const Pair P(name);
                if(!insert(P))
                {
                    throw exception("JSON.Object['%s']: unexpected failure",*name);
                }
                p = search(name);
                if(!p)
                {
                    throw exception("JSON.Object.search('%s'): unexpected failure",*name);
                }
                return p->value;
            }
        }

        const Value & Object:: operator[](const string &name) const
        {
            const Pair *p = search(name);
            if(!p)
            {
                throw exception("no JSON.Object['%s']", *name);
            }
            return p->value;
        }

        void Object:: display( std::ostream &os, int depth ) const
        {
            os << '{' << std::endl;
            const Object::const_iterator theEnd = end();
            ++depth;
            for( Object::const_iterator i = begin(); i != theEnd; ++i )
            {
                for(int j=0;j<depth;++j) os << ' ';
                const Pair  &p    = *i;
                const string name = jstr2cstr(p.name);
                os << '\"' << name << '\"' << ':' << std::endl;
                ++depth;
                for(int j=0;j<depth;++j) os << ' ';
                p.value.display(os,depth);
                --depth;
                if(i!=theEnd) os << ',' << std::endl;
            }
            os << ' ' << '}';
        }


    }
}

