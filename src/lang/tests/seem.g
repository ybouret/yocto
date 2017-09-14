// Simple Expression Evaluator Module
.seem;

// additive expression
AXP : (PLUS|MINUS)? MXP ( (PLUS|MINUS) MXP )*;

// multiplicative expression
MXP : PXP ( (MUL|DIV|MOD) PXP )*;

// power expression
PXP : ATOM (EXPONENT AXP)*;

ARGS : AXP (',' AXP)*;
FUNC : ID LPAREN ARGS RPAREN;
ATOM : NUMBER | FUNC | ID |  LPAREN AXP RPAREN;

PLUS     : '+';
MINUS    : '-';
MUL      : '*';
DIV      : '/';
MOD      : '%';
EXPONENT : '^';
LPAREN   : '(';
RPAREN   : ')';

NUMBER   : "[:digit:]+([.][:digit:]+)?([eE][-+][:digit:]+)?";
ID       : "([:alpha:]|_)([:word:]|_)*";

//lexical extra rules
@drop : "[:blank:]";
@endl : "[:endl:]";

//change the AST
%RPN : AXP (PLUS MINUS);
%RPN : MXP (MUL DIV MOD);
