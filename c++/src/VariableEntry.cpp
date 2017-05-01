#include "llvm/Pass.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Instructions.h"


using namespace llvm;

class VariableEntry {
    bool isPointer;
    Value* self;
    Value* var;

public:
    VariableEntry(Value* var,bool isPointer) {
        this->isPointer = isPointer;
        this->var = var;
    }

    Value* getVar() {
        return this->var;
    }

    bool isPtr() {
        return this->isPointer;
    }

};
