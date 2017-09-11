.simple;

code    : ((assign|id|string|COM) ';') *;
assign  : id '=' (string) ;
id      : "[:alpha:][:word:]*\\t?";
COM     : 'COM';

@string : cstring;

@drop   : "[:blank:]" ' ';
@endl   : "[:endl:]";

@comment: "#";
@comment: "/\\*" "\\*/";

%MergesSingle : code;

