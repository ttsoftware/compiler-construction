package compiler.example;// General stuff

import org.bytedeco.javacpp.*;

// Headers required by LLVM
import static org.bytedeco.javacpp.LLVM.*;

public class Fac {

    public static void main(String[] args) {

        BytePointer error = new BytePointer((Pointer) null); // Used to retrieve messages from functions

        // Initialize compiler
        LLVMLinkInMCJIT();
        LLVMInitializeNativeAsmPrinter();
        LLVMInitializeNativeAsmParser();
        LLVMInitializeNativeDisassembler();
        LLVMInitializeNativeTarget();

        // Create function module
        LLVMModuleRef mod = LLVMModuleCreateWithName("fac_module");
        // Function has a list of arguments of type int32
        LLVMTypeRef[] fac_args = {LLVMInt32Type()};
        // Function fac has 1 parameter, returns int32 and takes 1 int32
        LLVMValueRef fac = LLVMAddFunction(mod, "fac", LLVMFunctionType(LLVMInt32Type(), fac_args[0], 1, 0));
        // Set call convention? Don't know what that is
        LLVMSetFunctionCallConv(fac, LLVMCCallConv);
        // The value of the input parameter
        LLVMValueRef n = LLVMGetParam(fac, 0);

        // Define function blocks
        // The if statemnet block
        LLVMBasicBlockRef entry = LLVMAppendBasicBlock(fac, "entry");
        // The true block of the if statement
        LLVMBasicBlockRef iftrue = LLVMAppendBasicBlock(fac, "iftrue");
        // The false block of the if statement
        LLVMBasicBlockRef iffalse = LLVMAppendBasicBlock(fac, "iffalse");
        // The end block of the function
        LLVMBasicBlockRef end = LLVMAppendBasicBlock(fac, "end");

        // The LLVM builder
        LLVMBuilderRef builder = LLVMCreateBuilder();

        // TODO: Why do we position builder at the end? What does this do?
        LLVMPositionBuilderAtEnd(builder, entry);
        /*
         * The if statement - it is an ifstatement of type ICMP (Integer Compare)
         * We compare n with the int32 constant '0'
         */
        LLVMValueRef If = LLVMBuildICmp(
                builder,
                LLVMIntEQ,
                n,
                LLVMConstInt(LLVMInt32Type(), 0, 0),
                "n == 0"
        );
        // Insert condition flow - goto iftrue if n == 0, goto iffalse if n != 0
        LLVMBuildCondBr(builder, If, iftrue, iffalse);

        // Position builder at iftrue block
        LLVMPositionBuilderAtEnd(builder, iftrue);

        // Return 1 if n == 0
        LLVMValueRef res_iftrue = LLVMConstInt(LLVMInt32Type(), 1, 0);
        // TODO: What is br?
        LLVMBuildBr(builder, end);

        // Position builder at iffalse block
        LLVMPositionBuilderAtEnd(builder, iffalse);
        // Create n = n-1 assignment
        LLVMValueRef n_minus = LLVMBuildSub(builder, n, LLVMConstInt(LLVMInt32Type(), 1, 0), "n - 1");
        // Create call arguments for recursively calling fac
        LLVMValueRef[] call_fac_args = {n_minus};
        // Recursively call fac with n-1
        LLVMValueRef call_fac = LLVMBuildCall(
                builder,
                fac,
                new PointerPointer(call_fac_args),
                1,
                "fac(n - 1)"
        );

        // Multiply result of fac(n-1) with n
        LLVMValueRef res_iffalse = LLVMBuildMul(builder, n, call_fac, "n * fac(n - 1)");
        LLVMBuildBr(builder, end);

        // Position builder at end block
        LLVMPositionBuilderAtEnd(builder, end);

        // Collapse branch from iftrue and iffalse blocks
        // The Phi block decides which result-value we should use in the end block,
        // depending on which block we came from.
        LLVMValueRef res = LLVMBuildPhi(builder, LLVMInt32Type(), "result");
        LLVMValueRef[] phi_vals = {res_iftrue, res_iffalse};
        LLVMBasicBlockRef[] phi_blocks = {iftrue, iffalse};
        LLVMAddIncoming(res, new PointerPointer(phi_vals), new PointerPointer(phi_blocks), 2);
        LLVMBuildRet(builder, res);

        // TODO: What errors can occur?
        LLVMVerifyModule(mod, LLVMAbortProcessAction, error);
        LLVMDisposeMessage(error); // Handler == LLVMAbortProcessAction -> No need to check errors

        // Print errors in JIT
        LLVMExecutionEngineRef engine = new LLVMExecutionEngineRef();
        if (LLVMCreateJITCompilerForModule(engine, mod, 2, error) != 0) {
            System.err.println(error.getString());
            LLVMDisposeMessage(error);
            System.exit(-1);
        }

        // TODO: Lot of compile configuration?
        LLVMPassManagerRef pass = LLVMCreatePassManager();
        LLVMAddConstantPropagationPass(pass);
        LLVMAddInstructionCombiningPass(pass);
        LLVMAddPromoteMemoryToRegisterPass(pass);
        // LLVMAddDemoteMemoryToRegisterPass(pass); // Demotes every possible value to memory
        LLVMAddGVNPass(pass);
        LLVMAddCFGSimplificationPass(pass);
        LLVMRunPassManager(pass, mod);
        LLVMDumpModule(mod);

        // Execute fac function with exec_args
        LLVMGenericValueRef exec_args = LLVMCreateGenericValueOfInt(LLVMInt32Type(), 10, 0);
        LLVMGenericValueRef exec_res = LLVMRunFunction(engine, fac, 1, exec_args);
        System.err.println();
        System.err.println("; Running fac(10) with JIT...");
        System.err.println("; Result: " + LLVMGenericValueToInt(exec_res, 0));

        LLVMDisposePassManager(pass);
        LLVMDisposeBuilder(builder);
        LLVMDisposeExecutionEngine(engine);
    }
}