#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <iostream>

struct VarInfo {
    std::string type;
    int value;
    bool isDefined;
};

class SymbolTable {
public:
    void declareVariable(const std::string& name, const std::string& type);
    void defineVariable(const std::string& name, int value);
    bool isDeclared(const std::string& name) const;
    bool isDefined(const std::string& name) const;
    int getVariableValue(const std::string& name) const;
    void print() const;

private:
    std::unordered_map<std::string, VarInfo> table;
};

#endif // SYMBOL_TABLE_H
