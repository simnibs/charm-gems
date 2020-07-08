# charm-gems

[![PyPI version](https://badge.fury.io/py/charm-gems.svg)](https://badge.fury.io/py/charm-gems) ![Linux Build](https://github.com/simnibs/charm-gems/workflows/Linux%20Build/badge.svg)  ![MacOS Build](https://github.com/simnibs/charm-gems/workflows/MacOS%20Build/badge.svg) ![Windows Build](https://github.com/simnibs/charm-gems/workflows/Windows%20Build/badge.svg)

This repository contains the gems C++ code and python bindings used in Freesurfer's Sequence-Adaptive Multimodal SEGmentation ([SAMSEG](https://github.com/freesurfer/freesurfer/tree/dev/python/freesurfer/samseg)) ([Puonti et al., NeuroImage, 2016](https://www.sciencedirect.com/science/article/pii/S1053811916304724)) and in SimNIBS 4.0 Complete Head Anatomy Reconstruction Method (CHARM) ([Puonti et al., NeuroImage, 2020](https://www.sciencedirect.com/science/article/pii/S1053811920305309)) to create individualized head models for electric field simulations.
## Installation

The compiled charm-gems library is available via pip ≥ 19.3, for python 3.6, 3.7 and 3.8

```bash
pip install --upgrade pip
pip install charm-gems
```
Afterwards the package can be imported by calling

```python
import charm_gems as gems
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
This repository uses submodules. To start it, use

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
