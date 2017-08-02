//==================================================================
//
//
// create a group and add it to operands
//
//
//==================================================================
Pattern *nextGroupPattern()
{
    return 0;
}

void createGroup( Patterns &ops )
{
    assert(Y_LBRACK==*curr);
    if(++curr>last) throw exception("%sunfinished group",fn);


    // here is the first char of the group
    char C = *curr;

    // detect a posix reference
    if(':'==C)
    {
        std::cerr << "POSIX" << std::endl;
        ops.push_back(findPosixExpression());
        return;
    }

    auto_ptr<Logical> grp;
    switch(C)
    {
        default:
            grp.reset( new OR() );
    }

    while(true)
    {
        if(curr>=last)
        {
            throw exception("%sunfinished group",fn);
        }

        C = *curr;
        switch(C)
        {

            case Y_LBRACK:
                createGroup(grp->operands);
                break;

            case Y_RBRACK:
                ++curr;
                goto END_GRP;

            default:
                grp->operands.push_back( new Single(C) );
                ++curr;
        }


    }

END_GRP:
    ops.push_back( grp.yield() );

}
