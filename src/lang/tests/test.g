.test;

output : (decl|assign)*;
decl   : IDENT END;
assign : IDENT '=' (INT|STRING) END;

INT    : "[0-9]+";
IDENT  : "[:alpha:]+";
END    : ';';

@STRING : cstring;

@drop   : "[:blank:]+";
@endl   : "[:endl:]";
