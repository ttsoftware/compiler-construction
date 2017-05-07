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

            errs() << instruction << "  >>>>>>>instruction\n";

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

            // check if a load instruction
            if (auto* loadInstruction = dyn_cast<LoadInst>(&instruction)) {

                if (nullPointerMap.entryisMaybeNull(loadInstruction->getOperand(0))) {
                    errs() << "WARNING:\n";
                    errs() << "Dereferening a variable: " << *loadInstruction->getOperand(0) << "\n";
                    errs() << "On the following instruction: " << *loadInstruction << "\n";
                    errs() << "MIGHT result in null pointer exception.";
                    nullPointerMap.set(loadInstruction, (Value*) Enums::LatticeValue::IS_NULL, false);
                }

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
                    nullPointerMap.set(loadInstruction, nullPointerMap.get(loadInstruction->getOperand(0))->getVar(),
                                       false);
                };
            }
        }

        return nullPointerMap;
    }

/*    static std::vector<Value*> compare(NullPointerMap newNpm, NullPointerMap oldNpm) {
        std::unordered_map newMap = newNpm.getInnerMap();
        std::unordered_map oldMap = oldNpm.getInnerMap();
        std::vector<Value*> newKeys = newNpm.getKeys();
        std::vector<Value*> oldKeys = oldNpm.getKeys();
        // If keys1[i] = null &&
        for (int i = 0; i < oldKeys.size(); ++i) {
            Value* oldKey = oldKeys[i];
            bool hasKey = false;
            for (int j = 0; j < newKeys.size(); ++j) {
                hasKey = hasKey || (oldKey == newKeys[j]);
            }

            if (hasKey) {
                VariableEntry newVarEntry = newMap.at(oldKey);
                if (!newVarEntry.equals(oldMap.at(oldKey))) {

                }
            }
        }
    }*/

};