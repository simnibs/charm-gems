# charm-gems

[![PyPI version](https://badge.fury.io/py/charm-gems.svg)](https://badge.fury.io/py/charm-gems) ![Linux Build](https://github.com/simnibs/charm-gems/workflows/Linux%20Build/badge.svg)  ![MacOS Build](https://github.com/simnibs/charm-gems/workflows/MacOS%20Build/badge.svg) ![Windows Build](https://github.com/simnibs/charm-gems/workflows/Windows%20Build/badge.svg)


## Installation

The compiled charm-gems library is avalaibles via pip ≥ 19.3, for python 3.6, 3.7 and 3.8

```bash
pip install --upgrade pip
pip install charm-gems
```

## Manual Installation


### Requirements
* A C++ compiler compatible with C++ 11
* Python ≥ 3.6
* CMake
* Make (Linux/MacOS)
* Visual Studio (Windows)
* zlib (for windows, see https://github.com/horta/zlib.install)

### Preparation
This repostiroy uses submodules. To start it, use

```bash
git submodule init
git submodule update
```

### Linux/MacOS
1. Build ITK
```
mkdir ITK-build
cd ITK-build
cmake \
    -DBUILD_SHARED_LIBS=OFF \
    -DBUILD_TESTING=OFF \
    -DBUILD_EXAMPLES=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=../ITK-install \
    ../ITK
make install
cd ..
```

2. Install charm-gems
```
ITK_DIR=ITK-install python setup.py install
```
### Windows (Tested on Visual Studio 2019)
1. Build ITK
```
md ITK-build
cd ITK-build
cmake.exe ^
    -DBUILD_SHARED_LIBS=OFF ^
    -DBUILD_TESTING=OFF ^
    -DBUILD_EXAMPLES=OFF ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_INSTALL_PREFIX=..\ITK-install ^
    ..\ITK
cmake --build . --config Release --target Install
cd ..
```

2. Install charm-gems
```
set ITK_DIR=ITK-install
python setup.py install
``` 
