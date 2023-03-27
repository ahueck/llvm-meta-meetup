// RUN: %c-to-llvm %s | %apply-meta 2>&1 | %filecheck %s

// CHECK: %a = alloca [10 x i32]

// !14 = !DILocalVariable(name: "a", scope: !9, file: !1, line: 6, type: !15)
// !15 = !DICompositeType(tag: DW_TAG_array_type, baseType: !16, size: 320, elements: !17)
//  !16 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !12)
//   !12 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)

int foo() {
  const int a[10] = {0};
  return a[1];
}
