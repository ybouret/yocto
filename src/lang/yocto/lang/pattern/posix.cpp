#include "yocto/lang/pattern/posix.hpp"
#include "yocto/lang/pattern/basic.hpp"
#include "yocto/lang/pattern/logic.hpp"
#include "yocto/ptr/auto.hpp"

namespace yocto
{
    namespace Lang
    {
        Pattern * posix::upper()
        {
            return new Range('A','Z');
        }

        Pattern * posix::lower()
        {
            return new Range('a','z');
        }

        Pattern * posix::digit()
        {
            return new Range('0','9');
        }

        Pattern *posix::alpha()
        {
            auto_ptr<OR> p( new OR() );
            *p << lower();
            *p << upper();
            return p.yield();
        }

        Pattern *posix::alnum()
        {
            auto_ptr<OR> p( new OR() );
            *p << lower();
            *p << upper();
            *p << digit();
            return p.yield();
        }

        Pattern *posix::xdigit()
        {
            auto_ptr<OR> p( new OR() );
            *p << new Range('0','9');
            *p << new Range('a','f');
            *p << new Range('A','F');
            return p.yield();
        }

        Pattern * posix::blank()
        {
            auto_ptr<OR> p( new OR() );
            *p << new Single(' ');
            *p << new Single('\t');
            return p.yield();
        }

        Pattern * posix::space()
        {
            auto_ptr<OR> p( new OR() );
            
            *p << new Single(' ');
            *p << new Single('\t');
            *p << new Single('\n');
            *p << new Single('\r');
            *p << new Single('\v');
            *p << new Single('\f');

            return p.yield();
        }

        Pattern * posix:: punct()
        {
            return Logical::Among("!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~");
        }

        Pattern * posix:: word()
        {
            auto_ptr<OR> p( new OR() );
            *p << lower();
            *p << upper();
            *p << digit();
            *p << new Single('_');
            return p.yield();
        }

        Pattern * posix:: endl()
        {
            auto_ptr<Logical> p( new OR() );
            *p << new Single('\n');
            *p << new Single('\r');
            *p << Logical::Equal("\r\n");
            return p.yield();
        }

        Pattern * posix:: dot()
        {
            auto_ptr<Logical> p( new NONE() );
            *p << new Single('\n');
            *p << new Single('\r');
            return p.yield();
        }
        
    }
}

