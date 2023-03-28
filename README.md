# llvm-meta-meetup &middot; [![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause) ![](https://github.com/ahueck/llvm-meta-meetup/workflows/Meta-CI/badge.svg?branch=main)

Example project for the LLVM Social Darmstadt meetup
talk `Opaque pointers, the need for debug information and how to use them`.
The talk is attached with the initial release of this project,
see [Releases](https://github.com/ahueck/llvm-meta-meetup/releases).

## Scope

Finds the corresponding debug intrinsic of `alloca`s and prints their `DIType` structure to console.

Under [lib/pass](lib/pass), the analysis pass to find `alloca`s and their debug intrinsics can be found.
The printing of `DIType` nodes is done with a visitor, see [DIVisitor.h](lib/type/DIVisitor.h).

#### Example

Given the code:

```c
struct BasicStruct {
  int a;
};
int foo() {
  struct BasicStruct b_struct;
}
```

The pass will emit the following to console:

```llvm
  %b_struct = alloca %struct.BasicStruct, align 4:

!14 = !DILocalVariable(name: "b_struct", scope: !9, file: !1, line: 5, type: !15)
!15 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "BasicStruct", file: !1, line: 1, size: 32, elements: !16)
  !17 = !DIDerivedType(tag: DW_TAG_member, name: "a", scope: !15, file: !1, line: 2, baseType: !12, size: 32)
      !12 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
```

The visitor calls `traverse` and `visit` for each relevant `DINode` type. Users of the visitor should override
relevant `visit` functions, e.g., `bool visitBasicType(const llvm::DIBasicType* basic_type)`.

## Building llvm-meta-meetup

llvm-meta-meetup requires LLVM >= 10 (tested only with 14 & 15) and CMake version >= 3.20.
For testing, `llvm-lit` is used.
Use CMake presets `develop` or `release` to build.
Refer to the [CI file](.github/workflows/ci.yml) for a complete recipe.

### 2.1 Build

llvm-meta-meetup uses CMake to build. Example build recipe (release build, installs to default prefix
`${llvm-meta-meetup_SOURCE_DIR}/install/meta`)

```sh
$> cd llvm-meta-meetup
$> cmake --preset release
$> cmake --build build --target install --parallel
```

## 2.2 Test

Assuming build as described above.
Execute the lit test suite:

```sh
$> cmake --build build --target check-meta
```

## 2.3 Apply

In `build/script` the script `apply.sh` is generated to analyze single translation units,
see [apply.tmpl](script/apply.tmpl).
It is installed to `install-prefix/bin`.
For debug builds, the script name includes `${CMAKE_DEBUG_POSTFIX}` as a postfix.

#### Example:

```sh
$> cd llvm-meta-meetup
$> $install-prefix/bin/apply.sh test/pass/stack_basic_int.c
```