#ifndef YOCTO_ASSOCIATIVE_DB_ENTRY
#define YOCTO_ASSOCIATIVE_DB_ENTRY 1

#include "yocto/string.hpp"
#include "yocto/counted-object.hpp"

namespace yocto
{
    //! holds a label
    class db_label : public counted_object
    {
    public:
        const string name;
        const string & key() const throw();

        explicit db_label(const string &id);
        explicit db_label(const char   *id);

        db_label(const db_label &);
        virtual ~db_label() throw();

    private:
        YOCTO_DISABLE_ASSIGN(db_label);
    };

    //! fast type for map of set
    template <typename T>
    class db_entry : public db_label
    {
    public:
        YOCTO_ARGUMENTS_DECL_T;

        type value;

        inline virtual ~db_entry() throw() {}
        inline explicit db_entry(const string &id, param_type arg) : db_label(id), value(arg) {}
        inline db_entry(const db_entry &other) : db_label(other), value(other.value) {}

        inline db_entry & operator=( param_type x )
        {
            value = x;
            return *this;
        }

    private:
        YOCTO_DISABLE_ASSIGN(db_entry);
    };

}

#endif

