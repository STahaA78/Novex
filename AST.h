#ifndef AST_H
#define AST_H

#include "IR.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "symbol_table.h"
using namespace llvm;

class ASTNode
{
public:
    virtual ~ASTNode() = default;
    virtual llvm::Value *codegen() = 0;
};
class TypeAST : public ASTNode
{
public:
    std::string type;
    using TypeGenerator = std::function<llvm::Type *(llvm::LLVMContext &)>;
    static const std::unordered_map<std::string, TypeGenerator> typeMap;

    TypeAST(const std::string &t) : type(t) {}
    Value *codegen() override
    {
        return nullptr;
    }
};
class IdentifierAST : public ASTNode
{
public:
    std::string name;
    IdentifierAST(const std::string &name) : name(name) {}
    Value *codegen() override;
};
class LocalDeclarationAST : public ASTNode
{
public:
    IdentifierAST *identifier;
    TypeAST *type;
    LocalDeclarationAST(IdentifierAST *id, TypeAST *t)
        : identifier(id), type(t) {}
    Value *codegen() override;
};
class GlobalDeclarationAST : public ASTNode
{
public:
    IdentifierAST *identifier;
    TypeAST *type;
    GlobalDeclarationAST(IdentifierAST *id, TypeAST *t)
        : identifier(id), type(t) {}

    llvm::Value *codegen() override;
};
class PrintAST : public ASTNode
{
public:
    std::vector<ASTNode *> expressions;
    PrintAST(std::vector<ASTNode *> expressions)
        : expressions(std::move(expressions)) {}

    Value *codegen() override;
};
class StringLiteralAST : public ASTNode
{
public:
    std::string value;
    StringLiteralAST(const std::string &val) : value(val) {}
    Value *codegen()
    {
        Constant *strConst = ConstantDataArray::getString(context, value);
        GlobalVariable *gvar = new GlobalVariable(
            *module,
            strConst->getType(),
            true,
            GlobalValue::PrivateLinkage,
            strConst,
            ".str");
        return gvar;
    }
};
class IntegerLiteralAST : public ASTNode
{
public:
    int value;
    IntegerLiteralAST(int val) : value(val) {}
    Value *codegen()
    {
        return ConstantInt::get(Type::getInt32Ty(context), value);
    }
};
class CopyAST : public ASTNode
{
public:
    IdentifierAST *lhs;   
    ASTNode *rhs;         
    CopyAST(IdentifierAST *lhsId, ASTNode *rhsExpr)
        : lhs(lhsId), rhs(rhsExpr) {}

    Value *codegen() override;
};
class ReadAST : public ASTNode
{
public:
    IdentifierAST *Identifier;

    ReadAST(IdentifierAST *id)
        : Identifier(id) {}
    Value *codegen() override;
};
class BinaryOpAST : public ASTNode
{
public:
    std::string op;
    ASTNode *expression1;
    ASTNode *expression2;

    BinaryOpAST(ASTNode *lhs, ASTNode *rhs, std::string operation)
        : op(operation), expression1(lhs), expression2(rhs) {}
    Value *codegen() override;
};
class ComparisonAST : public ASTNode
{
public:
    std::string cmpOp;
    ASTNode *LHS;
    ASTNode *RHS;

    ComparisonAST(ASTNode *lhs, ASTNode *rhs, std::string op)
        : LHS(lhs), RHS(rhs), cmpOp(op) {}
    Value *codegen() override;
};
class LogicalOpAST : public ComparisonAST
{
public:
    LogicalOpAST(ASTNode *left, ASTNode *right, const std::string &operation)
        : ComparisonAST(left, right, operation) {}
    Value *codegen() override;
};
class StatementBlockAST : public ASTNode
{
public:
    std::vector<ASTNode *> statements;
    StatementBlockAST(const std::vector<ASTNode *> &stmts) : statements(stmts) {}
    Value *codegen() override;
};
class FuncAST : public ASTNode
{
public:
    IdentifierAST *Identifier;
    StatementBlockAST *statementsBlock;
    TypeAST *returnType;
    FuncAST(IdentifierAST *funcidentifier, StatementBlockAST *block, TypeAST *retType)
        : Identifier(funcidentifier), statementsBlock(block), returnType(retType) {}
    Value *codegen() override;
};
class MainFuncAST : public ASTNode
{
public:
    StatementBlockAST *statementsBlock;
    TypeAST *returnType;

    MainFuncAST(StatementBlockAST *block, TypeAST *retType)
        : statementsBlock(block), returnType(retType) {}
    Value *codegen() override;
};
class RunAST : public ASTNode
{
public:
    std::string name;
    RunAST(const std::string &funcName)
        : name(funcName) {}
    Value *codegen() override;
};
class IfAST : public ASTNode
{
public:
    ComparisonAST *condition;
    StatementBlockAST *thenBlock;
    StatementBlockAST *elseBlock;

    IfAST(ComparisonAST *cond, StatementBlockAST *thenBlk, StatementBlockAST *elseBlk)
        : condition(cond), thenBlock(thenBlk), elseBlock(elseBlk) {}
    Value *codegen() override;
};
class ForAST : public ASTNode
{
public:
    CopyAST *assignment;
    ComparisonAST *condition;
    CopyAST *increment; 
    StatementBlockAST *forBlock;
    ForAST(CopyAST *assign, ComparisonAST *cond, CopyAST *incrementvalue, StatementBlockAST *forBlk)
        : assignment(assign), condition(cond), increment(incrementvalue), forBlock(forBlk) {}
    Value *codegen() override;
};
#endif

