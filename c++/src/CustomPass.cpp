#include "llvm/Pass.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "NullPointerDetector.cpp"


using namespace llvm;

namespace {

    struct CustomPass : public BasicBlockPass {

        static char ID;

        CustomPass() : BasicBlockPass(ID) {}

        virtual bool runOnBasicBlock(BasicBlock& block) {

            NullPointerMap nullPointerMap;
            NullPointerDetector::detect(block, nullPointerMap);

            return false;
        }
    };
}

char CustomPass::ID = 0;

static void registerCustomPass(const PassManagerBuilder&, legacy::PassManagerBase& PM) {
    PM.add(new CustomPass());
}

static RegisterStandardPasses RegisterMyPass(
        PassManagerBuilder::EP_EarlyAsPossible,
        registerCustomPass
);
