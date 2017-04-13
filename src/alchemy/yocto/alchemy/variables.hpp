#ifndef YOCTO_ALCHEMY_VARIABLES_INCLUDED
#define YOCTO_ALCHEMY_VARIABLES_INCLUDED 1

#include "yocto/alchemy/library.hpp"
#include "yocto/associative/set.hpp"

namespace yocto
{
    namespace alchemy
    {
        class variable : public counted_object
        {
        public:
            typedef intr_ptr<string,variable> ptr;
            typedef set<string,ptr>           db;

            const size_t indx;
            const string name;
            const string load;

            variable(const size_t iv,
                     const char  *id,
                     const char  *ld);

            variable(const size_t iv,
                     const string &id,
                     const string &ld);

            ~variable() throw();

            const string & key() const throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(variable);
        };

        class variables
        {
        public:

            typedef variable::db::const_iterator iterator;
            virtual ~variables() throw();

            explicit variables(const library::pointer &a_lib,
                               const char             *names[],
                               const char             *loads[],
                               const size_t            num);

            explicit variables(const library::pointer &a_lib,
                               const array<string>    &names,
                               const array<string>    &loads);

            iterator begin() const throw();
            iterator end()   const throw();

            size_t operator[](const string &) const;
            size_t operator[](const char   *) const;

            const library::pointer &shlib;
            const size_t            count; //!< lib.size() + db.size()
            const size_t            extra;

            friend  std::ostream & operator<<( std::ostream &os, const variables &);

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(variables);
            variable::db db;
            void create_variable(const string &name, const string &load);
        };


    }
}

#endif
