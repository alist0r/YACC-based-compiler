%{
#include "y.tab.h"
int lineno = 1;
int colno = 1;
%}
%%
["].*["]             colno += strlen(yytext); return STRING_LIT;
"for"                colno += 3; return FOR;
"if"                 colno += 2; return IF;
"return"             colno += 6; return RETURN;
"else"               colno += 4; return ELSE;
"int"                colno += 3; yylval.type = U64; return TYPE;
"float"                colno += 3; yylval.type = FLOAT; return TYPE;
[a-z][a-z0-9]* {
	yylval.lab.type = ID;
	yylval.lab.line = lineno; 
	yylval.lab.col = colno; 
	yylval.lab.str = strdup(yytext); 
	colno += strlen(yytext);
	return ID;
}
-?([0-9]*[.])?[0-9]+ {
	yylval.int_lit.type = LITERAL; 
	yylval.int_lit.line = lineno; 
	yylval.int_lit.col = colno; 
	yylval.int_lit.val = atoi(yytext); 
	colno += strlen(yytext);
	return LITERAL;
}
"+"                  ++colno; return ADD;
"-"                  ++colno; return SUB;
"*"                  ++colno; return MUL;
"/"                  ++colno; return DIV;
"="                  ++colno; return MOV;
"}"                  ++colno; return RIGHT_CURLY;
"{"                  ++colno; return LEFT_CURLY;
"]"                  ++colno; return RIGHT_SQUARE;
"["                  ++colno; return LEFT_SQUARE;
")"                  ++colno; return RIGHT_PARENTHESES;
"("                  ++colno; return LEFT_PARENTHESES;
","                  ++colno; return COMMA;
";"                  ++colno; return SEMICOLON;
[ \t] ++colno; 
[\n] colno = 1; ++lineno;
. printf("unexpected char on line %d: %s\n", lineno, yytext); exit(1);

%%
int yywrap() {
	return 1;
}
