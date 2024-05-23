# Developer Guide

This guide is intended for developers who would like to work on zBassMusic code or get a better understanding
of how the system works.
It explains the architecture of the system, how to build it, and how to extend it.

## Prerequisites

* MSVC 14.39 or later
* CMake 3.25 or later
* Git

## Setup Project

!!! info "CMake vs IDE"
The setup instructions assume no IDE is used, and CMake is called from the command line.
If you use Visual Studio, CLion or any other IDE, you can use the CMake integration from it.

### Fork Repository

Unless you are an internal contributor, you can't push directly to the repository, so you have to fork it first.
To do it, click the "Fork" button in the top right corner of
the [repository page](https://github.com/Silver-Ore-Team/zBassMusic).

### Clone Repository

Clone your fork of the repository to your local machine.

```bash
git clone git@github.com:YOUR_USERNAME/zBassMusic.git
```

### Initialize Submodules

Pull the submodule dependencies.

```bash
git submodule init --remote
```

### Generate CMake

Generate CMake build directory using x86-debug preset. You can use presets:

* `x86-release` (Release)
* `x86-release-pdb` (RelWithDebInfo)
* `x86-debug` (Debug)

```bash 
cmake --preset="x86-debug"
```

### Build Project

Now you can compile the project.

```bash
cmake --build --preset="x86-debug"
```

Compiled binaries will be in the `out/x86-debug/build` directory.

VDF build will be in the `out/x86-debug/install` directory.

### Run Project

The project doesn't have any executables. You can run it using Gothic I or Gothic II games after installing the plugin.
For more information, see the [User Guide](../user-guide/index.md).

We recommend starting Gothic with debuggers attached to catch any unexpected exceptions.