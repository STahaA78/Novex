%{
	#include <stdio.h>
	#include <stdlib.h>
    #include "symbol_table.h"
	extern int yyparse();
	extern int yylex();
	extern FILE *yyin;
	void yyerror(const char *err);
    SymbolTable vartable;
	
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

%token MAKE TYPE_INT PRINT WRITE OPERATION COPY EQUALS SUM MULT SUB MAIN ASSIGN
%token COMMA LBRACKET RBRACKET Colon

%start program

%%

program:
    declarations functions mainfunc      {printf("Parsed complete program.\n");}
    ;

declarations:

    | declarations declaration   
    ;

declaration:
    MAKE TYPE_INT Colon IDENTIFIER        {vartable.declareVariable($4,"int");}
    ;

functions:
    
    | functions function
    ;

function:
    FUNCTION_NAME LBRACKET statements RBRACKET      {printf("Parsed function block: %s\n", $1);}
    ;
mainfunc:
    MAIN LBRACKET statements RBRACKET               {printf("Main Fucntion!\n");}
statements:
    
    | statements statement
    ;

statement:
      PRINT STRING                                      {printf("%s\n", $2);}
    | PRINT IDENTIFIER                                  {printf("%d\n",vartable.getVariableValue($2));}
    | WRITE IDENTIFIER                                  {int num1;
                                                        scanf("%d", &num1);
                                                        vartable.defineVariable($2, num1);}
    | OPERATION operands                                {printf("Operation executed.\n");}
    | IDENTIFIER COMMA EQUALS COMMA IDENTIFIER          {printf("Compare: %s == %s\n", $1, $5);}
    | IDENTIFIER COMMA COPY COMMA IDENTIFIER            {vartable.defineVariable($1,vartable.getVariableValue($5));}
    | IDENTIFIER ASSIGN IDENTIFIER                      {vartable.defineVariable($1,vartable.getVariableValue($3));}
    | IDENTIFIER ASSIGN NUMBER                          {vartable.defineVariable($1,$3);}
    ;

operands:
    operand_list
    ;

operand_list:
    SUM COMMA IDENTIFIER  COMMA IDENTIFIER COMMA IDENTIFIER    {vartable.defineVariable($3,vartable.getVariableValue($5)+vartable.getVariableValue($7));}
    |SUB COMMA  IDENTIFIER  COMMA IDENTIFIER COMMA IDENTIFIER       {vartable.defineVariable($3,vartable.getVariableValue($5)-vartable.getVariableValue($7));}
    |MULT COMMA  IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER       {vartable.defineVariable($3,vartable.getVariableValue($5)*vartable.getVariableValue($7));}
    |operand_list COMMA IDENTIFIER                     {printf("Additional operand: %s\n", $3);}
    ;

%%
void yyerror(const char *s) {
    fprintf(stderr, "Syntax error: %s\n", s);
}
int main(int argc, char **argv){
    if (argc > 1) {
        yyin = fopen(argv[1], "r");  
        if (!yyin) {
            perror("Cannot open input file");
            return 1;
        }
    } else {
        yyin = stdin;
    }

    yyparse();  // Start parsing
    return 0;
}


