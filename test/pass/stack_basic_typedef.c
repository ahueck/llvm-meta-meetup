// RUN: %c-to-llvm %s | %apply-meta 2>&1 | %filecheck %s

// CHECK: %a = alloca i32

// !14 = !DILocalVariable(name: "a", scope: !9, file: !1, line: 8, type: !15)
// !15 = !DIDerivedType(tag: DW_TAG_typedef, name: "Integer", file: !1, line: 5, baseType: !12)
//     !12 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)

typedef int Integer;

int foo() {
  Integer a = 0;
  return a;
}
