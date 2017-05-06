//
// Created by tankzorx on 5/6/17.
//
/*#include "llvm/Pass.h"*/

#ifndef PROJECT_VARIABLEENTRY_H
#define PROJECT_VARIABLEENTRY_H

using namespace llvm;

class VariableEntry {

public:

    bool isPointer;
    Value *self;
    Value *var;

    /*VariableEntry();*/
    VariableEntry(Value*, bool);
    Value *getVar();
    bool isPtr();
    bool isMaybeNull();
    bool equals(VariableEntry);
    VariableEntry* merge(VariableEntry*);
};

#endif //PROJECT_VARIABLEENTRY_H
