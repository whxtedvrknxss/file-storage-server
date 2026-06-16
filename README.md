# Multithreaded File Storage Server

![Static Badge](https://img.shields.io/badge/version-v0.0.0-blue)
![Static Badge](https://img.shields.io/badge/build-passing-green)
![Static Badge](https://img.shields.io/badge/tests-passing-green)

A high-performance, multithreaded file storage server written in C++ designed for efficient file uploads, downloads, and management over HTTP. 

## Features



## Getting the Source
```sh
git clone https://github.com/whxtedvrknxss/file-storage-server.git
```

## Building

This project uses **CMake** as a build-system generator and **vcpkg** as a dependency manager.

If **vcpkg** is not installed, there's a script in _scripts_ directory. 

### Windows Powershell

Run the script:

```sh
python .\scripts\vcpkg_install.py
.\tools\vcpkg\vcpkg_bootstrap.bat
```
Quick start/build everything:
```sh
mkdir build ; cd build
cmake -B . S .. -DCMAKE_TOOLCHAIN_FILE="..\tools\vcpkg\scripts\buildsystems\vcpkg.cmake" 
cmake --build .
```

