package compiler;

import java.io.File;

import static org.bytedeco.javacpp.LLVM.*;

public class Main {

    public static void main(String[] args) {

        String filePath = new File("src/main/c/hello.bc").getAbsolutePath();

        LLVMInitializeNativeAsmPrinter();
        LLVMInitializeNativeAsmParser();
        LLVMInitializeNativeDisassembler();
        LLVMInitializeNativeTarget();

        LLVMModuleRef moduleRef = ModuleLoader.loadModuleFromFile(filePath);

        LLVMDumpModule(moduleRef);

        // count functions
        // count basic blocks per function
        LLVMValueRef nextFunction = LLVMGetFirstFunction(moduleRef);

        int functionCount = 0;
        while (nextFunction != null) {

            // create block array of size basicBlockCount
            int basicBlockCount = LLVMCountBasicBlocks(nextFunction);
            LLVMBasicBlockRef basicBlocks = new LLVMBasicBlockRef();
            basicBlocks.capacity(basicBlockCount);

            LLVMGetBasicBlocks(nextFunction, basicBlocks);
            System.out.println(basicBlocks);

            // get instructions per block
            LLVMValueRef nextInstruction = LLVMGetFirstInstruction(basicBlocks);
            int instructionCount = 0;
            while (nextInstruction != null) {
                System.out.println(nextInstruction);
                nextInstruction = LLVMGetNextInstruction(nextInstruction);
                instructionCount++;
            }

            System.out.println(instructionCount);

            nextFunction = LLVMGetNextFunction(nextFunction);
            functionCount++;
        }

        System.out.println(functionCount);
    }
}
