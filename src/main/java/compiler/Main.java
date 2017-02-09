package compiler;

import org.bytedeco.javacpp.LLVM;

import static org.bytedeco.javacpp.LLVM.*;

public class Main {

    public static void main(String[] args) {

        LLVMInitializeNativeAsmPrinter();
        LLVMInitializeNativeAsmParser();
        LLVMInitializeNativeDisassembler();
        LLVMInitializeNativeTarget();

        LLVMModuleRef moduleRef = ModuleLoader.loadModuleFromFile("../../c/hello.bc");

        LLVMDumpModule(moduleRef);
    }
}
