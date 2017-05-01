#include "llvm/Pass.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Instructions.h"
#include <list>
#include <unordered_map>
#include <queue>
#include <iostream>

#include "VariableEntry.cpp"
#include "LatticeValue.cpp"

using namespace llvm;

class NullPointerMap {

    Value *nullPointerPointer = (Value *) 4095;
    Value *isNull = (Value *) 52428;
    Value *unknown = (Value *) 999999;

    std::unordered_map<Value*,VariableEntry*> map;

public:
    void set(Value* key,Value* val, bool isPointer) {
        VariableEntry* ventry = new VariableEntry(val,isPointer);
        map[key] = ventry;
    }

    void set(Value* key,Value* val) {
//            errs() << "KEYSTART\n" << *key << "   KEYEND\n";
//            errs() << "VALUESTART\n" << *val << "   VALUEEND\n";
        bool isPointer = map[key]->isPtr();
        VariableEntry* ventry = new VariableEntry(val,isPointer);
        map[key] = ventry;
    }

    VariableEntry* get(Value* var) {
        return map[var];
    }

    bool entryIsPointer(Value* var) {
        return map[var]->isPtr();
    }

    bool entryIsNull(Value* var) {
        return map[var]->getVar() == isNull;
    }

    std::unordered_map<Value*, VariableEntry*> getInnerMap() {
        return map;
    };
};