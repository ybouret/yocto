#ifndef YOCTO_OCL_TYPES_INCLUDED
#define YOCTO_OCL_TYPES_INCLUDED 1


#include "yocto/string.hpp"
#include "yocto/exception.hpp"
#include "yocto/memory/buffers.hpp"
#include "yocto/memory/pooled.hpp"

#if defined(YOCTO_APPLE)
#       include <OpenCL/cl.h>
#else
#       include <CL/cl.h>
#endif

namespace yocto
{
    namespace ocl
    {
        //! error to string...
        struct Error
        {
            static const char *String(const cl_int err) throw();
        };

    }

}

namespace yocto
{
    namespace ocl
    {

        //! derived exception
        class Exception : public exception
        {
        public:
            const cl_int code;
            explicit Exception(const cl_int err,const char *fmt,...) throw();
            virtual ~Exception() throw();
            Exception(const Exception &other) throw();
            virtual const char *what() const throw();

        private:
            YOCTO_DISABLE_ASSIGN(Exception);
        };

#define YOCTO_OCL_CHECK(error_code,FUNC) do { const cl_int __ocl_err = (error_code); if(CL_SUCCESS!=__ocl_err) throw ocl::Exception(__ocl_err,FUNC); } while(false)

    }

}

namespace yocto
{
    namespace ocl
    {

        //! get info wrapper
        template <
        typename CLASS_ID,
        typename CLASS_INFO>
        struct GetInfo
        {
            template <typename T>
            struct Read
            {
                template <typename CLASS_FUNC> static inline
                T Value(CLASS_ID         id,
                        CLASS_INFO       param_name,
                        CLASS_FUNC       class_func,
                        const char      *class_func_name)
                {
                    T            ans(0);
                    const size_t param_value_size     = sizeof(T);
                    void *       param_value          = &ans;
                    size_t       param_value_ret_size = 0;
                    const size_t err = class_func(id,
                                                  param_name,
                                                  param_value_size,
                                                  param_value,
                                                  &param_value_ret_size);
                    YOCTO_OCL_CHECK(err,class_func_name);
                    return ans;
                }
            };

            template <typename CLASS_FUNC> static inline
            size_t Length(CLASS_ID         id,
                          CLASS_INFO       param_name,
                          CLASS_FUNC       class_func,
                          const char      *class_func_name)
            {
                assert(class_func);
                assert(class_func_name);
                size_t       ans = 0;
                const cl_int err = class_func(id,
                                              param_name,
                                              0,
                                              NULL,
                                              &ans);
                YOCTO_OCL_CHECK(err,class_func_name);
                return ans;
            }

            template <typename CLASS_FUNC> static inline
            string String(CLASS_ID    id,
                          CLASS_INFO  param_name,
                          CLASS_FUNC  class_func,
                          const char *class_func_name)
            {
                assert(class_func);
                assert(class_func_name);
                const size_t len = Length(id,param_name,class_func,class_func_name);
                memory::buffer_of<char,memory::pooled> buf(len);
                const cl_int err = class_func(id,
                                              param_name,
                                              len,
                                              buf.rw(),
                                              NULL);
                YOCTO_OCL_CHECK(err,class_func_name);
                return string((char *)buf.ro(),len);
            }


        };


    }
}


namespace yocto
{
    namespace ocl
    {

        template <typename T>
        class Shared
        {
        public:
            inline virtual ~Shared() throw()
            {
                Release(handle);
                handle=0;
            }

            inline Shared(const Shared &other) : handle( other.handle )
            {
                Retain(handle);
            }

            inline const T operator*() const throw() { return handle; }

        protected:
            explicit Shared(const T h) throw() : handle(h) {}
            T handle;

        private:
            YOCTO_DISABLE_ASSIGN(Shared);
            static void    Retain(const   T) throw(); //!< to be defined
            static void    Release(const  T) throw(); //!< to be defined
            //static cl_uint RefCount(const T) throw(); //!< to be defined
        };

    }

}

#endif
