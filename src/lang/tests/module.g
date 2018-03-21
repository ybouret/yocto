
// testing modular
.module;
@comment: "/\\*" "\\*/";
@comment: "//";

prog : (AXP|string)*;

@include : "seem.g";

@string : cstring;

@include : "lex.g";
//@drop   : "[:blank:]" ' ';
//@endl   : "[:endl:]";
