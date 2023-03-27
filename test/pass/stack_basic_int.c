// RUN: %c-to-llvm %s | %apply-meta 2>&1 | %filecheck %s

// CHECK: %a = alloca i32, align 4

// !14 = !DILocalVariable(name: "a", scope: !9, file: !1, line: 6, type: !12)
//   !12 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)

int main(void) {
  int a = 10;
  return 0;
}
