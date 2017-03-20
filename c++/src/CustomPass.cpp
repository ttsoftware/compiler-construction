#include "llvm/Pass.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <unordered_map>

using namespace llvm;

namespace {

    struct CustomPass : public FunctionPass {

        static char ID;
        // This is our "sentinel value".
        Value* nullPointerPointer = (Value*) 4095;

        CustomPass() : FunctionPass(ID) {}

        virtual bool runOnFunction(Function& function) {

            std::unordered_map<Value*, Value*> nullPointerMap;

            errs() << "" << function.getName() << " body:\n";
            function.dump();

            // each block
            for (auto& block : function) {

                // each instruction
                for (auto& instruction : block) {

                    // check if an allocation instruction
                    if (auto* allocaInstruction = dyn_cast<AllocaInst>(&instruction)) {

                        // if the instruction is allocating a pointer, it is null
                        bool isPointer =
                                allocaInstruction->getAllocatedType()->getTypeID() == llvm::Type::TypeID::PointerTyID;
                        if (isPointer) {
                            nullPointerMap[allocaInstruction] = nullPointerPointer;
                        }
                    }

                    // check if a store instruction
                    if (auto* storeInstruction = dyn_cast<StoreInst>(&instruction)) {

                        // cast operand to constant. _Why_?
                        if (auto* nullStore = dyn_cast<Constant>(storeInstruction->getOperand(0))) {

                            // the instruction is storing null
                            if (nullStore->isNullValue()) {
                                nullPointerMap[storeInstruction->getOperand(1)] = nullPointerPointer;
                            } else {
                                // If pointer is NOT null, we add it to the nullPointerMap. We are pretty sure this is needed!
                                nullPointerMap[storeInstruction->getOperand(1)] = nullStore;
                            }
                        }
                        else {
                            // if not a constant, we want pointer reference
                            nullPointerMap[storeInstruction->getOperand(1)] = nullPointerMap[storeInstruction->getOperand(0)];
                        }
                    }

                    // check if a load instruction
                    if (auto* loadInstruction = dyn_cast<LoadInst>(&instruction)) {

                        errs() << *loadInstruction << "\n";
                        errs() << *loadInstruction->getOperand(0) << "\n";
                        errs() << nullPointerMap[loadInstruction->getOperand(0)] << "\n";

                        nullPointerMap[loadInstruction] = nullPointerMap[loadInstruction->getOperand(0)];

                        if (nullPointerMap[loadInstruction->getOperand(0)] == nullPointerPointer) {
                            errs() << "Null pointer dereferenced!!! BAD \n";
                        }
                        else {
                            errs() << "Derefenced a legal pointer\n";
                        }
                    }
                }
            }

            return false;
        }
    };
}

char CustomPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerCustomPass(const PassManagerBuilder&, legacy::PassManagerBase& PM) {
    PM.add(new CustomPass());
}

static RegisterStandardPasses RegisterMyPass(
        PassManagerBuilder::EP_EarlyAsPossible,
        registerCustomPass
);
