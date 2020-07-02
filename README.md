# charm-gems

## Preparation
This repostiroy uses submodules. To start it, please use

```bash
git submodule init
git submodule update
```

## Installing

## Requirements
* A C++ compiler compatible with C++ 11
* Python >= 3.7
* CMake
* Make (Linux/MacOS)

## Instructios

### Linux/MacOS
1. Build ITK
```
mkdir ITK-tmp
cd ITK-tmp
cmake \
    -DBUILD_SHARED_LIBS=OFF \
    -DBUILD_TESTING=OFF \
    -DBUILD_EXAMPLES=OFF \
    -DCMAKE_INSTALL_PREFIX=../ITK-install \
    ../ITK
make install
cd ..
```

2. Install charm-gems
```
ITK_DIR=ITK-install python setup.py install
```
### Windows (Visual Studio 2019)
1. Build ITK
```
md ITK-tmp
cd ITK-tmp
cmake.exe ^
    -G "Visual Studio 16 2019" ^
    -DBUILD_SHARED_LIBS=OFF ^
    -DBUILD_TESTING=OFF ^
    -DBUILD_EXAMPLES=OFF ^
    -DCMAKE_INSTALL_PREFIX=..\ITK-install ^
    ..\ITK
make install
cd ..
```

"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"