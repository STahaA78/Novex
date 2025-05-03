%{
#include "y.tab.h"
#include <string.h>
int yylex();
%}

%%

"make"              { return MAKE; }
"int"               { return TYPE_INT; }
"print:::"          { return PRINT; }
"write:::"          { return WRITE; }
"operation"         { return OPERATION; }
"sum"               { return SUM;}
"sub"               { return SUB;}
"product"           { return MULT;}
"copy"              { return COPY; }
"equals?"           { return EQUALS; }
"="                 {return ASSIGN;}
\/\/"Main"\/\/      {return MAIN;}
\/\/[a-zA-Z_][a-zA-Z0-9_]*\/\/ {yylval.str = strdup(yytext + 2); yylval.str[strlen(yylval.str) - 2] = '\0'; return FUNCTION_NAME;}

"_$"[^$]*"$_" {
    yylval.str = strdup(yytext+2);
    yylval.str[strlen(yylval.str) - 2] = '\0';
    return STRING;
}


[0-9]+              { yylval.num = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.str = strdup(yytext); return IDENTIFIER; }

":"                 {return Colon;}
","                 { return COMMA; }
"["                 { return LBRACKET; }
"]"                 { return RBRACKET; }

[ \t\r\n]+          ;
.                   { return yytext[0]; }

%%
int yywrap (void) {return 1;}