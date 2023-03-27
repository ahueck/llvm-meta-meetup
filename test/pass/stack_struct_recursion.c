// RUN: %c-to-llvm %s | %apply-meta 2>&1 | %filecheck %s

// CHECK: %impl = alloca %struct.TreeNode, align 8

// !13 = !DILocalVariable(name: "impl", scope: !9, file: !1, line: 10, type: !14)
// !14 = !DIDerivedType(tag: DW_TAG_typedef, name: "TreeNode_t", file: !1, line: 7, baseType: !15)
//  !15 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "TreeNode", file: !1, line: 5, size: 64, elements:
//  !16)
//   !17 = !DIDerivedType(tag: DW_TAG_member, name: "parent", scope: !15, file: !1, line: 6, baseType: !18, size: 64)
//    !18 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !15, size: 64)

typedef struct TreeNode {
  struct TreeNode* parent;
} TreeNode_t;

void foo() {
  TreeNode_t impl;
}