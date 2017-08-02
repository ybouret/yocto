inline Pattern *subExpr()
{
    auto_ptr<AND> sub( new AND() );
    Logical      &sxp = *sub;

    while(curr<last)
    {
        const char C = curr[0];
        switch(C)
        {

                //__________________________________________________
                //
                // grouping
                //__________________________________________________
            case Y_LPAREN:
                ++curr; // skip left parenthesis
                ++depth;
                sxp << subExpr();
                break;

            case Y_RPAREN:
                if(--depth<0) throw exception("%sunexpected '%c'",fn,Y_RPAREN);
                ++curr; // skip right parenthesis
                goto END_SXP;

                //__________________________________________________
                //
                // alternating
                //__________________________________________________
            case Y_ALTERN: {
                ++curr; // skip alternation sign
                auto_ptr<Logical> alt( new OR() );
                *alt << Pattern::Optimize(sub.yield()); //!< LHS
                *alt << subExpr();   //!< RHS
                return Pattern::Optimize(alt.yield()); }

                //__________________________________________________
                //
                // simple jokers
                //__________________________________________________
            case '?':
            case '*':
            case '+':
                if(sxp.operands.size<=0) throw exception("%sunexpected joker '%c'",fn,C);
                createSimpleJoker(sxp.operands,C);
                ++curr; // skip joker sign
                break;

                //__________________________________________________
                //
                // dot
                //__________________________________________________
            case '.':
                sxp << posix::dot();
                ++curr;
                break;

                //__________________________________________________
                //
                // escape sequence
                //__________________________________________________
            case '\\':
                subEscapeSequence(sxp.operands);
                break;

                //__________________________________________________
                //
                // curly braced jokers
                //__________________________________________________
            case Y_LBRACE:
                createBracedJoker(sxp.operands);
                break;

                //__________________________________________________
                //
                // start a new group
                //__________________________________________________
            case Y_LBRACK:
                createGroup(sxp.operands);
                break;

                //__________________________________________________
                //
                // default
                //__________________________________________________
            default:
                sxp << new Single(C);
                ++curr;
        }
    }

END_SXP:
    //______________________________________________________________
    //
    // sanity checks
    //______________________________________________________________
    if( sxp.operands.size <= 0 )
    {
        throw exception("%sempty sub expression",fn);
    }
    
    return Pattern::Optimize(sub.yield());
}
