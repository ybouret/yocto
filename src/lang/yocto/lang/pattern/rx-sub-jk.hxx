//==============================================================================
//
//
// create a simple Joker
//
//
//==============================================================================
inline void createSimpleJoker(Patterns &ops, const char C)
{
    assert(ops.size>0);
    switch(C)
    {
        case '+': ops.push_back( OneOrMore(ops.pop_back()));        return;
        case '?': ops.push_back( Optional::Create(ops.pop_back())); return;
        case '*': ops.push_back( ZeroOrMore(ops.pop_back()));       return;
        default : break;
    }
    throw exception("%scorrupted code in createSimpleJoker!",fn);
}

//==============================================================================
//
//
// extract joker from braced expression: reference or counting
//
//
//==============================================================================
static inline bool __is_bad( char C ) throw()
{
    return (' '==C) || ('\t'==C);
}

inline void createBracedJoker( Patterns &ops )
{
    assert(Y_LBRACE== *curr);

    //__________________________________________________________________________
    //
    // check possibility
    //__________________________________________________________________________
    if(ops.size<=0) throw exception("%sno pattern before braces",fn);

    //__________________________________________________________________________
    //
    // extract content
    //__________________________________________________________________________
    const char *ini = ++curr;
    while(*(curr++)!=Y_RBRACE)
    {
        if(curr>=last) throw exception("%smissing '%c'",fn,Y_RBRACE);
    }
    const char *end = curr;
    assert(end>ini);


    string info(ini,static_cast<size_t>(end-ini)-1);
    info.remove(__is_bad);
    if(info.size()<=0) throw exception("%sempty braces",fn);

    const char C = info[0];
    if( PatternDict::IsValidFirstChar(C) )
    {
        // assuming reference
        if(!dict) throw exception("%smissing dictionary for '%s'", fn, info.c_str() );
        ops.push_back( (*dict)[info] );
    }
    else
    {
        char *addr_n = &info[0];
        char *coma   = strchr(addr_n, ',');

        static const char nmsg[] = "braced joker first count";
        static const char mmsg[] = "braced joker second count";

        if(coma)
        {
            *(coma++) = 0;
            const string nstr = addr_n;
            const string mstr = coma;
            const size_t n = strconv::to_size(nstr,nmsg);
            if(mstr.size()>0)
            {
                const size_t m = strconv::to_size(mstr,mmsg);
                ops.push_back( Counting::Create(ops.pop_back(), n, m) );
            }
            else
            {
                ops.push_back( AtLeast::Create(ops.pop_back(), n) );
            }
        }
        else
        {
            const string nstr = addr_n;
            const size_t n = strconv::to_size(nstr,nmsg);
            ops.push_back( Counting::Create(ops.pop_back(), n, n) );
        }



    }

}
