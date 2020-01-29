## C Bindings Layer
**C++** is great, and header-only libraries are super convenient as
zero-configuration, drag-and-drop, dependencies, but both present significant
challenges for any use case that requires long-term **ABI** stability (_any_
modification can present as an **ABI** break for header-only libraries),
or cross-language FFI bindings (symbol mangling is a never-ending nightmare).

To support these, and many other, use cases, **Dart** provides a secondary interface
(available in the `#include <dart/abi.h>` header) that any **C89** compliant
toolchain/FFI library can bind against without issue. Additionally, the **ABI**
header is self contained and can be shipped around without the rest of the
surrounding project.

It is worth noting that the **Dart** **C** layer is _not_ a re-implementation of
the library, and simply wraps the **C++** layer such that implementatoin details
beyond the public **C** interface are not visible to clients. It does _not_
provide a performance improvement over the **C++** interface.

## Compilation and Installation
**Dart** is implemented using modern **C++**, and requires both Microsoft's Guidelines
Support Library [GSL](https://github.com/Microsoft/GSL), and a **C++14** enabled toolchain
(`clang` >= **3.8**, `gcc` >= **5.0**, apple's `clang` >= **8.3**, `MSVC` ~> **19.22**).
Support for **C++11**  may be added in the future, but is not currently being pursued.

**Dart** utilizes `cmake` for its build process and currently primarily targets
Linux/macOS, but has experimental (and improving) support for Windows.
On Linux/macOS, it can be built in the following way:
```bash
# Clone it down.
git clone https://github.com/target/libdart.git
cd libdart/

# Create the cmake build directory and prepare a build
# with tests enabled.
mkdir build
cd build

# Build, test, and install (assuming a 4 core machine).
# Running tests isn't mandatory, but is highly recommended.
# After install, the Dart ABI can be linked against using
# -ldart_abi, or -ldart_abi_static if static linking is
# desired.
cmake .. -Dbuild_abi=ON
make -j 4
ctest
make install

# Generate documentation (if desired).
# Doxygen must have already been installed.
# Generates documentation inside the directory "docs"
cd ..
doxygen
```
For instructions on building for windows, see our [windows](WINDOWS.md) build instructions.

**Dart** can optionally leverage [RapidJSON](https://github.com/Tencent/rapidjson),
[sajson](https://github.com/chadaustin/sajson), 
and [libyaml](https://github.com/yaml/libyaml.git), and will attempt to detect installations
automatically while building, but can be independently specified with `-DDART_HAS_RAPIDJSON`,
`-DDART_USE_SAJSON`, and `-DDART_HAS_YAML` preprocessor flags.

## Usage
To illustrate usage for the bindings layer, this document will refer back to **C++** examples
from the primary [readme](README.md), showing how equivalent operations would be performed
using the C layer.
