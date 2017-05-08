#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>
#include <vector>
#include "llvm/Pass.h"
#include "NullPointerMap.cpp"


using namespace llvm;

class NullPointerDetector {

public:

    /**
     * Return modified nullPointerMap after passing block
     *
     * @param block
     * @param nullPointerMap
     * @return
     */
    static NullPointerMap detect(BasicBlock& block, NullPointerMap nullPointerMap) {

        // each instruction
        for (auto& instruction : block) {

            /*errs() << "INST: " << instruction << "\n";*/

            // check if an allocation instruction
            if (auto* allocaInstruction = dyn_cast<AllocaInst>(&instruction)) {

                // if the instruction is allocating a pointer, it is null
                bool isPointer =
                        allocaInstruction->getAllocatedType()->getTypeID() == llvm::Type::TypeID::PointerTyID;
                if (isPointer) {
                    nullPointerMap.set(allocaInstruction, (Value*) Enums::LatticeValue::UNKNOWN, true);
                } else {
                    // Add allocainstruction to the map, but set it to a value that indicates that it's not a pointer?.
                    nullPointerMap.set(allocaInstruction, (Value*) Enums::LatticeValue::UNKNOWN, false);
                }
            }

            // check if a store instruction
            if (auto* storeInstruction = dyn_cast<StoreInst>(&instruction)) {
                // cast operand to constant. _Why_?
                if (auto* nullStore = dyn_cast<Constant>(storeInstruction->getOperand(0))) {
                    // the instruction is storing null
                    if (nullStore->isNullValue()) {
                        nullPointerMap.set(
                                storeInstruction->getOperand(1),
                                (Value*) Enums::LatticeValue::NULL_POINTER
                        );

                    } else {
                        nullPointerMap.set(
                                storeInstruction->getOperand(1),
                                storeInstruction->getOperand(0)
                        );
                    }
                } else {
                    // TODO: Why does this cause segmentation fault on second iteration?
                    // Guard against nulls
                    if (nullPointerMap.get((storeInstruction->getOperand(0))) != NULL) {

                        if (nullPointerMap.get((storeInstruction->getOperand(0)))->getVar() ==
                            (Value*) Enums::LatticeValue::IS_NULL) {
                            nullPointerMap.set(
                                    storeInstruction->getOperand(1),
                                    (Value*) Enums::LatticeValue::NULL_POINTER
                            );
                        } else {
                            nullPointerMap.set(
                                    storeInstruction->getOperand(1),
                                    nullPointerMap.get((storeInstruction->getOperand(0)))->getVar()
                            );
                        }
                    }
                }
            }

            // check if a load instruction
            if (auto* loadInstruction = dyn_cast<LoadInst>(&instruction)) {

                if (nullPointerMap.entryisMaybeNull(loadInstruction->getOperand(0))) {
                    errs() << "WARNING:\n";
                    errs() << "Dereferencing a variable: " << *loadInstruction->getOperand(0) << "\n";
                    errs() << "On the following instruction: " << *loadInstruction << "\n";
                    errs() << "MIGHT result in null pointer exception.\n";
                    nullPointerMap.set(loadInstruction, (Value*) Enums::LatticeValue::MAYBE_NULL, false);
                }

                // Guard against nulls
                if (nullPointerMap.get(loadInstruction->getOperand(0)) != NULL) {
                    if (nullPointerMap.entryIsNull(loadInstruction->getOperand(0))) {
                        errs() << "FAILURE:\n";
                        errs() << "Tried to dereference a variable: " << *loadInstruction->getOperand(0) << "\n";
                        errs() << "On the following instruction: " << *loadInstruction << "\n";
                        errs() << "But " << *loadInstruction->getOperand(0) << " is null!\n";
                        nullPointerMap.set(loadInstruction, (Value*) Enums::LatticeValue::IS_NULL, false);
                    } else if (nullPointerMap.get(loadInstruction->getOperand(0))->getVar() ==
                               (Value*) Enums::LatticeValue::NULL_POINTER) {
                        nullPointerMap.set(loadInstruction, (Value*) Enums::LatticeValue::IS_NULL, false);
                    } else {
                        nullPointerMap.set(loadInstruction,
                                           nullPointerMap.get(loadInstruction->getOperand(0))->getVar(),
                                           false);
                    };
                }
            }
        }

        return nullPointerMap;
    }

    /**
     * Return a list of keys which have changed in newNpm relative to oldNpm
     *
     * @param newNpm
     * @param oldNpm
     * @return
     */
    static std::vector<Value*> compare(NullPointerMap newNpm, NullPointerMap oldNpm) {

        std::vector<Value*> differences{};

        std::unordered_map<Value*, VariableEntry*> newMap = newNpm.getInnerMap();
        std::unordered_map<Value*, VariableEntry*> oldMap = oldNpm.getInnerMap();
        std::vector<Value*> newKeys = newNpm.getKeys();
        std::vector<Value*> oldKeys = oldNpm.getKeys();

        // check if any of the old keys has changed
        for (int i = 0; i < oldKeys.size(); i++) {
            bool hasKey = false;
            for (int j = 0; j < newKeys.size(); j++) {
                hasKey = hasKey || (oldKeys[i] == newKeys[j]);
            }

            if (hasKey) {
                VariableEntry newVarEntry = *(newMap.at(oldKeys[i]));
                if (!newVarEntry.equals(*(oldMap.at(oldKeys[i])))) {
                    // newkey has changed value
                    differences.push_back(oldKeys[i]);
                }
            }
        }

        // check if there are new keys
        for (int i = 0; i < newKeys.size(); i++) {
            bool hasKey = false;
            for (int j = 0; j < oldKeys.size(); j++) {
                hasKey = hasKey || (newKeys[i] == oldKeys[j]);
            }

            if (!hasKey) {
                // new key does not exist in old keyset
                differences.push_back(newKeys[i]);
            }
        }

        return differences;
    }
};