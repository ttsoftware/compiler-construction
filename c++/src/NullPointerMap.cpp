#include "llvm/Pass.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Instructions.h"
#include <list>
#include <unordered_map>
#include <queue>
#include <iostream>
#include "LatticeValue.hpp"
#include "VariableEntry.hpp"

using namespace llvm;

class NullPointerMap {

    std::unordered_map<Value *, VariableEntry *> map;

public:
    void set(Value *key, Value *val, bool isPointer) {
        VariableEntry *ventry = new VariableEntry(val, isPointer);
        map[key] = ventry;
    }

    void set(Value *key, Value *val) {
        bool isPointer = map[key]->isPtr();
        VariableEntry *ventry = new VariableEntry(val, isPointer);
        map[key] = ventry;
    }

    void set(Value* key, VariableEntry* entry) {
        map[key] = entry;
    }

    VariableEntry *get(Value *var) {
        return map[var];
    }

    bool entryIsPointer(Value *var) {
        return map[var]->isPtr();
    }

    bool entryIsNull(Value *var) {
        return map[var]->getVar() == (Value *) Enums::LatticeValue::IS_NULL;
    }

    bool entryisMaybeNull(Value* var) {
        return map[var]->isMaybeNull();
    }

    std::unordered_map<Value *, VariableEntry *> getInnerMap() {
        return map;
    };

    std::vector<Value*> getKeys() {
        std::vector<Value*> keys;
        for ( auto it = map.begin(); it != map.end(); ++it ) {
            keys.push_back(it->first);
        }
        return keys;
    }

    NullPointerMap merge(NullPointerMap other) {
        NullPointerMap ret;
        std::unordered_map<Value*, VariableEntry*> otherMap = other.getInnerMap();
        std::unordered_map<Value*, VariableEntry*> thisMap = this->getInnerMap();
        std::vector<Value*> otherKeys = other.getKeys();
        std::vector<Value*> thisKeys = this->getKeys();
        for (int i = 0; i < otherKeys.size(); ++i) {
            // Beware: Readable code ahead.
            try {
                ret.set(otherKeys[i], otherMap[otherKeys[i]]->merge(
                        thisMap[otherKeys[i]]
                ));
            } catch (...) {
                ret.set(otherKeys[i],otherMap[otherKeys[i]]);
            }
        }

        for (int i = 0; i < thisKeys.size(); ++i) {
            // Beware: Readable code ahead.
            try {
                VariableEntry* k = otherMap[thisKeys[i]];
            } catch (...) {
                ret.set(thisKeys[i],thisMap[thisKeys[i]]);
            }
        }
        return ret;
    }
};