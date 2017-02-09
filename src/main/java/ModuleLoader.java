
import org.bytedeco.javacpp.*;

// Headers required by LLVM
import static org.bytedeco.javacpp.LLVM.*;

public class ModuleLoader {

    public static LLVMModuleRef loadModuleFromFile(String path) throws Exception{
        LLVMMemoryBufferRef module = new LLVMMemoryBufferRef();
        BytePointer message = new BytePointer((Pointer)null);
        int hasError = LLVMCreateMemoryBufferWithContentsOfFile(new BytePointer(path), module, message);
        if (hasError == 1) {
            throw new Exception("Could not load bitcode file");
        }
        LLVMModuleRef moduleRef = new LLVMModuleRef();
        hasError = LLVMParseBitcode(module, moduleRef, message);
        if (hasError==1) {
            throw new Exception("Could not load bitcode file");
        }
        return moduleRef;
    }

}
