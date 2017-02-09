package compiler;

import org.bytedeco.javacpp.LLVM;

public class Main {

    public static void main(String[] args) {

        LLVM.LLVMModuleRef moduleRef = ModuleLoader.loadModuleFromFile("../../c/hello.bc");
    }
}
