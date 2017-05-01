#include "llvm/Pass.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "NullPointerMap.cpp"


using namespace llvm;

namespace {

    Value *nullPointerPointer = (Value *) 4095;
    Value *isNull = (Value *) 52428;
    Value *unknown = (Value *) 999999;

    struct CustomPass : public BasicBlockPass {

        static char ID;

        CustomPass() : BasicBlockPass(ID) {}

        virtual bool runOnBasicBlock(BasicBlock &block) {

            NullPointerMap myMap;
            // each instruction
            for (auto &instruction : block) {

                errs() << instruction << "  >>>>>>>instruction\n";

                // check if an allocation instruction
                if (auto *allocaInstruction = dyn_cast<AllocaInst>(&instruction)) {

                    // if the instruction is allocating a pointer, it is null
                    bool isPointer =
                            allocaInstruction->getAllocatedType()->getTypeID() == llvm::Type::TypeID::PointerTyID;
                    if (isPointer) {
                        myMap.set(allocaInstruction, unknown, true);
                    } else {
                        // Add allocainstruction to the map, but set it to a value that indicates that it's not a pointer?.
                        myMap.set(allocaInstruction, unknown, false);
                    }
                }

                // check if a store instruction
                if (auto *storeInstruction = dyn_cast<StoreInst>(&instruction)) {
                    errs() << *storeInstruction << " HALLO\n";
                    // cast operand to constant. _Why_?
                    if (auto *nullStore = dyn_cast<Constant>(storeInstruction->getOperand(0))) {
                        // the instruction is storing null
                        if (nullStore->isNullValue()) {
                            myMap.set(
                                    storeInstruction->getOperand(1),
                                    nullPointerPointer
                            );

                        } else {
                            myMap.set(
                                    storeInstruction->getOperand(1),
                                    storeInstruction->getOperand(0)
                            );
                        }
                    } else {
                        if (myMap.get((storeInstruction->getOperand(0)))->getVar() == isNull) {
                            myMap.set(
                                    storeInstruction->getOperand(1),
                                    nullPointerPointer
                            );
                        } else {
                            myMap.set(
                                    storeInstruction->getOperand(1),
                                    myMap.get((storeInstruction->getOperand(0)))->getVar()
                            );
                        }

                    }
                }

                // check if a load instruction
                if (auto *loadInstruction = dyn_cast<LoadInst>(&instruction)) {

                    if (myMap.entryIsNull(loadInstruction->getOperand(0))) {
                        errs() << "FAILURE:\n";
                        errs() << "Tried to dereference a variable: " << *loadInstruction->getOperand(0) << "\n";
                        errs() << "On the following instruction: " << *loadInstruction << "\n";
                        errs() << "But " << *loadInstruction->getOperand(0) << " is null!\n";
                        myMap.set(loadInstruction, isNull, false);
                    } else if (myMap.get(loadInstruction->getOperand(0))->getVar() == nullPointerPointer) {
                        myMap.set(loadInstruction, isNull, false);
                    } else {
                        myMap.set(loadInstruction, myMap.get(loadInstruction->getOperand(0))->getVar(), false);
                    };
                }
            }
            return false;
        }
    };
}

char CustomPass::ID = 0;

static void registerCustomPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new CustomPass());
}

static RegisterStandardPasses RegisterMyPass(
        PassManagerBuilder::EP_EarlyAsPossible,
        registerCustomPass
);
