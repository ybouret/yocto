
#include "yocto/ipso/mesh.hpp"
#include "yocto/string/tokenizer.hpp"

namespace yocto
{
    namespace ipso
    {

        axis_names:: ~axis_names() throw()
        {
        }

        axis_names:: axis_names() :
        _axis_names(3,as_capacity)
        {
        }

        axis_names:: axis_names( const axis_names &other ) :
        _axis_names(other)
        {
        }

        axis_names:: axis_names(const char **names,
                                const size_t count ) :
        _axis_names(count,as_capacity)
        {
            assert(names);
            for(size_t i=0;i<count;++i)
            {
                const string id = names[i];
                push_back(id);
            }
        }

        static inline bool __is_comma(const char C) throw() { return ',' == C; }

        axis_names:: axis_names( const char *comma_separated_fields ) :
        _axis_names(3,as_capacity)
        {
            const string s = comma_separated_fields;
            tokenizer::split(*this,s,__is_comma);
        }

    }

}


namespace yocto
{
    namespace ipso
    {
        mesh_info:: ~mesh_info() throw() {}

        mesh_info:: mesh_info(const size_t d) throw() :
        dimension(d),
        axis_db(d),
        axis_info(d)
        {
            assert(d>=1);assert(d<=3);
        }

        const field_db & mesh_info:: db() const throw()
        {
            assert(axis_db.size()==dimension);
            return axis_db;
        }

        field_info & mesh_info:: operator[](size_t dim) throw()
        {
            assert(dim<dimension);
            assert(dimension==axis_info.size());
            return *axis_info[++dim];
        }

        const field_info & mesh_info:: operator[](size_t dim) const throw()
        {
            assert(dim<dimension);
            assert(dimension==axis_info.size());
            return *axis_info[++dim];
        }

        const char * mesh_info:: vtk_coordinates(const size_t dim) throw()
        {
            static const char *vtk_id[3] =
            {
                "X_COORDINATES",
                "Y_COORDINATES",
                "Z_COORDINATES"
            };
            return vtk_id[dim&3];
        }

    }

}


