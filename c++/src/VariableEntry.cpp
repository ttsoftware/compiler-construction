#include "llvm/Pass.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Instructions.h"
#include "LatticeValue.hpp"


using namespace llvm;

class VariableEntry {

public:

    bool isPointer;
    Value *self;
    Value *var;

    inline VariableEntry(Value *var, bool isPointer) {
        this->isPointer = isPointer;
        this->var = var;
    }

    Value *getVar() {
        return this->var;
    }

    bool isPtr() {
        return this->isPointer;
    }

    bool isMaybeNull() {
        return this->getVar() == (Value *) Enums::LatticeValue::MAYBE_NULL;
    }

    bool equals(VariableEntry other) {
        return this->var == other.getVar();
    }

    VariableEntry* merge(VariableEntry* other) {
        VariableEntry *ret = new VariableEntry(NULL, NULL);
        if (other->getVar() == this->getVar()) {
            ret->var = this->getVar();
            ret->isPointer = other->isPtr();
        } else {
            ret->var = (Value *) Enums::LatticeValue::MAYBE_NULL;
        }
        return ret;
    }
};
