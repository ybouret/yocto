.nwsrt;

codes : code*;
code  : name swaps;
int   : "[:digit:]+";
info  : "-.*";
name  : int info;
swap  : '[' int ',' int ']';
swaps : '[' swap ( ',' swap )* ']';

@drop   : "[:blank:]+";
@endl   : "[:endl:]";

