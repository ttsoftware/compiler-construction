#include "llvm/Pass.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Instructions.h"
#include "LatticeValue.hpp"
#include "VariableEntry.hpp"


using namespace llvm;

inline VariableEntry::VariableEntry(Value* var, bool isPointer) {
    this->isPointer = isPointer;
    this->var = var;
}

Value* VariableEntry::getVar() {
    return this->var;
}

bool VariableEntry::isPtr() {
    return this->isPointer;
}

bool VariableEntry::isMaybeNull() {
    return this->getVar() == (Value*) Enums::LatticeValue::MAYBE_NULL;
}

bool VariableEntry::equals(VariableEntry other) {
    return this->var == other.getVar();
}

VariableEntry* VariableEntry::merge(VariableEntry* other) {
    VariableEntry* ret = new VariableEntry(NULL, NULL);
    if (other->getVar() == this->getVar()) {
        ret->var = this->getVar();
        ret->isPointer = other->isPtr();
    } else {
        ret->var = (Value*) Enums::LatticeValue::MAYBE_NULL;
    }
    return ret;
}

