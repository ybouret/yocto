#ifndef YOCTO_LANG_SYNTAX_AGGREGATE_INCLUDED
#define YOCTO_LANG_SYNTAX_AGGREGATE_INCLUDED 1

#include "yocto/lang/syntax/compound.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            //! create an internal aggregate rule
            /**
             Remember that is the label containt the FusionMark char, 
             the whole content will be merged with parent's content.
             */
            class Aggregate : public Compound
            {
            public:
                static const uint32_t UUID = YOCTO_FOURCC(' ','&','&',' ');
                virtual ~Aggregate() throw();
                explicit Aggregate(const string &);
                explicit Aggregate(const char   *);

                
                YOCTO_LANG_SYNTAX_RULE_DECL();

                size_t getMinCount() const throw(); //!< number of non-admitting-empty members

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Aggregate);
            };
            
        }
    }
}

#endif
