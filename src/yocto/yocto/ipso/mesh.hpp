#ifndef YOCTO_IPSO_MESH_INCLUDED
#define YOCTO_IPSO_MESH_INCLUDED 1

#include "yocto/ipso/subset.hpp"

namespace yocto
{
    namespace ipso
    {
        //! base class to handle axis names
        typedef vector<string,memory::pooled::allocator> _axis_names;

        class axis_names : public _axis_names
        {
        public:
            explicit axis_names();
            virtual ~axis_names() throw();
            axis_names(const axis_names &other);

            explicit axis_names( const char **names, const size_t count );
            explicit axis_names( const char *comma_separated_fields );
        private:
            YOCTO_DISABLE_ASSIGN(axis_names);
        };
        
        //! info on mesh
        class mesh_info : public object
        {
        public:
            const string name;
            const size_t dimension;
            virtual ~mesh_info() throw();
            const field_db & db() const throw();

            field_info       & operator[](size_t dim) throw();
            const field_info & operator[](size_t dim) const throw();
            
            static const char * vtk_coordinates(const size_t dim) throw();


        protected:
            explicit mesh_info(const char   *id,const size_t d);
            explicit mesh_info(const string &id,const size_t d);
            field_db axis_db;   //!< axis fields database
            fields   axis_info; //!< axis fields info

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(mesh_info);
        };

        

    }
}

#endif

