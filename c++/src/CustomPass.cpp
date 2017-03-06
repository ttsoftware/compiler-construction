#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

namespace {
    struct CustomPass : public FunctionPass {
        static char ID;

        CustomPass() : FunctionPass(ID) {}

        virtual bool runOnFunction(Function &F) {
            errs() << "I saw a function called " << F.getName() << "!\n";
            return false;
        }
    };
}

char CustomPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerCustomPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new CustomPass());
}

static RegisterStandardPasses RegisterMyPass(
        PassManagerBuilder::EP_EarlyAsPossible,
        registerCustomPass
);
