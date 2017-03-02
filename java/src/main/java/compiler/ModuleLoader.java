package compiler;

import org.bytedeco.javacpp.*;
import static org.bytedeco.javacpp.LLVM.*;

public class ModuleLoader {

    /**
     * Absolute path to bytecode file
     *
     * @param path
     * @return
     */
    public static LLVMModuleRef loadModuleFromFile(String path) {

        LLVMMemoryBufferRef module = new LLVMMemoryBufferRef();
        BytePointer message = new BytePointer((Pointer) null);
        int hasError = LLVMCreateMemoryBufferWithContentsOfFile(new BytePointer(path), module, message);

        if (hasError == 1) {
            throw new RuntimeException("Could not load bitcode file");
        }

        LLVMModuleRef moduleRef = new LLVMModuleRef();
        hasError = LLVMParseBitcode(module, moduleRef, message);

        if (hasError == 1) {
            throw new RuntimeException("Could not load bitcode file");
        }

        return moduleRef;
    }
}
