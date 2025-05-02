%{
	#include <stdio.h>
	#include <stdlib.h>
	
	extern int yyparse();
	extern int yylex();
	extern FILE *yyin;
	void yyerror(const char *err);

	
	//#define DEBUGBISON
	//This code is for producing debug output.
	#ifdef DEBUGBISON
		#define debugBison(a) (printf("\n%d \n",a))
	#else
		#define debugBison(a)
	#endif
%}

%union {
    int num;
    char *str;
}

%token <str> IDENTIFIER FUNCTION_NAME STRING
%token <num> NUMBER

%token MAKE TYPE_INT PRINT WRITE OPERATION COPY EQUALS SUM MULT SUB
%token COMMA LBRACKET RBRACKET Colon

%start program

%%

program:
    declarations functions      {printf("Parsed complete program.\n");}
    ;

declarations:

    | declarations declaration   
    ;

declaration:
    MAKE TYPE_INT Colon IDENTIFIER        {printf("Declared variable: int %s\n", $4);}
    ;

functions:
    
    | functions function
    ;

function:
    FUNCTION_NAME LBRACKET statements RBRACKET      {printf("Parsed function block: %s\n", $1);}
    ;

statements:
    
    | statements statement
    ;

statement:
      PRINT STRING                                      {printf("Print string: %s\n", $2);}
    | PRINT IDENTIFIER                                  { printf("Print variable: %s\n", $2);}
    | WRITE IDENTIFIER                                  {printf("Write prompt for: %s\n", $2);}
    | OPERATION operands                                {printf("Operation executed.\n");}
    | IDENTIFIER COMMA EQUALS COMMA IDENTIFIER          {printf("Compare: %s == %s\n", $1, $5);}
    | IDENTIFIER COMMA COPY COMMA IDENTIFIER                  {printf("Copy value into: %s of %s\n", $1,$5);}
    ;

operands:
    operand_list
    ;

operand_list:
    IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER        {printf("Operands: %s, %s, %s\n", $1, $3, $5);}
    | operand_list COMMA IDENTIFIER                     {printf("Additional operand: %s\n", $3);}
    ;

%%
void yyerror(const char *s) {
    fprintf(stderr, "Syntax error: %s\n", s);
}
int main(){
    yyparse();
    return 0;
}

