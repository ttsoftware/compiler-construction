#include "llvm/Pass.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <unordered_map>
#include <string.h>

using namespace llvm;




namespace {


    Value* nullPointerPointer = (Value*) 4095;
    Value* isNull = (Value*) 52428;
    Value* unknown = (Value*) 999999;

    class MyLogger {
        bool logIsOn;

    public:

        MyLogger() {
            this->logIsOn = true;
        }

        void log(std::string msg) {
            if (this->logIsOn) {
                errs() << msg << "\n";
            }
            return;
        }

        void toggleLog(bool logEnabled) {
            this->logIsOn = logEnabled;
            return;
        }
    };


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





        CustomPass() : FunctionPass(ID) {}

        virtual bool runOnFunction(Function& function) {

            MyLogger logger;
            logger.toggleLog(false);


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
//                            errs() << *nullStore << "     CONSTANT\n";
                            // the instruction is storing null
                            if (nullStore->isNullValue()) {
//                            errs() << *nullStore << "     is NULL\n";

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
                        }
                        else {
                            // if not a constant, we want pointer reference
//                            errs() << "setting " << *(storeInstruction->getOperand(1)) << "\n";
//                            errs() << "to " << myMap.get((storeInstruction->getOperand(0)))->getVar() << "\n";
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
                    if (auto* loadInstruction = dyn_cast<LoadInst>(&instruction)) {

//                        errs() << *loadInstruction << "\n";
//                        errs() << *loadInstruction->getOperand(0) << "\n";


                        if(myMap.entryIsNull(loadInstruction->getOperand(0))) {
                            errs() << "FAILURE:\n";
                            errs() << "Tried to dereference a variable: " << *loadInstruction->getOperand(0) << "\n";
                            errs() << "On the following instruction: " << *loadInstruction << "\n";
                            errs() << "But " << *loadInstruction->getOperand(0) << " is null!\n";
                            myMap.set(loadInstruction,isNull,false);
                        } else if (myMap.get(loadInstruction->getOperand(0))->getVar() == nullPointerPointer) {
//                            myMap.set(loadInstruction,isNull);
//                            errs() << "Setting: " << *loadInstruction << "\n";
//                            errs() << "To: " << "isNull" << "\n";

                            myMap.set(loadInstruction,isNull,false);
                        } else {
//                            myMap.set(loadInstruction,myMap.get(loadInstruction->getOperand(0))->getVar());
//                            errs() << "Setting: " << *loadInstruction << "\n";
//                            errs() << "To: " << *myMap.get(loadInstruction->getOperand(0))->getVar() << "\n";
                            myMap.set(loadInstruction,myMap.get(loadInstruction->getOperand(0))->getVar(),false);
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
