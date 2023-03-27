// llvm-meta-meetup library
// Copyright (c) 2023 llvm-meta-meetup authors
// Distributed under the BSD 3-Clause License license.
// (See accompanying file LICENSE)
// SPDX-License-Identifier: BSD-3-Clause

#ifndef META_DIVISITOR_H
#define META_DIVISITOR_H

#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Support/Debug.h"

#include <functional>

namespace meta {

namespace visitor {

namespace detail {

template <typename Fn>
class ScopeExit {
 private:
  Fn exit_fn_;

 public:
  explicit ScopeExit(Fn&& exit_fn) : exit_fn_(std::forward<Fn>(exit_fn)) {
  }

  ScopeExit(const ScopeExit&) = delete;

  ScopeExit& operator=(const ScopeExit&) = delete;

  ~ScopeExit() {
    std::invoke(exit_fn_);
  }
};

template <typename Fn>
ScopeExit<Fn> create_scope_exit(Fn&& exit_fn) {
  return ScopeExit<Fn>(std::forward<Fn>(exit_fn));
}

}  // namespace detail

template <typename SubClass>
class DINodeVisitor {
 private:
  llvm::SmallPtrSet<const llvm::DINode*, 8> visited_dinodes_;

  template <typename TySink, typename TySource, typename VisitFn>
  bool invoke_if(VisitFn&& visitor, TySource&& type) {
    if (auto* base_t = llvm::dyn_cast<TySink>(type)) {
      return std::invoke(std::forward<VisitFn>(visitor), *this, base_t);
    }
    return false;
  }

  template <typename T>
  bool invoke_if_any(T&& type) {
    using namespace llvm;
    return invoke_if<DIBasicType>(&DINodeVisitor::traverseBasicType, std::forward<T>(type)) ||
           invoke_if<DIDerivedType>(&DINodeVisitor::traverseDerivedType, std::forward<T>(type)) ||
           invoke_if<DICompositeType>(&DINodeVisitor::traverseCompositeType, std::forward<T>(type)) ||
           invoke_if<DILocalVariable>(&DINodeVisitor::traverseLocalVariable, std::forward<T>(type));
  }

 protected:
  unsigned depth_composite_{0};
  unsigned depth_derived_{0};
  unsigned depth_var_{0};

  [[nodiscard]] inline unsigned depth() const {
    return depth_composite_ + depth_derived_ + depth_var_;
  }

  inline bool visited_node(const llvm::DINode* node) {
    return visited_dinodes_.contains(node);
  }

 public:
  [[nodiscard]] SubClass& get() {
    return static_cast<SubClass&>(*this);
  }

  [[nodiscard]] const SubClass& get() const {
    return static_cast<const SubClass&>(*this);
  }

  bool traverseLocalVariable(const llvm::DIVariable* var) {
    const bool ret = get().visitLocalVariable(var);
    if (!ret) {
      return false;
    }

    const auto* type = var->getType();
    const auto ret_v = get().traverseType(type);
    return ret_v;
  }

  bool visitLocalVariable(const llvm::DIVariable*) {
    return true;
  }

  bool traverseType(const llvm::DIType* type) {
    if (!type) {
      return true;  // FIXME special case, to be observed
    }

    if (!get().visitType(type)) {
      return false;
    }
    return invoke_if_any(type);
  }

  bool visitType(const llvm::DIType*) {
    return true;
  }

  bool traverseBasicType(const llvm::DIBasicType* basic_type) {
    ++depth_var_;
    const auto exit = detail::create_scope_exit([&]() {
      assert(depth_var_ > 0);
      --depth_var_;
    });
    return get().visitBasicType(basic_type);
  }

  bool visitBasicType(const llvm::DIBasicType*) {
    return true;
  }

  bool traverseDerivedType(const llvm::DIDerivedType* derived_type) {
    ++depth_derived_;
    const auto exit = detail::create_scope_exit([&]() {
      assert(depth_derived_ > 0);
      --depth_derived_;
    });

    const bool ret = get().visitDerivedType(derived_type);
    if (!ret) {
      return false;
    }

    const bool ret_v = get().traverseType(derived_type->getBaseType());
    return ret_v;
  }

  bool visitDerivedType(const llvm::DIDerivedType*) {
    return true;
  }

  bool traverseCompositeType(const llvm::DICompositeType* composite_type) {
    // Make sure, we do not infinitely recurse
    if (visited_node(composite_type)) {
      return true;
    }

    ++depth_composite_;
    const auto exit = detail::create_scope_exit([&]() {
      assert(depth_composite_ > 0);
      --depth_composite_;
    });

    const bool ret = get().visitCompositeType(composite_type);
    visited_dinodes_.insert(composite_type);
    if (!ret) {
      return false;
    }

    const bool ret_b = get().traverseType(composite_type->getBaseType());
    if (!ret_b) {
      return false;
    }

    for (auto* eleme : composite_type->getElements()) {
      invoke_if_any(eleme);
    }

    return true;
  }

  bool visitCompositeType(const llvm::DICompositeType*) {
    return true;
  }
};

}  // namespace visitor

namespace util {

class DIPrinter : public visitor::DINodeVisitor<DIPrinter> {
 private:
  llvm::raw_ostream& outp_;
  llvm::Optional<const llvm::Module*> module_;

  std::string no_pointer_str(const llvm::Metadata& type) {
    std::string view;
    llvm::raw_string_ostream rso(view);
#if LLVM_VERSION_MAJOR > 14
    type.print(rso, module_.value_or(nullptr));

    if (module_) {
      return rso.str();
    }
#else
    type.print(rso, module_.getValueOr(nullptr));

    if (module_) {
      return rso.str();
    }
#endif
    const llvm::StringRef ref(rso.str());
    const auto a_pos = ref.find("=");
    if (a_pos == llvm::StringRef::npos || (a_pos + 2) > ref.size()) {
      return ref.str();
    }

    return std::string{ref.substr(a_pos + 2)};
  }

  [[nodiscard]] unsigned width() const {
    return depth() == 1 ? 0 : depth();
  }

 public:
  explicit DIPrinter(llvm::raw_ostream& outp, const llvm::Module* mod = nullptr) : outp_(outp), module_(mod) {
  }

  bool visitLocalVariable(llvm::DIVariable const* var) {
    outp_ << llvm::left_justify("", width()) << no_pointer_str(*var) << "\n";
    return true;
  }

  bool visitBasicType(const llvm::DIBasicType* basic_type) {
    outp_ << llvm::left_justify("", width() + 3) << no_pointer_str(*basic_type) << "\n";
    return true;
  }

  bool visitDerivedType(const llvm::DIDerivedType* derived_type) {
    outp_ << llvm::left_justify("", width()) << no_pointer_str(*derived_type) << "\n";
    return true;
  }

  bool visitCompositeType(const llvm::DICompositeType* composite_type) {
    outp_ << llvm::left_justify("", width()) << no_pointer_str(*composite_type) << "\n";
    return true;
  }
};

}  // namespace util

}  // namespace meta

#endif  // DIMETA_DIVISITOR_H
