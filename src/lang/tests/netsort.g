.netsort;

networks: network*;

network : size line+;

size   : "[:digit:]+";
line   : '[' pair ( ',' pair)* ']';
pair   : '[' size ',' size ']';

@drop   : "[:blank:]+";
@endl   : "[:endl:]";
