//==================================================================
//
//
// parse an escape sequence from sub-expression
//
//
//==================================================================
inline void subEscapeSequence(Patterns &ops)
{
    assert('\\'==*curr);
    if(++curr>=last) throw exception("%sunfinished escape sequence",fn);
    const char E = curr[0];
    ++curr; // skip escape char

    switch(E)
    {
        case '.':
        case '\\':
        case '/':
        case '{':
        case '}':
        case '(':
        case ')':
        case '[':
        case ']':
        case '"':
        case '\'':
        case '+':
        case '*':
        case '?':
            ops.push_back( new Single(E) );
            return;

        case 'n': ops.push_back( new Single('\n') ); return;
        case 'r': ops.push_back( new Single('\r') ); return;
        case 't': ops.push_back( new Single('\t') ); return;
        case 'f': ops.push_back( new Single('\f') ); return;
        case 'v': ops.push_back( new Single('\v') ); return;

        case 'x':
            ops.push_back( hexEscapeSequence() );
            return;

        default:
            break;
    }

    throw exception("%sunexpected escape char '%c'",fn,E);

}
