package compiler;

import org.bytedeco.javacpp.BytePointer;
import org.bytedeco.javacpp.presets.LLVM;

import java.io.File;
import java.util.HashMap;

import static org.bytedeco.javacpp.LLVM.*;

public class Main {

    public static void main(String[] args) {

        String filePath = new File("src/main/c/hello.bc").getAbsolutePath();

        LLVMInitializeNativeAsmPrinter();
        LLVMInitializeNativeAsmParser();
        LLVMInitializeNativeDisassembler();
        LLVMInitializeNativeTarget();

        HashMap<String,Boolean> nullPointerMap = new HashMap<>();

        LLVMModuleRef module = ModuleLoader.loadModuleFromFile(filePath);
        for(LLVMValueRef func = LLVMGetFirstFunction(module); func != null; func = LLVMGetNextFunction(func)){
            LLVMDumpValue(func);
            for(LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(func); bb != null; bb = LLVMGetNextBasicBlock(bb)){
                for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction != null; instruction = LLVMGetNextInstruction(instruction)){

                    String variableName = LLVMGetValueName(instruction).getString();

                    // On alloca, by default the variable is set to null
                    // We do not currently differ between pointer types and non-pointer types.
                    if (LLVMIsAAllocaInst(instruction) != null) {

                        LLVMDumpValue(LLVMGetOperand(instruction,0));

                        if (!variableName.equals("")) {
                            nullPointerMap.put(variableName, true);
                        }
                    }

                    // On a store instruction, we use LLVM's own IsNull function for now.
                    if (LLVMIsAStoreInst(instruction) != null) {

                        if (LLVMIsNull(LLVMGetOperand(instruction,0)) == 1) {
                            nullPointerMap.put(variableName, true);
                        } else {
                            nullPointerMap.put(variableName, false);
                        }
                    }

                    // Load instructions that use a variable that is null
                    // Generates a null pointer exception.
                    if (LLVMIsALoadInst(instruction) != null) {
                        if (nullPointerMap.containsKey(variableName) && nullPointerMap.get(variableName)) {

                            LLVMDumpValue(LLVMGetOperand(instruction,0));
                            System.out.println(LLVMIsNull(LLVMGetOperand(instruction,0)));

                            throw new NullPointerException("You dun goofd son");
                        }
                    }
                    if (LLVMIsACallInst(instruction) != null) {
                        // Not doing anything here yet.
                    }
                }
            }
        }
    }
}
