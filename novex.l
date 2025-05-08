%{
#include "novex.tab.h"
#include <string.h>

extern void yyerror(const char *msg);
extern int yylineno;
%}

%option yylineno
%option noyywrap

%%

"#"[^#]*"#"               {;}
"make"              { return MAKE; }
"local"             {return LOCAL;}
"int"               { return TYPE_INT; }
"str"               {return TYPE_STR;}
"print:::"          { return PRINT; }
"read:::"          { return READ; }
"operation"         { return OPERATION; }
"sum"               { return SUM;}
"sub"               { return SUB;}
"product"           { return MULT;}
"div"               {return DIV;}
"copy"              { return COPY; }
"equals?"           { return EQUALS; }
"gt"                {return GT;}
"lt"                {return LT;}
"gte"               {return GTE;}
"lte"               {return LTE;}
"if"                {return IF;}
"else"              {return ELSE;}
"and"               {return AND;}
"or"                {return OR;}
"not"               {return NOT;}
"run"               {return RUN;}
"for"               {return FOR;}
"go"                {return GO;}
"++"                {return INCREMENT;}

\/\/"main"\/\/      {return MAIN;}
\/\/[a-zA-Z_][a-zA-Z0-9_]*\/\/ {yylval.string_literal = strdup(yytext + 2); yylval.string_literal[strlen(yylval.string_literal) - 2] = '\0'; return FUNCTION_NAME;}



[a-zA-Z_][a-zA-Z0-9_]* { yylval.identifier = strdup(yytext); return IDENTIFIER; }
[0-9]+              { yylval.integer_literal = atoi(yytext); return NUMBER; }
"_$"[^$]*"$_" {
    yylval.string_literal = strdup(yytext+2);
    yylval.string_literal[strlen(yylval.string_literal) - 2] = '\0';
    return STRING;
}

"."                 {return DOT;}
":"                 {return COLON;}
","                 { return COMMA; }
"["                 { return LBRACKET; }
"]"                 { return RBRACKET; }

[ \t\r\n]+          ;
.                   { return yytext[0]; }

%%
