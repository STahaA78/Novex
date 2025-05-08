#include "AST.h"
#include "IR.h"
#include <llvm/Support/raw_ostream.h>
#include <fstream>
#include <unordered_map>
#include <iostream>

const std::unordered_map<std::string, TypeAST::TypeGenerator> TypeAST::typeMap = {
    {"INTEGER", [](llvm::LLVMContext &ctx)
     {
         return llvm::Type::getInt32Ty(ctx);
     }},
    {"STRING", [](llvm::LLVMContext &ctx)
     {
         return llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(ctx));
     }},
};
Value *PrintAST::codegen()
{
    Function *printfFunc = module->getFunction("printf");
    if (!printfFunc)
    {
        auto *printfTy = FunctionType::get(
            builder.getInt32Ty(),                    
            PointerType::get(builder.getInt8Ty(), 0), 
            true);
        printfFunc = Function::Create(
            printfTy,
            Function::ExternalLinkage,
            "printf",
            module);
    }
    std::string fmt;
    std::vector<Value *> args;
    for (auto &exp : expressions)
    {
        Value *v = exp->codegen();
        if (!v)
            return nullptr; 

        if (v->getType()->isIntegerTy())
            fmt += "%d";
        else if (v->getType()->isPointerTy())
            fmt += "%s";

        args.push_back(v);
    }
    Constant *fmtConst = ConstantDataArray::getString(
        context, fmt + "\n", true);
    auto *fmtGV = new GlobalVariable(
        *module,
        fmtConst->getType(),
        true,
        GlobalValue::PrivateLinkage,
        fmtConst);
    Value *fmtPtr = builder.CreateConstGEP1_32(
        fmtGV->getValueType(), fmtGV, 0);
    args.insert(args.begin(), fmtPtr);
    CallInst *call = builder.CreateCall(printfFunc, args);
    return call;
}
Value *IdentifierAST::codegen()
{
    Value *ptr = globalSymbolTable->lookupSymbol(name);
    if (!ptr)
    {
        errs() << "Error: Symbol not found for " << name << "\n";
        return nullptr;
    }
    if (isa<AllocaInst>(ptr))
    {
        auto *allocaInst = dyn_cast<AllocaInst>(ptr);
        Type *elementType = allocaInst->getAllocatedType();
        return builder.CreateLoad(elementType, ptr, "loaded_" + name);
    }
    else if (isa<GlobalVariable>(ptr))
    {
        auto *globalVar = dyn_cast<GlobalVariable>(ptr);
        Type *elementType = globalVar->getValueType();
        return builder.CreateLoad(elementType, ptr, "loaded_" + name);
    }
    else if (isa<Argument>(ptr))
    {
        return ptr;
    }
    else
    {
        errs() << "Unexpected pointer type for " << name << "\n";
        return nullptr;
    }
}
Value *IfAST::codegen()
{
    globalSymbolTable->enterScope();
    Value *condVal = condition->codegen();
    if (!condVal)
    {
        errs() << "Error: Failed to generate condition value\n";
        return nullptr;
    }
    llvm::Type *condType = condVal->getType();
    Value *boolCond = nullptr;
    if (condType->isIntegerTy(1)) {
        boolCond = condVal;
    } else if (condType->isIntegerTy(32)) {
        boolCond = builder.CreateICmpNE(condVal, ConstantInt::get(condType, 0), "intCond");
    } 
    else if (condType->isPointerTy()) {
        Value *zero = ConstantInt::get(Type::getInt32Ty(context), 0);
        Value *firstCharPtr = builder.CreateGEP(
            Type::getInt8Ty(context), condVal, {zero}, "firstCharPtr");
        Value *firstChar = builder.CreateLoad(Type::getInt8Ty(context), firstCharPtr, "firstChar");
        boolCond = builder.CreateICmpNE(firstChar, ConstantInt::get(Type::getInt8Ty(context), 0), "strCond");
    }
    else {
        errs() << "Unsupported condition type for if\n";
        return nullptr;
    }
    Function *function = builder.GetInsertBlock()->getParent();
    BasicBlock *thenBB = BasicBlock::Create(context, "if.then", function);
    BasicBlock *elseBB = BasicBlock::Create(context, "if.else", function);
    BasicBlock *mergeBB = BasicBlock::Create(context, "if.end", function);
    builder.CreateCondBr(boolCond, thenBB, elseBB);
    // then block
    builder.SetInsertPoint(thenBB);
    thenBlock->codegen();
    builder.CreateBr(mergeBB);
    // else block
    builder.SetInsertPoint(elseBB);
    if (elseBlock)
        elseBlock->codegen();
    builder.CreateBr(mergeBB);
    // continue
    builder.SetInsertPoint(mergeBB);
    globalSymbolTable->exitScope();
    return nullptr;
}


Value *GlobalDeclarationAST::codegen()
{
    llvm::Type *varType = TypeAST::typeMap.at(type->type)(context);
    llvm::Constant *initializer = llvm::Constant::getNullValue(varType);
    llvm::GlobalVariable *gVar = new llvm::GlobalVariable(
        *module,                         
        varType,                         
        false,                           
        llvm::GlobalValue::ExternalLinkage, 
        initializer,                     
        identifier->name               
    );
    globalSymbolTable->setSymbol(identifier->name, gVar, varType);
    return gVar;
}
Value *LocalDeclarationAST::codegen()
{
    llvm::Type *varType = TypeAST::typeMap.at(type->type)(context);
    AllocaInst *alloca = builder.CreateAlloca(varType, nullptr, identifier->name);
    globalSymbolTable->setSymbol(identifier->name, alloca, varType);
    return alloca;
}
Value *CopyAST::codegen()
{
    Value *lhsPtr = globalSymbolTable->lookupSymbol(lhs->name);
    if (!lhsPtr)
    {
        llvm::outs() << "Unknown variable: " << lhs->name << "\n";
        return nullptr;
    }
    llvm::Type *lhsType = globalSymbolTable->getSymbolType(lhs->name);
    if (!lhsType)
    {
        llvm::outs() << "Failed to get type for variable: " << lhs->name << "\n";
        return nullptr;
    }
    Value *rhsVal = rhs->codegen();
    if (!rhsVal)
        return nullptr;
    llvm::Type *rhsType = rhsVal->getType();
    if (lhsType != rhsType)
    {
        llvm::outs() << "Type mismatch: Cannot assign " << *rhsType << " to " << *lhsType << "\n";
        return nullptr;
    }
    builder.CreateStore(rhsVal, lhsPtr);
    return rhsVal;
}
Value *ReadAST::codegen()
{
    Value *varPtr = globalSymbolTable->lookupSymbol(Identifier->name);
    Type *varType = globalSymbolTable->getSymbolType(Identifier->name);
    if (!varPtr || !varType)
    {
        errs() << "Error: undeclared variable '" << Identifier->name << "'\n";
        return nullptr;
    }
    FunctionCallee scanfFunc = module->getOrInsertFunction(
        "scanf",
        FunctionType::get(
            builder.getInt32Ty(),
            PointerType::getUnqual(builder.getInt8Ty()),
            true));
    std::string fmt;
    if (varType->isIntegerTy(32))
        fmt = "%d";
    else if (varType->isPointerTy())
    {
        fmt = "%s";
        FunctionCallee mallocFunc = module->getOrInsertFunction(
            "malloc",
            FunctionType::get(
                PointerType::getUnqual(builder.getInt8Ty()), 
                {builder.getInt32Ty()}, 
                false));
        Value *size = builder.getInt32(100);
        Value *buffer = builder.CreateCall(mallocFunc, {size});
        Value *fmtPtr = builder.CreateGlobalStringPtr(fmt, ".fmt_" + Identifier->name);
        builder.CreateCall(scanfFunc, {fmtPtr, buffer});
        builder.CreateStore(buffer, varPtr);
        return nullptr;
    }
    else
    {
        errs() << "Error: unsupported input type for '" << Identifier->name << "'\n";
        return nullptr;
    }

    Value *fmtPtr = builder.CreateGlobalStringPtr(fmt, ".fmt_" + Identifier->name);

    builder.CreateCall(scanfFunc, {fmtPtr, varPtr});

    return nullptr;
}

Value *BinaryOpAST::codegen()
{
    Value *lhs = expression1->codegen();
    Value *rhs = expression2->codegen();
    return performBinaryOperation(lhs, rhs, op);
}

Value *ComparisonAST::codegen()
{
    Value *lhsVal = LHS->codegen();
    Value *rhsVal = RHS->codegen();
    return performComparison(lhsVal, rhsVal, cmpOp);
}
Value *StatementBlockAST::codegen()
{
    Value *lastValue = nullptr;
    for (auto *stmt : statements)
    {
        if (stmt)
        {
            lastValue = stmt->codegen();
        }
    }
    return lastValue;
}
Value *FuncAST::codegen()
{
    globalSymbolTable->enterScope();
    llvm::Type *FuncType = TypeAST::typeMap.at(returnType->type)(context);
    FunctionType *funcType = FunctionType::get(FuncType, {}, false);
    Function *function = Function::Create(funcType, Function::ExternalLinkage, Identifier->name, *module);
    IRBuilderBase::InsertPoint savedIP = builder.saveIP();
    BasicBlock *entryBB = BasicBlock::Create(context, "entry", function);
    builder.SetInsertPoint(entryBB);
    statementsBlock->codegen();
    builder.CreateRet(ConstantInt::get(function->getReturnType(), 0));
    globalSymbolTable->exitScope();
    builder.restoreIP(savedIP);
    return function;
}
Value *MainFuncAST::codegen()
{
    globalSymbolTable->enterScope();
    FunctionType *mainTy = FunctionType::get(builder.getInt32Ty(), false);
    Function *function= Function::Create(mainTy, Function::ExternalLinkage, "main", module);
    IRBuilderBase::InsertPoint savedIP = builder.saveIP();
    BasicBlock *entry = BasicBlock::Create(context, "entry", function);
    builder.SetInsertPoint(entry);
    statementsBlock->codegen();
    builder.CreateRet(ConstantInt::get(function->getReturnType(), 0));
    globalSymbolTable->exitScope();
    builder.restoreIP(savedIP);
    return function;
}
Value *RunAST::codegen()
{
    Function *callee = module->getFunction(name);
    if (!callee)
    {
        errs() << "Unknown function: " << name << "\n";
        return nullptr;
    }
    return builder.CreateCall(callee,{});
}
Value *LogicalOpAST::codegen()
{
    if (cmpOp == "NOT")
    {
        if (!RHS)
        {
            errs() << "NOT operation requires a right-hand side operand (is NULL)\n";
            return nullptr;
        }
        Value *rhsVal = RHS->codegen();
            if (!rhsVal)
            {
                return nullptr;
            }
            if (!rhsVal->getType()->isIntegerTy(1))
            {
                rhsVal = builder.CreateICmpEQ(rhsVal, ConstantInt::get(rhsVal->getType(), 1), "tobool");
            }
            return builder.CreateNot(rhsVal, "nottmp");
    }
    if (!LHS || !RHS)
    {
        errs() << "Error: NULL operand for binary logical operator: " << cmpOp << "\n";
        return nullptr;
    }
    Value *lhsVal = LHS->codegen();
    Value *rhsVal = RHS->codegen();
    if (!lhsVal || !rhsVal)
        return nullptr;

    if (!lhsVal->getType()->isIntegerTy(1))
    {
        lhsVal = builder.CreateICmpNE(
            lhsVal,
            ConstantInt::get(lhsVal->getType(), 0),
            "tobool");
    }
    if (!rhsVal->getType()->isIntegerTy(1))
    {
        rhsVal = builder.CreateICmpNE(
            rhsVal,
            ConstantInt::get(rhsVal->getType(), 0),
            "tobool");
    }
    if (cmpOp == "AND")
    {
        return builder.CreateAnd(lhsVal, rhsVal, "andtmp");
    }
    else if (cmpOp == "OR")
    {
        return builder.CreateOr(lhsVal, rhsVal, "ortmp");
    }
    else
    {
        errs() << "Unknown logical operator: " << cmpOp << "\n";
        return nullptr;
    }
}

Value *ForAST::codegen()
{
    globalSymbolTable->enterScope();
    Function *function = builder.GetInsertBlock()->getParent();
    BasicBlock *condBB = BasicBlock::Create(context, "for.cond", function);
    BasicBlock *loopBB = BasicBlock::Create(context, "for.body", function);
    BasicBlock *afterBB = BasicBlock::Create(context, "for.end", function);
    // Initialize loop variable (e.g., INDEX = 1)
    if (assignment)
        assignment->codegen();
    // Jump to condition block
    builder.CreateBr(condBB);
    // Condition block
    builder.SetInsertPoint(condBB);
    Value *condValue = condition->codegen(); // Should return i1 from ComparisonAST
    // If condValue is i32, convert it to i1
    if (condValue->getType()->isIntegerTy(32)) {
        condValue = builder.CreateICmpNE(condValue, ConstantInt::get(condValue->getType(), 0), "condBool");
    }
    builder.CreateCondBr(condValue, loopBB, afterBB);
    // Loop body
    builder.SetInsertPoint(loopBB);
    forBlock->codegen();
    // Step update
    increment->codegen();
    // Loop back
    builder.CreateBr(condBB);
    // After loop
    builder.SetInsertPoint(afterBB);
    globalSymbolTable->exitScope();
    return nullptr;
}
