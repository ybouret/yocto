.simple;

code    : ((assign|id|string) ';') *;
assign  : id '=' (string) ;
id      : "[:alpha:][:word:]*\\t?";

@string : "cstring";

@drop   : "[:blank:]" ' ';
@endl   : "[:endl:]";

@comment: "#";
@comment: "/\\*" "\\*/";


/*
main   : (a b) h*;
second : ((c));
third  : d | "hello"+ | "cow";
fourth : (a b)+ | (c d);
fifth  : ("cow"|"boy")+ 'running';
sixth  : $ID49IAAAAAE6LT46YXo=$ some?;

@drop : "[:blank:]+";
@endl : "[:endl:]";
@string : "cstring";
@litteral : 'rstring';
%noSingle : main;
*/
