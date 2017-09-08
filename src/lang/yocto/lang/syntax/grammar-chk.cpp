#include "yocto/lang/syntax/grammar.hpp"
#include "yocto/lang/syntax/rules.hpp"
#include "yocto/exception.hpp"
#include "yocto/associative/map.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {
            namespace
            {
                typedef map<string,int> RuleDB;


                static inline
                void updateDB(RuleDB     &db,
                              const Rule *rule,
                              const char *gram )
                {
                    assert(rule);
                    assert(gram);
                    assert(rule->self_);
                    //std::cerr << "...update " << rule->label << std::endl;
                    int *pCount = db.search(rule->label);
                    if(!pCount)
                    {
                        throw exception("{%s}: unregistered rule '%s'!!!", gram, rule->label.c_str());
                    }
                    int &count = *pCount;
                    if(count>0) return; // already visited
                    ++count;
                    Compound *C = 0;
                    switch(rule->uuid)
                    {
                        case Optional::UUID:
                            updateDB(db,static_cast<Optional *>(rule->self_)->content(),gram);
                            return;

                        case Aggregate::UUID:
                            C = static_cast<Aggregate *>(rule->self_);
                            goto UPDATE_COMPOUND;

                        case Alternate::UUID:
                            C = static_cast<Alternate *>(rule->self_);
                            goto UPDATE_COMPOUND;

                        case Counting::UUID:
                            updateDB(db,static_cast<Counting *>(rule->self_)->content(),gram);
                            return;
                            
                        case Terminal::UUID:
                            return;

                        default: throw exception("{%s}: unhandled UUID=x%08x for rule '%s'",gram, unsigned(rule->uuid), rule->label.c_str() );
                    }

                UPDATE_COMPOUND:
                    assert(C!=NULL);
                    for(const MetaNode *m = C->members.head;m;m=m->next)
                    {
                        updateDB(db,m->addr,gram);
                    }
                    return;
                }
            }

            void Grammar:: check_connectivity() const
            {
                const char *gram = name.c_str();
                //______________________________________________________________
                //
                // checking connectivity
                //______________________________________________________________
                if(rules.size<=0)             throw exception("{%s}: no rules",gram);


                //______________________________________________________________
                //
                // checking consistency
                //______________________________________________________________
                for(const Rule *rule = rules.head;rule;rule=rule->next)
                {

                    const uint32_t uuid = rule->uuid;
                    if((Aggregate::UUID==uuid) ||
                       (Alternate::UUID==uuid) )
                    {
                        if(false&&rule->admitsEmpty())
                        {
                            throw exception("{%s}: rule '%s' admits empty content!", gram, rule->id() );
                        }
                    }
                }


                //______________________________________________________________
                //
                // checking connectivity
                //______________________________________________________________
                {
                    RuleDB db(rules.size,as_capacity);
                    for(const Rule *rule = rules.head;rule;rule=rule->next)
                    {
                        //std::cerr << "...registering " << rule->label << std::endl;
                        if( ! db.insert(rule->label,0) )
                        {
                            throw exception("{%s}: unexpected failure to register '%s'", gram, rule->id());
                        }
                    }
                    updateDB(db,rules.head,gram);
                    for(const Rule *rule = rules.head;rule;rule=rule->next)
                    {
                        const int *pCount = db.search(rule->label);
                        if(!pCount) throw exception("{%s}: unexpected removed '%s'", gram, rule->id());
                        //std::cerr << rule->label << " -> " << *pCount << std::endl;
                        if(1 != *pCount)
                            throw exception("{%s}: disconnected rule '%s'",gram, rule->id());
                    }
                }
            }

        }
    }
}
