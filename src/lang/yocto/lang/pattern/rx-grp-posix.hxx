//==============================================================================
//
//
// extract a posix sub-expression
//
//
//==============================================================================
Pattern * findPosixExpression()
{
    assert(':'==*curr);
    const char *ini = ++curr; // skip ':'
    while(true)
    {
        if(curr>=last) throw exception("%sunfinished posix expression",fn);
        if(curr[0]==':')
        {
            if(curr[1]!=Y_RBRACK) throw exception("%sexpecting ':%c'",fn,Y_RBRACK);
            break;
        }
        ++curr;
    }
    assert(':'      == curr[0]);
    assert(Y_RBRACK == curr[1]);
    const char *end = curr;
    ++curr;
    ++curr;

    const size_t n = static_cast<size_t>(end-ini);
    if(n<=0) throw exception("%sempty posix expression",fn);

    const string id(ini,n);
#define Y_LANG_POSIX(NAME) if(#NAME==id) return posix::NAME()

    Y_LANG_POSIX(upper);
    Y_LANG_POSIX(lower);
    Y_LANG_POSIX(digit);
    Y_LANG_POSIX(alpha);
    Y_LANG_POSIX(alnum);
    Y_LANG_POSIX(xdigit);
    Y_LANG_POSIX(blank);
    Y_LANG_POSIX(space);
    Y_LANG_POSIX(punct);

    Y_LANG_POSIX(word);
    Y_LANG_POSIX(endl);
    Y_LANG_POSIX(dot);
    Y_LANG_POSIX(any1);
    
    throw exception("%sunknown posix :%s:",fn,id.c_str());
}
