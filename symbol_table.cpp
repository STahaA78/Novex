#include "symbol_table.h"

void SymbolTable::declareVariable(const std::string& name, const std::string& type) {
    if (table.count(name)) {
        std::cerr << "Error: Variable '" << name << "' already declared\n";
        return;
    }
    table[name] = {type, 0, false};
}

void SymbolTable::defineVariable(const std::string& name, int value) {
    if (!table.count(name)) {
        std::cerr << "Error: Variable '" << name << "' not declared\n";
        return;
    }
    table[name].value = value;
    table[name].isDefined = true;
}

bool SymbolTable::isDeclared(const std::string& name) const {
    return table.count(name);
}

bool SymbolTable::isDefined(const std::string& name) const {
    auto it = table.find(name);
    return it != table.end() && it->second.isDefined;
}

int SymbolTable::getVariableValue(const std::string& name) const {
    auto it = table.find(name);
    if (it == table.end() || !it->second.isDefined) {
        std::cerr << "Error: Variable '" << name << "' is not defined\n";
        return 0;
    }
    return it->second.value;
}

void SymbolTable::print() const {
    std::cout << "\n--- Symbol Table ---\n";
    for (const auto& [name, info] : table) {
        std::cout << name << " (" << info.type << ") = " << info.value
                  << " [" << (info.isDefined ? "defined" : "declared") << "]\n";
    }
}
