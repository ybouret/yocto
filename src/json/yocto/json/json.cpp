#include "yocto/json/json.hpp"
#include <cstring>

namespace yocto
{
    namespace JSON
    {
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

        void Value:: swap_with(Value &other) throw()
        {
            cswap(impl,other.impl);
            cswap_const(type,other.type);
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

        Value & Object:: insert(const string &name, const Value::Type t)
        {
            const Pair P(name);
        }
        
    }
}

