//==================================================================
//
//
// create a group and add it to operands
//
//
//==================================================================
inline Pattern *nextGroupSingle()
{
    return 0;
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
        std::cerr << "POSIX" << std::endl;
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

                exit(1);
            }
                break;

            default:
                grp->operands.push_back( new Single(C) );
                ++curr;
        }


    }

END_GRP:
    ops.push_back( Pattern::Optimize(grp.yield()) );

}


