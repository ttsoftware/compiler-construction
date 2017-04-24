#include "llvm/Pass.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <list>
#include <string.h>
#include <iostream>

using namespace llvm;

namespace {

//    class CFG {
//
//    public:
//
//        CFGNode entry;
//
//    };

    class CFGNode {

    public:

        Value* inst;

        std::list<CFGNode> incoming;
        std::list<CFGNode> outgoing;


        CFGNode() {
            this->inst = NULL;
        }

        void setInst(Value* v) {
            this->inst = v;
        }

        Value* getInst() {
            return this->inst;
        }

        void dump() {
            this->inst->dump();
        }
    };


    struct CFGPass : public FunctionPass {

        static char ID;



        CFGPass() : FunctionPass(ID) {}

        virtual bool runOnFunction(Function& function) {

            CFGNode currentNode;

            for (auto& block : function) {

                // each instruction
                for (auto& instruction : block) {

                    if (auto* branchInst = dyn_cast<BranchInst>(&instruction)) {

                        CFGNode newNode;
                        newNode.setInst(branchInst);
                        currentNode.outgoing.push_front(newNode);
                    }
                }
            }

            for (std::list<CFGNode>::iterator it = currentNode.outgoing.begin(); it != currentNode.outgoing.end(); it++)
                it->dump();

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
