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
    std::cerr << "processing {" << info << "}" << std::endl;
    
    const char C = info[0];
    if( PatternDict::IsValidFirstChar(C) )
    {
        // assuming reference
        if(!dict) throw exception("%smissing dictionary for '%s'", fn, info.c_str() );
        ops.push_back( (*dict)[info] );
    }
    else
    {
        
        throw exception("%snot implemented yet",fn);

    }

}
