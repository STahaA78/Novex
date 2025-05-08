#ifndef IR_H
#define IR_H
#include "symbol_table.h"
extern SymbolTable *globalSymbolTable;
Value *performComparison(Value *lhs, Value *rhs, const std::string &op);
Value *performBinaryOperation(Value *lhs, Value *rhs, const std::string &op);
void yyerror(const char *err);
void initializeLLVM();
void printLLVMIR();
void addReturnInstr();
#endif
