%{
#include <stdio.h>
#include <stdlib.h>
#include "symbol_table.h"
#include "AST.h"

extern int yylex();
extern FILE *yyin;
void yyerror(const char *err);
%}


%code requires {
    #include "AST.h"
    #include <vector>
    class TypeAST;  
}


%union {
    char* identifier;
    int integer_literal;
    char *string_literal;
    const char* op;
    ASTNode* ast_node;
    std::vector<ASTNode*>* statement_list;
    TypeAST* type_node;
    CopyAST* copy_node;
    ReadAST *input_node;
    StatementBlockAST* statement_block;
    ComparisonAST* comparison_node;
}

%token <identifier> IDENTIFIER 
%token <string_literal> STRING FUNCTION_NAME
%token <integer_literal> NUMBER

%token MAIN RUN
%token TYPE_STR TYPE_INT
%token PRINT READ 
%token OPERATION COPY   
%token SUM MULT SUB  DIV
%token EQUALS  GT LT GTE LTE
%token AND OR NOT
%token DOT COMMA LBRACKET RBRACKET COLON
%token IF ELSE
%token MAKE LOCAL
%token FOR INCREMENT GO



%type <statement_list> statements functions global_declarations 
%type <ast_node> statement global_declaration local_declaration term print_statement expr operation_statement run_statement function main_function comparison_statement if_statement for_statement
%type <copy_node> copy_statement
%type <type_node> type
%type <input_node> read_statement
%type <op> operators comparison_operators binary_logical_operators unary_logical_operators
%type <statement_block> else_statement
%type <comparison_node> condition_statement logical_statement
%type <integer_literal> increment integer_expr

%left OR
%left AND
%right NOT
%start program

%%

program:
    global_declarations functions main_function {
      

        globalSymbolTable->enterScope();

        bool hasError = false;

        for (ASTNode* node : *$1) {
          
            Value* result = node->codegen();

            delete node;
        }
        for (ASTNode* node : *$2) {
            Value* result = node->codegen();

            delete node;
        }
     
           
        Value* result = $3->codegen();


        globalSymbolTable->exitScope();
        delete $1;
        delete $2;
        delete $3;

        addReturnInstr();
     


        llvm::errs().flush();
    }
    ;


global_declarations:
    {
        $$ = new std::vector<ASTNode*>();
    }
    | global_declarations global_declaration 
    {
        $$ = $1;
        $$->push_back($2);
    }
    ;

global_declaration:
    MAKE type COLON IDENTIFIER DOT 
    {
        $$ = new GlobalDeclarationAST(new IdentifierAST(std::string($4)), $2);
        free($4);
    }
    ;


local_declaration:
    LOCAL type COLON IDENTIFIER DOT 
    {  
        $$ = new LocalDeclarationAST(new IdentifierAST(std::string($4)), $2);
        free($4);
    }
    ;

type:
    TYPE_INT 
    {
        $$=new TypeAST("INTEGER");
    }
    | TYPE_STR 
    {
        $$=new TypeAST("STRING");
    }
    ;


main_function:
    { 
        std::vector<ASTNode*> emptystmt;
        StatementBlockAST* block = new StatementBlockAST(emptystmt);
        $$ = new MainFuncAST(block, new TypeAST("INTEGER"));
    }
    |MAIN LBRACKET statements RBRACKET 
    { 
        StatementBlockAST* block = new StatementBlockAST(*$3);
        delete $3; // free the vector after moving its contents
        $$ = new MainFuncAST(block, new TypeAST("INTEGER"));
    }
    ;

statements:
    {
        $$ = new std::vector<ASTNode*>();
    }
    | statements statement 
    {
        $$ = $1;
        $$->push_back($2);
    }
;


statement:
      print_statement                {$$=$1;}
      | copy_statement               {$$=$1;}
      | read_statement               {$$=$1;}
      | operation_statement          {$$=$1;}
      | run_statement                {$$=$1;}
      | comparison_statement         {$$=$1;}
      | local_declaration            {$$=$1;}
      | if_statement                 {$$=$1;}
      | for_statement                {$$=$1;}
    ;


if_statement:
     IF LBRACKET logical_statement RBRACKET LBRACKET statements RBRACKET else_statement 
    {
      $$ = new IfAST($3,new StatementBlockAST(*$6), $8);
    }
    ;
else_statement:                   
    {
        std::vector<ASTNode*> emptystmt;
        $$ = new StatementBlockAST(emptystmt);
    }
    | ELSE LBRACKET statements RBRACKET 
    {
        $$=new StatementBlockAST(*$3);
    }
;


functions:
    {
         $$ = new std::vector<ASTNode*>();
    }
    |functions function   
    { 
        $$ = $1;
        $$->push_back($2);
    }
;

function:
   FUNCTION_NAME LBRACKET statements RBRACKET  
    { 
        StatementBlockAST* block = new StatementBlockAST(*$3);
        delete $3; // free the vector after moving its contents
        $$ = new FuncAST(new IdentifierAST($1), block, new TypeAST("INTEGER"));
        free($1);
    }
;

run_statement:
    RUN FUNCTION_NAME DOT 
    {
        $$ = new RunAST(std::string($2));
        free($2); 
    }
;



operation_statement:
    OPERATION operators COMMA IDENTIFIER COMMA expr COMMA expr DOT 
    {  
      if (!$6 || !$8) 
      {
         yyerror("Invalid operands in OPERATION");
      }
      IdentifierAST* lhs = new IdentifierAST($4);
      BinaryOpAST* operation = new BinaryOpAST($6, $8, $2);
      $$ = new CopyAST(lhs, operation);
      free($4);
    }
    ;

operators:
    SUM     {$$="+";}
    |SUB    {$$="-";}
    |MULT   {$$="*";}
    |DIV    {$$="/";}
    ;


condition_statement:
    comparison_operators COMMA expr COMMA expr  
    {
        
        if (!$3 || !$5) 
        {
         yyerror("Invalid operands in OPERATION");
        }
        $$ = new ComparisonAST($3, $5, $1);
    }
    ;

comparison_statement:
    comparison_operators IDENTIFIER COMMA expr COMMA expr DOT
    {
        if (!$4 || !$6)
        {
         yyerror("Invalid operands in OPERATION");
        }
        IdentifierAST* lhs = new IdentifierAST($2);
        ComparisonAST* operation = new ComparisonAST($4, $6, $1);
            $$ = new CopyAST(lhs, operation);
        free($2);
    }
    ;
comparison_operators:
    EQUALS      {$$="==";}
    |GT         {$$=">";}
    |LT         {$$="<";}
    |GTE        {$$=">=";} 
    |LTE        {$$="<=";}     
    ;

logical_statement:
    logical_statement binary_logical_operators logical_statement 
    {
        $$ = new LogicalOpAST($1, $3, $2);
    }
    | unary_logical_operators logical_statement   
    {
        $$=new LogicalOpAST(nullptr, $2,$1);
    }
    | condition_statement       {$$=$1;}
    

binary_logical_operators:
    AND         {$$="AND";}
    |OR         {$$="OR";}
    ;

unary_logical_operators:
    NOT  
    {
        $$="NOT";
    }
    ;

expr:
    IDENTIFIER          {$$ = new IdentifierAST($1);}
    | NUMBER            {$$=new IntegerLiteralAST($1);}
    ;


print_statement:
    PRINT term DOT            
    {   auto outputs = std::vector<ASTNode*>();
        outputs.push_back($2);
        $$ = new PrintAST(outputs);
    }
    ;

copy_statement:
    IDENTIFIER COMMA COPY COMMA term DOT 
    {
        $$=new CopyAST(new IdentifierAST($1),$5);
    }
    ;

read_statement:
    READ IDENTIFIER DOT    
    {
        $$= new ReadAST(new IdentifierAST($2));
    }
    ;

term:
    IDENTIFIER              {$$ = new IdentifierAST($1);}
    | STRING                {$$=new StringLiteralAST($1);}
    | NUMBER                {$$=new IntegerLiteralAST($1);}
    ;


for_statement:
    FOR copy_statement GO NUMBER increment LBRACKET statements RBRACKET 
    {
        auto loopVar = $2->lhs;
        auto startLit = dynamic_cast<IntegerLiteralAST*>($2->rhs);
        auto endLit = new IntegerLiteralAST($4);
        auto cond = new ComparisonAST(loopVar, endLit,(startLit && endLit && startLit->value > endLit->value) ? ">=" : "<=");
        auto increment = $5 ? new IntegerLiteralAST($5) : new IntegerLiteralAST(1);
        auto binaryOp = new BinaryOpAST(loopVar, increment, "+");
        auto incrementAssign = new CopyAST(loopVar, binaryOp);
        $$ = new ForAST($2, cond, incrementAssign, new StatementBlockAST(*$7));
    }
;


increment:
                                        { $$ = 1; }
  | INCREMENT integer_expr              { $$ = $2; }
;

integer_expr:
    NUMBER          {$$ = $1;}
  | '-' NUMBER       {$$ = -$2;}
;


%%

int main(int argc, char** argv) {
        if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp == NULL) {
            return EXIT_FAILURE;
        }
    yyin = fp;
     
    } 
    if (yyin == NULL) {
        yyin = stdin;
    
    }

    initializeLLVM();
  
    
    int parserResult = yyparse();
  

    printLLVMIR();
   
    
    return EXIT_SUCCESS;
}
