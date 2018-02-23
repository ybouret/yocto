
#include "yocto/associative/db-entry.hpp"

namespace yocto
{
    db_label:: ~db_label() throw()
    {
    }

    db_label:: db_label( const string &id ) : name(id) {}
    db_label:: db_label( const char   *id ) : name(id) {}

    db_label:: db_label(const db_label &other) : name(other.name) {}

    const string & db_label :: key() const throw() { return name; }
    

}


