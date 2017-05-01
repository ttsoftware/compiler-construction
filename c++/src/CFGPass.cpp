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

    struct worklistElement {
        BasicBlock* bb;
        NullPointerMap knowledge;
    };

    struct CFGPass : public FunctionPass {

        static char ID;
        CFGPass() : FunctionPass(ID) {}

        virtual bool runOnFunction(Function& function) {

            function.viewCFG();

            // Stack

            BasicBlock& block = function.getEntryBlock();

            TerminatorInst* terminator = block.getTerminator();

            for (unsigned i = 0, NSucc = terminator->getNumSuccessors(); i < NSucc; ++i) {
                BasicBlock* blockChild= terminator->getSuccessor(i);
                errs() << *blockChild << "\n";
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
