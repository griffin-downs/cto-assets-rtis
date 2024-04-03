# cto-assets-rtis: Compile-Time Optimized Assets for Real-Time Interactive Simulations

<img src="https://raw.githubusercontent.com/griffin-downs/cto-assets-rtis/main/assets/qr-code.svg" width=100>

### https://cto-assets-rtis.griffin-downs.com/

![Build Project Artifact](https://github.com/griffin-downs/cto-assets-rtis/actions/workflows/build-project-artifact.yml/badge.svg)

## Overview

cto-assets-rtis is an innovative project that pioneers the integration of compile-time optimizations with real-time web simulations and 3D graphics rendering. At its core, it focuses on enhancing web-based 3D real-time interactive simulations through a sophisticated asset pipeline specifically designed for WebAssembly (Wasm). This approach dramatically reduces the overhead associated with traditional file fetching, loading, and data transformation, streamlining the rendering process for WebGL/OpenGL applications.

As of writing, the cto-assets-rtis website is roughly 195kb in size, one tenth the "average size" of a modern website. Additionally, the entire project compiles in roughly 30 seconds. This leaves plenty of room in the future for more detailed, hand-crafted models.

## Key Features

- Compile-Time Optimized 3D Model Asset Pipeline: Utilizes advanced C++ template metaprogramming algorithms and idioms borrowing from Compiler and Programming Language Theory to process and optimize code-generated 3D model data during the build phase, embedding this data directly into the Wasm binary for cache-friendly GPU data loading from the binary's data section, bypassing both the stack and heap.
- Advanced Vertex Management System: Features a flexible and type-safe system for defining and manipulating vertex data structures, automating efficient data layout for GPU processing with self-calculating glVertexAttribPointer attributes.
- High Security through Code Generation: Thanks to the compile-time asset pipeline, this framework is capable of embedding proprietary or secret file data into the program where it can be transformed or used as a transient value to ensure that sensitive data is not present in the resulting binary.
- Cross-Platform Compatibility: Designed to perform seamlessly across various platforms, this project showcases flexibility and wide applicability for developing web-based 3D applications and is a great starting framework for those interested in this emerging space.
- Emscripten Integration: Leverages Emscripten to compile C++ code to WebAssembly, enabling high-performance applications that run in web browsers.
- Camera System: Implements a quaternion-based camera system, allowing for smooth and intuitive user interaction within 3D environments.
- OpenGL Abstractions: Professional abstractions on top of OpenGL utilizing RAII concepts.
- Low Memory Overhead: Minimal heap usage thanks to no dynamic allocations being performed outside of third party library initialization and management.

## Technologies Used

- C++: Utilizes advanced features and template metaprogramming for compile-time optimizations and asset processing.
- WebAssembly (Wasm): Targets WebAssembly for running high-performance web applications.
- Emscripten: Compiles C++ into Wasm, bridging the gap between high-level programming and web development.
- Vcpkg: Package manager for library dependencies.
- WebGL/OpenGL: For rendering 3D graphics in web browsers, leveraging GLM (OpenGL Mathematics) for graphics computations.
- CMake: Employs an advanced build automation pipeline for efficient code generation and project compilation.

## Applications

cto-assets-rtis is particularly beneficial for developers and organizations aiming to create interactive 3D web applications, such as:

- Educational tools and simulations
- Interactive 3D models and product visualizations
- Browser-based games and virtual environments
- Architectural and engineering visualizations

## Getting Started

cto-assets-rtis primarily targets Emscripten (which utilizes the LLVM toolchain) and was developed under Windows. There are no guarantees that it will build under other development setups. That being said, it would be great to expand the build testing matrix in the future. In the meantime, if you would like to build the project:

```powershell

# Get System-Level dependencies
choco install visualstudio2022buildtools --yes --params "--add Microsoft.VisualStudio.Component.VC.Llvm.Clang --add Microsoft.VisualStudio.Component.VC.Llvm.ClangToolset --add Microsoft.VisualStudio.ComponentGroup.NativeDesktop.Llvm.Clang --add Microsoft.VisualStudio.Component.VC.CMake.Project"
choco install --yes ninja

git clone https://github.com/griffin-downs/cto-assets-rtis.git
cd cto-assets-rtis

# Bootstrap the project - this will build the website by default
./Build-Project.ps1

# Build Native:
# ./Build-Project -Target Native

# Load the resulting website by launching a server for it (emrun)
emrun ./build/bin/index.html
```
## Contributing & Usage

This repository is listed as "All Rights Reserved." If you would like to contribute in some way or utilize the code or concepts here, please reach out so we can talk about it. 🙂