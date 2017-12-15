#include "yocto/lang/syntax/grammar.hpp"
#include "yocto/hashing/sha1.hpp"
#include "yocto/memory/buffers.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {



            static const char __mark_terminal = 0;
            static const char __mark_internal = 1;

            void Node::runHash( hashing::function &H ) const
            {
                H(origin.label);
                if(terminal)
                {
                    H(&__mark_terminal,1);
                    const Lexeme   *lex = (const Lexeme *)impl;
                    for(const Char *ch  = lex->head;ch;ch=ch->next)
                    {
                        H(&(ch->code),1);
                    }
                }
                else
                {
                    H(&__mark_internal,1);
                    const List *children = (const List *)impl;
                    for(const Node *sub = children->head;sub;sub=sub->next)
                    {
                        sub->runHash(H);
                    }
                }
            }

            string Node:: signature() const
            {
                hashing::sha1 H;
                H.set();
                runHash(H);
                memory::buffer_of<uint8_t,memory::global> s(H.length);
                assert(s.length()==H.length);
                H.get(s.rw(),s.length());
                string ans(H.length*2,as_capacity);
                for(size_t i=0;i<s.length();++i)
                {
                    const uint8_t b = s[i];
                    if( b > 32 && b < 127 )
                    {
                        ans += char(b);
                    }
                    else
                    {
                        ans += hexa_text_lower[ s[i] ];
                    }
                }
                return ans;
            }

            void Node:: serialize(ios::ostream &fp) const
            {
                // write the label
                const string &label = origin.label;
                fp.emit<uint32_t>(label.size()) << label;

                if(terminal)
                {
                    // set terminal
                    fp.write(__mark_terminal);

                    // write the content
                    const Lexeme *lex = (const Lexeme *)impl;
                    fp.emit<uint32_t>(lex->size);
                    for(const Char *ch = lex->head;ch;ch=ch->next)
                    {
                        fp.write(ch->code);
                    }
                }
                else
                {
                    // set internal
                    fp.write(__mark_internal);

                    // write the children
                    const List *children = (const List *)impl;
                    fp.emit<uint32_t>(children->size);
                    for(const Node *sub = children->head;sub;sub=sub->next)
                    {
                        sub->serialize(fp);
                    }
                }
            }

            Node * Node:: loadFrom( Source &S, const Grammar &G )
            {
                static const char fn[] = "Syntax::Node::loadFrom";
                ios::istream     &fp   = (ios::istream &)(*(S.module->input));

                // read label
                const size_t sz = fp.read<uint32_t>("label size");
                if(sz<=0) throw exception("%s:invalid negative label size",fn);
                string label(sz,as_capacity);
                for(size_t i=0;i<sz;++i)
                {
                    label += fp.read_char("label");
                }


                // find origin rule
                const Rule &r = G.getRuleByLabel(label);

                // read type
                const int t = fp.read<uint8_t>("node type");
                //std::cerr << "found node type=" << t <<  std::endl;

                // act accordingly
                switch(t)
                {
                    case __mark_terminal:
                    {
                        const size_t     nch = fp.read<uint32_t>("lexeme size");
                        auto_ptr<Lexeme> lxm( new Lexeme(S.stamp()) );
                        for(size_t i=0;i<nch;++i)
                        {
                            const uint8_t code = fp.read_byte("lexeme code");
                            Char *ch = new Char( *(S.module), code);
                            lxm->push_back(ch);
                        }
                        return Node::Create(S,r,lxm.yield());
                    }

                    case __mark_internal:
                    {
                        //std::cerr << "new internal" << std::endl;
                        const size_t nch = fp.read<uint32_t>("children");
                        auto_ptr<Node> node( Node::Create(S,r) );
                        Node::List    &children = node->toList();
                        for(size_t i=0;i<nch;++i)
                        {
                            children.push_back( loadFrom(S,G) );
                        }
                        return node.yield();
                    }

                    default:
                        break;
                }

                throw exception("%s: invalid node type=%d",fn,t);
            }

        }
    }
}
