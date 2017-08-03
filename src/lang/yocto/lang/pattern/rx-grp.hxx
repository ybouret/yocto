//==============================================================================
//
//
// create a group and add it to operands
//
//
//==============================================================================


static inline int cmpChars( const int lhs, const int rhs ) throw()
{
    return lhs-rhs;
}

inline Pattern *nextGroupSingle()
{
    static const char plain[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ^_`abcdefghijklmnopqrstuvwxyz|~";

    if(curr>=last) throw exception("%sunfinished range",fn);
    const char C = *curr;
    size_t     i = 0;
    if( core::locate(&C, plain, sizeof(plain)/sizeof(plain[0])-1, i, cmpChars) )
    {
        ++curr;
        return new Single(C);
    }
    else
    {
        if( C == '\\' )
        {
            Patterns tmp;
            ++curr; // skipping escape
            grpEscapeSequence(tmp);
            return tmp.pop_back();
        }
        else
        {
            throw exception("%sexpecting a Single for range",fn);
            return 0;
        }
    }
}

inline void createGroup( Patterns &ops )
{
    assert(Y_LBRACK==*curr);
    if(++curr>last) throw exception("%sunfinished group",fn);


    //__________________________________________________________________________
    //
    // here is the first char of the group
    //__________________________________________________________________________
    char C = *curr;

    //__________________________________________________________________________
    //
    // detect a posix reference
    //__________________________________________________________________________
    if(':'==C)
    {
        ops.push_back(findPosixExpression());
        return;
    }

    //__________________________________________________________________________
    //
    // create a group
    //__________________________________________________________________________
    auto_ptr<Logical> grp;
    switch(C)
    {
        case '^':
            grp.reset( new NONE() );
            ++curr;
            break;

        case '-':
            grp.reset( new OR() );
            *grp << new Single('-');
            ++curr;
            break;

        default:
            grp.reset( new OR() );
    }

    //__________________________________________________________________________
    //
    // the parse the following
    //__________________________________________________________________________
    while(true)
    {
        if(curr>=last)
        {
            throw exception("%sunfinished group",fn);
        }

        C = *curr;
        switch(C)
        {

                //______________________________________________________________
                //           //
            case Y_LBRACK:  // recursive call
                //_________//___________________________________________________
                createGroup(grp->operands);
                break;

                //______________________________________________________________
                //           //
            case Y_RBRACK:  // end it
                //_________//___________________________________________________
                ++curr;
                goto END_GRP;

                //______________________________________________________________
                //        //
            case '\\':   // escape sequence
                //______//______________________________________________________
                ++curr;// skip backspace
                grpEscapeSequence(grp->operands);
                break;

                //______________________________________________________________
                //      //
            case '-':  // make a range
                //____//_________________________________________________________
            {
                // check previous pattern
                if(grp->operands.size<=0)                  throw exception("%sno previous pattern for range",fn);
                if(grp->operands.tail->uuid!=Single::UUID) throw exception("%sprevious pattern for range is not a Single",fn);

                // get next pattern: skip '-'
                ++curr;

                auto_ptr<Pattern> rhs( nextGroupSingle()        );
                auto_ptr<Pattern> lhs( grp->operands.pop_back() );

                assert(rhs->uuid==Single::UUID);
                assert(lhs->uuid==Single::UUID);
                *grp << new Range(static_cast<Single *>(lhs->addr)->code,static_cast<Single *>(rhs->addr)->code);

            } break;

            default:
                grp->operands.push_back( new Single(C) );
                ++curr;
        }


    }

END_GRP:
    ops.push_back( grp.yield() );

}


