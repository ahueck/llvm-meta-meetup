// RUN: %c-to-llvm %s | %apply-meta 2>&1 | %filecheck %s

// CHECK: alloca %struct.BasicStruct

// !14 = !DILocalVariable(name: "b_struct", scope: !9, file: !1, line: 10, type: !15)
// !15 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "BasicStruct", file: !1, line: 5, size: 32,
// elements: !16)
//  !17 = !DIDerivedType(tag: DW_TAG_member, name: "a", scope: !15, file: !1, line: 6, baseType: !12, size: 32)
//      !12 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)

struct BasicStruct {
  int a;
};

int foo() {
  struct BasicStruct b_struct = {0};
  return b_struct.a;
}
