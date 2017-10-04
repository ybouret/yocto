.simple;

code    : ((assign|id|string|COM|"hello") ';') *;
assign  : id '=' (string) ;
id      : "[:alpha:][:word:]*\\t?";
COM     : 'COM';

@include : "lex.g";

@string : cstring;

@drop   : "[:blank:]" ' ';
@endl   : "[:endl:]";

@comment: "/\\*" "\\*/";

