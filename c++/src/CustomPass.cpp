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


    Value* nullPointerPointer = (Value*) 4095;
    Value* isNull = (Value*) 52428;
    Value* unknown = (Value*) 999999;


    class VariableEntry {
        bool isPointer;
        Value* self;
        Value* var;

    public:
        VariableEntry(Value* var,bool isPointer) {
            this->isPointer = isPointer;
            this->var = var;
        }

        Value* getVar() {
            return this->var;
        }

        bool isPtr() {
            return this->isPointer;
        }

    };



    class NullPointerMap {

        std::unordered_map<Value*,VariableEntry*> map;


    public:
        void set(Value* key,Value* val, bool isPointer) {
            VariableEntry* ventry = new VariableEntry(val,isPointer);
            map[key] = ventry;
        }

        void set(Value* key,Value* val) {
            bool isPointer = map[key]->isPtr();
            VariableEntry* ventry = new VariableEntry(val,isPointer);
            map[key] = ventry;
        }

        VariableEntry* get(Value* var) {
            return map[var];
        }

        bool entryIsPointer(Value* var) {
            return map[var]->isPtr();
        }

        bool entryIsNull(Value* var) {
            return map[var]->getVar() == isNull;
        }
    };

    struct CustomPass : public FunctionPass {

        static char ID;
        // This is our "sentinel value".


        CustomPass() : FunctionPass(ID) {}

        virtual bool runOnFunction(Function& function) {

            std::unordered_map<Value*, Value*> nullPointerMap;
            std::unordered_map<Value*, bool> isAPointerMap;

            NullPointerMap myMap;

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
                            myMap.set(allocaInstruction,unknown,true);
                        } else {
                            // Add allocainstruction to the map, but set it to a value that indicates that it's not a pointer?.
                            myMap.set(allocaInstruction,unknown,false);
                        }
                    }

                    // check if a store instruction
                    if (auto* storeInstruction = dyn_cast<StoreInst>(&instruction)) {

                        // cast operand to constant. _Why_?
                        if (auto* nullStore = dyn_cast<Constant>(storeInstruction->getOperand(0))) {
                            errs() << *nullStore << "     CONSTANT\n";
                            // the instruction is storing null
                            if (nullStore->isNullValue()) {
                                errs() << *nullStore << " VAR IS NULL\n";
                                myMap.set(
                                        storeInstruction->getOperand(1),
                                        nullPointerPointer
                                );

                            } else {
                                myMap.set(
                                        storeInstruction->getOperand(1),
                                        storeInstruction->getOperand(0)
                                );
                                //nullPointerMap[storeInstruction->getOperand(1)] = ;
                            }
                        }
                        else {
                            // if not a constant, we want pointer reference
                            myMap.set(
                                    storeInstruction->getOperand(1),
                                    storeInstruction->getOperand(0)
                            );
                        }
                    }

                    // check if a load instruction
                    if (auto* loadInstruction = dyn_cast<LoadInst>(&instruction)) {

                        errs() << *loadInstruction << "\n";
                        errs() << *loadInstruction->getOperand(0) << "\n";
                        errs() << nullPointerMap[loadInstruction->getOperand(0)] << "\n";

                        if(myMap.entryIsNull(loadInstruction->getOperand(0))) {
                            errs() << "FAILURE\n";
                        } else if (myMap.get(loadInstruction->getOperand(0))->getVar() == nullPointerPointer) {
                            myMap.set(loadInstruction,isNull);
                        } else {
                            myMap.set(loadInstruction,myMap.get(loadInstruction->getOperand(0))->getVar());
                        };
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
