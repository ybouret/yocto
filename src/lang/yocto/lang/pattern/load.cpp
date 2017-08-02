#include "yocto/lang/pattern/basic.hpp"
#include "yocto/lang/pattern/joker.hpp"
#include "yocto/lang/pattern/logic.hpp"
#include "yocto/exception.hpp"

#include "yocto/ptr/auto.hpp"

namespace yocto
{

    namespace Lang
    {

        static inline void __LoadOperands( Patterns &ops, ios::istream &fp )
        {
            const size_t n = fp.read<uint32_t>("Logical.operands.size");
            for(size_t i=1;i<=n;++i)
            {
                ops.push_back( Pattern::Load(fp) );
            }
        }

        Pattern * Pattern:: Load(ios::istream &fp)
        {

            const uint32_t id = fp.read<uint32_t>("pattern.uuid");
            switch(id)
            {
                    // basic
                case Any1::   UUID: return new Any1();
                case Single:: UUID: return new Single( fp.read<uint8_t>("Single.code") );
                case Range::  UUID:
                {
                    const uint8_t lo = fp.read<uint8_t>("Range.lower");
                    const uint8_t up = fp.read<uint8_t>("Range.upper");
                    return new Range(lo,up);
                }
                    //jokers
                case Optional::UUID: return Optional::Create( Load(fp) );
                case AtLeast:: UUID:
                {
                    const size_t count = fp.read<uint32_t>("AtLeast.count");
                    return AtLeast::Create( Load(fp), count);
                }
                    
                case Counting:: UUID:
                {
                    const size_t min_count = fp.read<uint32_t>("Counting.min_count");
                    const size_t max_count = fp.read<uint32_t>("Counting.max_count");
                    return Counting::Create(Load(fp),min_count,max_count);
                }

                    // logical
                case AND::UUID: {
                    auto_ptr<Logical> p( new AND() );
                    __LoadOperands(p->operands,fp);
                    return p.yield();
                }

                case OR::UUID: {
                    auto_ptr<Logical> p( new OR() );
                    __LoadOperands(p->operands,fp);
                    return p.yield();
                }

                case NONE::UUID: {
                    auto_ptr<Logical> p( new NONE() );
                    __LoadOperands(p->operands,fp);
                    return p.yield();
                }

                default:
                    break;
            }
            throw exception("unexpected UUID");
            return 0;
        }

    }

}
