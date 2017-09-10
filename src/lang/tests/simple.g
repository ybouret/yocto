.simple;

code    : ((assign|id|string) ';') *;
assign  : id '=' (string) ;
id      : "[:alpha:][:word:]*\\t?";

@string : "cstring";

@drop   : "[:blank:]" ' ';
@endl   : "[:endl:]";

@comment: "#";
@comment: "/\\*" "\\*/";

%MergesSingle : code;

