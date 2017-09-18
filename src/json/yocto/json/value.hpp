#ifndef YOCTO_JSON_VALUE_INCLUDED
#define YOCTO_JSON_VALUE_INCLUDED 1

#include "yocto/string.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/associative/set.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/lang/source.hpp"

namespace yocto
{
    namespace JSON
    {


        typedef string String;
        typedef double Number;
        class          Object;
        class          Array;
        class          Pair;


#define YOCTO_JSON_VALUE_TO(CLASS) \
inline CLASS       & to##CLASS() throw()       { assert(Is##CLASS==type); return *static_cast<CLASS *>(impl);       }\
inline const CLASS & to##CLASS() const throw() { assert(Is##CLASS==type); return *static_cast<const CLASS *>(impl); }

#define YOCTO_JSON_VALUE_CHECK(TYPE) inline bool is##TYPE() const throw() { return Is##TYPE == type; }

        class Value
        {
        public:
            enum Type
            {
                IsString,
                IsNumber,
                IsObject,
                IsArray,
                IsTrue,
                IsFalse,
                IsNull
            };

            Value() throw(); // IsNull
            Value(const Type t);
            ~Value() throw();
            Value(const Value &other);
            Value & operator=( const Value &other);
            
            void   swap_with(Value &other) throw();
            size_t length() const throw();
            void   clear() throw();


            YOCTO_JSON_VALUE_TO(Number)
            YOCTO_JSON_VALUE_TO(String)
            YOCTO_JSON_VALUE_TO(Array)
            YOCTO_JSON_VALUE_TO(Object)

            YOCTO_JSON_VALUE_CHECK(String)
            YOCTO_JSON_VALUE_CHECK(Number)
            YOCTO_JSON_VALUE_CHECK(Object)
            YOCTO_JSON_VALUE_CHECK(Array)
            YOCTO_JSON_VALUE_CHECK(True)
            YOCTO_JSON_VALUE_CHECK(False)
            YOCTO_JSON_VALUE_CHECK(Null)

            const Type type;

            static Value & LoadFrom( Lang::Source & );

            Value(const String &);
            Value(const Number  );

            inline friend
            std::ostream & operator<<( std::ostream &os, const Value &v )
            {
                v.display(os,0);
                return os;
            }
            
            void display( std::ostream &, int depth ) const;

        private:
            void      *impl;

        };

        typedef vector<Value> ArrayBase;

        class Array : public ArrayBase
        {
        public:
            explicit Array() throw();
            virtual ~Array() throw();
            Array(const Array &);
            inline size_t length() const throw() { return size(); }

            void display( std::ostream &, int depth ) const;

        private:
            YOCTO_DISABLE_ASSIGN(Array);
        };

        class Pair
        {
        public:
            const String name;
            Value        value;
            const string &key() const throw();

            Pair(const string &n);
            Pair(const string &n, const Value &v);
            ~Pair() throw();
            Pair(const Pair &other);

        private:
            YOCTO_DISABLE_ASSIGN(Pair);
        };


        typedef set<string,Pair> ObjectBase;

        class Object : public ObjectBase
        {
        public:
            explicit Object() throw();
            virtual ~Object() throw();
            Object(const Object &);

            inline size_t length() const throw() { return size(); }

            Value       & operator[](const string &);
            const Value & operator[](const string &) const;

            void display( std::ostream &, int depth ) const;


        private:
            YOCTO_DISABLE_ASSIGN(Object);
        };
    }
}


#endif
