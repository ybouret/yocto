//==============================================================================
//
// compiling an escape sequence for a group
//
//==============================================================================
inline void grpEscapeSequence(Patterns &ops)
{
    if(curr>=last) throw exception("%sunfinished Escape Sequence in group",fn);
    const char E = *(curr++); // get and skip the first escape char
    switch(E)
    {
        case ':':
        case '\\':
        case '/':
        case '^':
        case '-':
        case '[':
        case ']':
        case '{':
        case '}':
        case '"':
        case '\'':
            ops.push_back( new Single(E) );
            return;

            Y_ESC_CTRL();

        case 'x':
            ops.push_back( hexEscapeSequence() );
            return;

        default:
            break;
    }
    throw exception("%sunknown escape char '%c' in group", fn, E);
}

