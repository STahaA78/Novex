// SymbolTable.h
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/Support/raw_ostream.h>
#include "llvm/IR/Verifier.h"


#include <vector>
#include <string>
#include <map>
#include <stack>
#include <utility>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstddef> 
#include <string>  
#include <vector>  
#include <memory>  

using namespace llvm;


extern LLVMContext context;
extern Module *module;
extern IRBuilder<> builder;
extern Function *mainFunction;

// #include "CodegenContext.h"

class SymbolTable
{
    struct SymbolEntry
    {
        llvm::Value *value;
        llvm::Type *type;
        bool isArray;
        int startIndex;
        int endIndex;
    };

public:
    SymbolTable() = default;

    void enterScope();
    void exitScope();
    llvm::Value *lookupSymbol(const std::string &id, llvm::Value *index = nullptr);
    void setSymbol(const std::string &id, llvm::Value *value, llvm::Type *type, bool isArray = false, int startIndex = -1, int endIndex = -1);
    llvm::Value *createNewSymbol(const std::string &id, llvm::Type *type, bool isArray = false, int startIndex = -1, int endIndex = -1);
    Type *getSymbolType(const std::string &id);

private:
    std::stack<std::unordered_map<std::string, SymbolEntry>> SymbolTableStack;
};

#endif // SYMBOLTABLE_H
