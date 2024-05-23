# Style Guide

## Code Style

Use `.clang-format` to format your code. You can find the configuration file in the root of the repository.

??? info "Naming Rules"
* use `UPPERCASE` for constants, macros, and preprocessor directives
* use `PascalCase` for classes, structs, enums, and namespaces
* use `PascalCase` for methods and properties
* use `PascalCase` for functions
* use `kebabCase` for local variables
* prefix private fields with `m_`
* prefix static fields with `s_`
* prefix static variables with `s_`
* prefix global variables with `g_`

??? info "Other Rules"
* use `#pragma once` instead of include guards
* use the same namespace as the folder structure
* excluding `Gothic` directory which has `GOTHIC_NAMESPACE` defined (subdirectories should use new namespace)
* excluding `Hooks.hpp` files, which may need `GOTHIC_NAMESPACE` to implement member functions
* prefer single class per file
* use the same file name as the class name
* use `#pragma region` and `#pragma endregion` to group related code for bigger files
* don't use `using namespace` in header files
* can use `using namespace` in source files
* don't use `using namespace std` in any file
* avoid use `using namespace X;` where X is a namespace of vendor code
* can use `using namespace X;` where X is the project namespace

## Design Principles

### Separation of Concerns

Code should be modular, and every unit should encapsulate its private state, while providing a public interface
for others to use. Each unit should have a single responsibility. Each unit group should also represent a single,
larger concept being the aggregate of member concepts.

### Keep It Stupid Simple (KISS)

Code should be simple and easy to understand. Don't overengineer, don't over-abstract,
don't generalize specializations that already provide everything you need.

## Memory & Pointers

Pointers of objects shared between multiple consumers are created and managed by an owner. The owner shares the
object as a const reference or a reference if the object must be mutable.

Objects may be owner-less if shared using std::shared_ptr.

Don't dereference a reference unless you absolutely have to.

## Gothic/Union

* Code builds for every engine: G1, G1A, G2, G2A

### Hooks

* Prefer PartialHook if you don't need to wrap the target function from both sides.
* PartialHook: avoid subtracting esp further than the current function frame. Different callers will have different
  stack offsets.

## Would you add or change something?

The style guide is not set in stone. If you think that we should add or alter some rules, feel free to contribute.