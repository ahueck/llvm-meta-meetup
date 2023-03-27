// llvm-meta-meetup library
// Copyright (c) 2023 llvm-meta-meetup authors
// Distributed under the BSD 3-Clause License license.
// (See accompanying file LICENSE)
// SPDX-License-Identifier: BSD-3-Clause

#include "MetaPass.h"

#include "DIVisitor.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace meta {

class MetaPass : public llvm::PassInfoMixin<MetaPass> {
 public:
  llvm::PreservedAnalyses run(llvm::Module&, llvm::ModuleAnalysisManager&);

  bool runOnModule(llvm::Module&);

  static bool runOnFunc(llvm::Function&);
};

class LegacyMetaPass : public llvm::ModulePass {
 private:
  MetaPass pass_impl_;

 public:
  static char ID;  // NOLINT

  LegacyMetaPass() : ModulePass(ID){};

  bool runOnModule(llvm::Module& module) override;

  ~LegacyMetaPass() override = default;
};

llvm::PreservedAnalyses MetaPass::run(llvm::Module& module, llvm::ModuleAnalysisManager&) {
  const auto changed = runOnModule(module);
  return changed ? llvm::PreservedAnalyses::none() : llvm::PreservedAnalyses::all();
}

bool MetaPass::runOnModule(llvm::Module& module) {  // NOLINT
  const auto changed = llvm::count_if(module.functions(), [&](auto& func) { return MetaPass::runOnFunc(func); }) > 1;
  return changed;
}

bool LegacyMetaPass::runOnModule(llvm::Module& module) {
  const auto modified = pass_impl_.runOnModule(module);
  return modified;
}

namespace compat {
template <typename DbgVar>
llvm::Value* get_alloca_for(const DbgVar* dbg_var) {
#if LLVM_VERSION_MAJOR < 13
  return dbg_var->getVariableLocation();
#else
  return dbg_var->getVariableLocationOp(0);
#endif
}
}  // namespace compat

bool MetaPass::runOnFunc(llvm::Function& function) {
  if (function.isDeclaration()) {
    return false;
  }

  llvm::outs() << "Function: " << function.getName() << ":\n";
  llvm::outs() << "-------------------------------------\n";

  const auto find_dbg_variable = [&](AllocaInst* alloca_inst) -> llvm::Optional<DILocalVariable*> {
    llvm::SmallVector<DbgVariableIntrinsic*, 2> dbg_inst_v;
    llvm::findDbgUsers(dbg_inst_v, alloca_inst);
    if (dbg_inst_v.empty()) {
      return llvm::None;
    }
    // Assume only first llvm.dbg intrinsic is interesting:
    auto* dbg = dbg_inst_v.front();
    if (compat::get_alloca_for(dbg) == alloca_inst) {
      return dbg->getVariable();
    }
    return llvm::None;
  };

  util::DIPrinter di_printer(llvm::outs(), function.getParent());

  for (auto& inst : llvm::instructions(function)) {
    if (auto* alloca_inst = dyn_cast<AllocaInst>(&inst)) {
      const auto local_divariable = find_dbg_variable(alloca_inst);
      if (local_divariable) {
        llvm::outs() << *alloca_inst << ":\n\n";
        di_printer.traverseLocalVariable(*local_divariable);
      }
    }
  }

  llvm::outs() << "-------------------------------------\n";

  return false;
}

}  // namespace meta

#define DEBUG_TYPE "meta-pass"

//.....................
// New PM
//.....................
llvm::PassPluginLibraryInfo getMetaPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "meta", LLVM_VERSION_STRING, [](PassBuilder& pass_builder) {
            pass_builder.registerPipelineParsingCallback(
                [](StringRef name, ModulePassManager& module_pm, ArrayRef<PassBuilder::PipelineElement>) {
                  if (name == "meta") {
                    module_pm.addPass(meta::MetaPass());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getMetaPassPluginInfo();
}

//.....................
// Old PM
//.....................
char meta::LegacyMetaPass::ID = 0;  // NOLINT

static RegisterPass<meta::LegacyMetaPass> x("meta", "Meta Pass");  // NOLINT

ModulePass* createMetaPass() {
  return new meta::LegacyMetaPass();
}

extern "C" void AddMetaPass(LLVMPassManagerRef pass_manager) {
  unwrap(pass_manager)->add(createMetaPass());
}
