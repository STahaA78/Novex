#include "IR.h"

extern char *yytext;
extern int yylineno;

SymbolTable *globalSymbolTable = nullptr;
LLVMContext context;
Module *module = nullptr;
IRBuilder<> builder(context);

void initializeLLVM()
{
    module = new Module("top", context);
    globalSymbolTable = new SymbolTable();
}
void addReturnInstr()
{
   
    if (!builder.GetInsertBlock())
    {
       
        return;
    }
    builder.CreateRet(ConstantInt::get(context, APInt(32, 0)));
   
}
void printLLVMIR()
{
  


    // ——— New: Write LLVM IR to output.ll ———
    std::error_code EC;
    raw_fd_ostream out("output.ll", EC);
    if (EC)
    {
        fprintf(stderr, "Error opening output.ll: %s\n", EC.message().c_str());
        return;
    }

    module->print(out, nullptr);
    fprintf(stderr, "LLVM IR written to output.ll\n");
}
Value *performBinaryOperation(Value *lhs, Value *rhs, const std::string &op)
{
    Type *type = lhs->getType();

    if (type != rhs->getType())
    {
        yyerror("Binary operation between incompatible types");
        exit(EXIT_FAILURE);
    }
    if (type->isIntegerTy())
    {
        if (op == "+")
            return builder.CreateAdd(lhs, rhs, "addtmp");
        if (op == "-")
            return builder.CreateSub(lhs, rhs, "subtmp");
        if (op == "*")
            return builder.CreateMul(lhs, rhs, "multmp");
        if (op == "/")
            return builder.CreateSDiv(lhs, rhs, "divtmp");
    }
    else
    {
        yyerror("Binary operation of type other than integer");
        exit(EXIT_FAILURE);
    }
    yyerror(("Unsupported or illegal operator: " + op).c_str());
    exit(EXIT_FAILURE);
}
Value *performComparison(Value *lhs, Value *rhs, const std::string &op)
{
    Value* cmp = nullptr;
    if (lhs->getType()->isIntegerTy() && rhs->getType()->isIntegerTy())
    {
        if (op == "<")
            cmp=builder.CreateICmpSLT(lhs, rhs, "iless");
        if (op == ">")
            cmp= builder.CreateICmpSGT(lhs, rhs, "igreater");
        if (op == "==")
            cmp= builder.CreateICmpEQ(lhs, rhs, "iequal");
        if (op == "<=")
            cmp= builder.CreateICmpSLE(lhs, rhs, "ilessequal");
        if (op == ">=")
            cmp= builder.CreateICmpSGE(lhs, rhs, "igreaterequal");
        return builder.CreateZExt(cmp, Type::getInt32Ty(context), "zexttmp");
        yyerror("illegal integer comparison operation");
        exit(EXIT_FAILURE);
    }
    else
    {
        yyerror("comparison between incompatible types");
        exit(EXIT_FAILURE);
    }
}
void yyerror(const char *s)
{
    fprintf(stderr, "Parse error at line %d: %s\n", yylineno, s);
}