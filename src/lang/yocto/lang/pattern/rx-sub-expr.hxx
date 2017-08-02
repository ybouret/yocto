inline Pattern *subExpr()
{
    auto_ptr<AND> sub( new AND() );
    Logical      &sxp = *sub;

    while(curr<last)
    {
        const char C = curr[0];
        switch(C)
        {

                //______________________________________________________________
                //
                // grouping
                //______________________________________________________________
            case Y_LPAREN:
                ++curr; // skip left parenthesis
                ++depth;
                sxp << subExpr();
                break;

            case Y_RPAREN:
                if(--depth<0) throw exception("%sunexpected '%c'",fn,Y_RPAREN);
                ++curr; // skip right parenthesis
                goto END_SXP;

                //______________________________________________________________
                //
                // alternating
                //______________________________________________________________
            case Y_ALTERN: {
                ++curr; // skip alternation sign
                auto_ptr<Logical> alt( new OR() );
                *alt << sub.yield(); //!< LHS
                *alt << subExpr();   //!< RHS
                return alt.yield(); }

                //______________________________________________________________
                //
                // simple jokers
                //______________________________________________________________
            case '?':
            case '*':
            case '+':
                if(sxp.operands.size<=0) throw exception("%sunexpected joker '%c'",fn,C);
                createSimpleJoker(sxp.operands,C);
                ++curr; // skip joker sign
                break;

                //______________________________________________________________
                //
                // dot
                //______________________________________________________________
            case '.':
                sxp << posix::dot();
                ++curr;
                break;

                //______________________________________________________________
                //
                // escape sequence
                //______________________________________________________________
            case '\\':
                subEscapeSequence(sxp.operands);
                break;

                //______________________________________________________________
                //
                // curly braced jokers
                //______________________________________________________________
            case Y_LBRACE:
                createBracedJoker(sxp.operands);
                break;

                //______________________________________________________________
                //
                // start a new group
                //______________________________________________________________
            case Y_LBRACK:
                createGroup(sxp.operands);
                break;

                //______________________________________________________________
                //
                // default
                //______________________________________________________________
            default:
                sxp << new Single(C);
                ++curr;
        }
    }

END_SXP:
    //__________________________________________________________________________
    //
    // sanity checks
    //__________________________________________________________________________
    if( sxp.operands.size <= 0 )
    {
        throw exception("%sempty sub expression",fn);
    }
    
    return sub.yield();
}
