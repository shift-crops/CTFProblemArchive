#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

struct SSPPass : public FunctionPass {
	static char ID;
	SSPPass() : FunctionPass(ID) {}

	virtual bool runOnFunction(Function &F);
};

char SSPPass::ID = 0;

bool SSPPass::runOnFunction(Function &F){
	inst_iterator inst_it = inst_begin(F);

	Type* i64aTy = ArrayType::get(IntegerType::getInt64Ty(F.getContext()), 2);
	new AllocaInst(i64aTy, 0, NULL, "canary", &*inst_it);

	Type* vTy = Type::getVoidTy(F.getContext());
	FunctionType* FTy = FunctionType::get(vTy, false);

	StringRef Constraints = "~{dirflag},~{fpsr},~{flags}";
	StringRef Prologue = 
		"rdrand %rax\n"
		"and $$0xffffffffffffff00, %rax\n"
		"mov %rax,-0x10(%rbp)\n"
		"xor %fs:0x28,%rax\n"
		"mov %rax,-0x8(%rbp)\n";
	CallInst::Create(InlineAsm::get(FTy, Prologue, Constraints, true), "prologue", &*inst_it);

	inst_it = inst_end(F);
	StringRef Epilogue = 
		"mov -0x10(%rbp), %rdx\n"
		"xor -0x8(%rbp), %rdx\n"
		"xor %fs:0x28, %rdx\n"
		".byte 0x74,0x05\n"
		"callq __stack_chk_fail@plt\n";
	CallInst::Create(InlineAsm::get(FTy, Epilogue, Constraints, true), "epilogue", &*(--inst_it));

	return true;
}

static void registerInstructionPass(const PassManagerBuilder &, legacy::PassManagerBase &PM){
  PM.add(new SSPPass());
}
static RegisterStandardPasses RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerInstructionPass);
