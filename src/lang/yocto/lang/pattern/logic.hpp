#ifndef YOCTO_LANG_PATTERN_LOGIC_INCLUDED
#define YOCTO_LANG_PATTERN_LOGIC_INCLUDED 1

#include "yocto/lang/pattern.hpp"

namespace yocto
{
    namespace Lang
    {
        //! base class for logical operation
        class Logical : public Pattern
        {
        public:
            virtual ~Logical() throw();
            Patterns operands;

            Logical & operator<<( Pattern *);

            static Logical *Among(const string &chars);
            static Logical *Among(const char   *chars);
            static Logical *Equal(const string &chars);
            static Logical *Equal(const char   *chars);

        protected:
            explicit Logical(const uint32_t t) throw();
            Logical(const Logical &other);

            void __viz_ops( ios::ostream &fp ) const;
            void __sav_ops( ios::ostream &fp ) const;

        private:
            YOCTO_DISABLE_ASSIGN(Logical);
        };
    }
}

namespace yocto
{
    namespace Lang
    {
        class AND : public Logical
        {
        public:
            YOCTO_LANG_PATTERN_DECL(AND, ' ', '&', '&', ' ');
            explicit AND() throw();

        private:
            AND(const AND &);
        };
    }
}

namespace yocto
{
    namespace Lang
    {
        class OR : public Logical
        {
        public:
            YOCTO_LANG_PATTERN_DECL(OR, ' ', '|', '|', ' ');
            explicit OR() throw();

        private:
            OR(const OR &);
        };
    }
}

namespace yocto
{
    namespace Lang
    {
        class NONE : public Logical
        {
        public:
            YOCTO_LANG_PATTERN_DECL(NONE, ' ', '!', '=', ' ');
            explicit NONE() throw();

        private:
            NONE(const NONE &);
        };
    }
}



#endif
