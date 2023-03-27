# llvm-meta-meetup

Example project for the LLVM Social Darmstadt
talk `Opaque pointers, the need for debug information and how to use them`.

Finds the corresponding debug intrinsic of `alloca`s and prints their `DIType` structure to console.

Under [lib/pass](lib/pass), the analysis pass to find allocas can be found.
The printing of `DIType` nodes is done with a visitor, see [DIVisitor.h](lib/type/DIVisitor.h).

The visitor calls `traverse` and `visit` for each relevant `DINode` type. Users of the visitor should override
relevant `visit` functions, e.g., `bool visitBasicType(const llvm::DIBasicType* basic_type)`.

## Building llvm-meta-meetup

llvm-meta-meetup requires LLVM >= 10 and CMake version >= 3.20. Use CMake presets `develop` or `release`
to build.

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

```sh
$> cmake --build build --target check-meta
```

## 2.3 Apply

In `build/script` the script `apply.sh` is generated to analyze single translation units,
see [apply.tmpl](script/apply.tmpl).
It is installed to `install-prefix/bin`.

#### Example:

```sh
$> apply.sh test/pass/stack_basic_int.c
```