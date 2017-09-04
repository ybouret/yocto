.simple;

//main : a "c" 'd' a+ "b"? c*;
main   : (a b);
second : ((c));
third  : d | "hello"+ | "cow";
fourth : (a b)+ | (c d);
fifth  : ("cow"|"boy")+ 'running';
sixth  : $ID49IAAAAAE6LT46YXo=$;

@drop : "[:blank:]+";
@endl : "[:endl:]";
%noSingle : main;
