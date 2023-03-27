// llvm-meta-meetup library
// Copyright (c) 2023 llvm-meta-meetup authors
// Distributed under the BSD 3-Clause License license.
// (See accompanying file LICENSE)
// SPDX-License-Identifier: BSD-3-Clause

namespace llvm {
class ModulePass;
}  // namespace llvm

llvm::ModulePass* createMetaPass();
