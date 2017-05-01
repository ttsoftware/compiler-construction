#include "llvm/Pass.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <list>
#include <unordered_map>
#include <queue>
#include <iostream>
#include "llvm/Analysis/CFG.h"
#include "NullPointerMap.cpp"

using namespace llvm;

namespace {

    void processInstruction(Instruction instruction,NullPointerMap* currentKnowledge) {
        // TODO: FIX SO IT CAN WORK ON WHAT COMES OUT OF THE AUTO LOOP!
    };
/*
    NullPointerMap processBlock(BasicBlock* block,NullPointerMap currentKnowledge) {

//        errs() << *block;

        for (Instruction& instruction : *block) {

            // check if an allocation instruction
            if (auto* allocaInstruction = dyn_cast<AllocaInst>(&instruction)) {

                // if the instruction is allocating a pointer, it is null
                bool isPointer =
                        allocaInstruction->getAllocatedType()->getTypeID() == llvm::Type::TypeID::PointerTyID;
                currentKnowledge.set(allocaInstruction, unknown, isPointer);
            }

            // check if a store instruction
            if (auto* storeInstruction = dyn_cast<StoreInst>(&instruction)) {
//            errs() << *storeInstruction << "STORE INSTRUCTION\n";
                // cast operand to constant. _Why_?
                if (auto* nullStore = dyn_cast<Constant>(storeInstruction->getOperand(0))) {
//                            errs() << *nullStore << "     CONSTANT\n";
                    // the instruction is storing null
                    if (nullStore->isNullValue()) {
//                            errs() << *nullStore << "     is NULL\n";

                        currentKnowledge.set(
                                storeInstruction->getOperand(1),
                                nullPointerPointer
                        );

                    } else {
                        errs() << *storeInstruction->getOperand(0) << " OPERAND0\n";
                        errs() << *storeInstruction->getOperand(1) << " OPERAND1\n";
                        currentKnowledge.set(
                                storeInstruction->getOperand(1),
                                storeInstruction->getOperand(0)
                        );
                    }
                }
                else {
                    // if not a constant, we want pointer reference
//                            errs() << "setting " << *(storeInstruction->getOperand(1)) << "\n";
//                            errs() << "to " << currentKnowledge.get((storeInstruction->getOperand(0)))->getVar() << "\n";
                    if (currentKnowledge.get((storeInstruction->getOperand(0)))->getVar() == isNull) {
                        currentKnowledge.set(
                                storeInstruction->getOperand(1),
                                nullPointerPointer
                        );
                    } else {
                        currentKnowledge.set(
                                storeInstruction->getOperand(1),
                                currentKnowledge.get((storeInstruction->getOperand(0)))->getVar()
                        );
                    }

                }
            }

            // check if a load instruction
            if (auto* loadInstruction = dyn_cast<LoadInst>(&instruction)) {

//                        errs() << *loadInstruction << "\n";
//                        errs() << *loadInstruction->getOperand(0) << "\n";


                if(currentKnowledge.entryIsNull(loadInstruction->getOperand(0))) {
                    errs() << "FAILURE:\n";
                    errs() << "Tried to dereference a variable: " << *loadInstruction->getOperand(0) << "\n";
                    errs() << "On the following instruction: " << *loadInstruction << "\n";
                    errs() << "But " << *loadInstruction->getOperand(0) << " is null!\n";
                    currentKnowledge.set(loadInstruction,isNull,false);
                } else if (currentKnowledge.get(loadInstruction->getOperand(0))->getVar() == nullPointerPointer) {
//                            currentKnowledge.set(loadInstruction,isNull);
//                            errs() << "Setting: " << *loadInstruction << "\n";
//                            errs() << "To: " << "isNull" << "\n";

                    currentKnowledge.set(loadInstruction,isNull,false);
                } else {
//                            currentKnowledge.set(loadInstruction,currentKnowledge.get(loadInstruction->getOperand(0))->getVar());
//                            errs() << "Setting: " << *loadInstruction << "\n";
//                            errs() << "To: " << *currentKnowledge.get(loadInstruction->getOperand(0))->getVar() << "\n";
                    currentKnowledge.set(loadInstruction,currentKnowledge.get(loadInstruction->getOperand(0))->getVar(),false);
                };
            }
        }

        return currentKnowledge;
    }
*/
    struct worklistElement {
        BasicBlock* bb;
        NullPointerMap knowledge;
    };

//    bool nullPointerMapCompare(NullPointerMap m1, NullPointerMap m2) {
//        auto innerMap1 = m1.getInnerMap();
//        auto innerMap2 = m2.getInnerMap();
//        for (auto kv : innerMap1) {
//            kv.first
//        }
//    }

    struct CFGPass : public FunctionPass {

        static char ID;
        CFGPass() : FunctionPass(ID) {}

        virtual bool runOnFunction(Function& function) {
/*
            NullPointerMap myMap;

//            errs() << "" << function.getName() << " body:\n";
//            function.dump();

            std::queue<worklistElement> worklist;


            BasicBlock* entryBlock = &function.getEntryBlock();

            worklistElement elem;
            elem.bb = entryBlock;
            elem.knowledge = myMap;

            worklist.push(elem);

            while (worklist.size() > 0) {
                worklistElement currentElem = worklist.front();
                worklist.pop();
                NullPointerMap newKnowledge;
                newKnowledge = processBlock(currentElem.bb,currentElem.knowledge);

                auto innerMapOldKnowledge = currentElem.knowledge.getInnerMap();
                auto innerMapNewKnowledge = newKnowledge.getInnerMap();
                for (auto kv : innerMapOldKnowledge) {
                    if (innerMapNewKnowledge.find(kv.first) != innerMapNewKnowledge.end()) {
                        // Old key exists in new knowledge
//                        if (innerMapNewKnowledge[kv.first])
                    }

                }

                //if( newKnowledge > currentElem.knowledge) {
                //  Add successor of  bb to worklist.
                //}


            }

//            errs() << "" << *entryBlock << " ENTRY BLOCK";
*/
            return false;
        }

    };


}

char CFGPass::ID = 1;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerCustomPass(const PassManagerBuilder&, legacy::PassManagerBase& PM) {
    PM.add(new CFGPass());
}

static RegisterStandardPasses RegisterMyPass(
        PassManagerBuilder::EP_EarlyAsPossible,
        registerCustomPass
);
