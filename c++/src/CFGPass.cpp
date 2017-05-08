#include "llvm/Pass.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <list>
#include <unordered_map>
#include <queue>
#include <iostream>
#include "llvm/Analysis/CFG.h"
#include "NullPointerDetector.cpp"


using namespace llvm;

namespace {

    struct WorklistElement {
        BasicBlock& bb;
        NullPointerMap knowledge;
    };

    struct CFGPass : public FunctionPass {

        static char ID;

        CFGPass() : FunctionPass(ID) {}

        virtual bool runOnFunction(Function& function) {

            function.viewCFG();

            // Init the worklist
            NullPointerMap knowledge;
            std::queue<WorklistElement> worklist;
            std::unordered_map<BasicBlock*, NullPointerMap> blockKnowledge;

            worklist.push((WorklistElement) {function.getEntryBlock(), knowledge});

            // init the blockKnowledge map
            for (auto& block : function) {
                NullPointerMap initKnowledge;
                blockKnowledge[&block] = initKnowledge;
            }

            // iterate through successor tree
            while (!worklist.empty()) {

                BasicBlock& currentBlock = worklist.front().bb;
                NullPointerMap currentKnowledge = worklist.front().knowledge;
                worklist.pop();
                errs() << "PROCESSING BLOCK: " << currentBlock << "\n";

                /*NullPointerMap newKnowledge = NullPointerDetector::detect(currentBlock, currentKnowledge);*/


                NullPointerMap oldKnowledge = blockKnowledge[&currentBlock];
                NullPointerMap mergedKnowledge = oldKnowledge.merge(currentKnowledge);
                NullPointerMap newKnowledge = NullPointerDetector::detect(currentBlock, mergedKnowledge);
                // DETECT @ CURRENTKNOWLEDGE
                // MERGE RESULT
                // DIFFERENCE? PUSH SUCCESSORS : do nothing.
                //errs() << mergedKeys.size() << " keys\n";

                /*for (int i = 0; i < mergedKeys.size(); i++) {
                    errs() << " >> " << *(mergedKeys)[i] << "\n";
                }*/

                // what to do with newKnowledge?

                //errs() << newKeys.size() << " keys\n";

                /*for (int i = 0; i < newKeys.size(); i++) {
                    errs() << " >> " << *(newKeys)[i] << "\n";
                }*/

                // update the blockMap ?
                blockKnowledge[&currentBlock] = newKnowledge;

                std::vector<Value*> mergedKeys = mergedKnowledge.getKeys();
                std::vector<Value*> oldKeys = oldKnowledge.getKeys();
                /*for (int i = 0; i < oldKeys.size(); i++) {
                    errs() << " NEWKEY>> " << *(oldKeys)[i] << "\n";
                }
                for (int i = 0; i < mergedKeys.size(); i++) {
                    errs() << " MERGEDKEY>> " << *(mergedKeys)[i] << "\n";
                }*/
                // if merged knowledge is different, we add to stack ?

                std::vector<Value*> differences = NullPointerDetector::compare(newKnowledge, oldKnowledge);

                errs() << differences.size() << " differences\n";

                if (differences.size() > 0) {
                    // there was at least one difference - push all successors to worklist?
                    TerminatorInst* terminator = currentBlock.getTerminator();

                    for (unsigned i = 0, NSucc = terminator->getNumSuccessors(); i < NSucc; ++i) {
                        BasicBlock* successorBlock = terminator->getSuccessor(i);

                        // add new element to worklist - and append newKnowledge?
                        worklist.push((WorklistElement) {*successorBlock, newKnowledge});

                        errs() << "BLCK: " << *successorBlock << "\n";
                    }
                }
            }

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
