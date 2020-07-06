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
* Visual Studio (Windows)
* ZLib

## Instructios

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

2. Install zlib (in an administrator prompt)
```
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://raw.githubusercontent.com/horta/zlib.install/master/install.bat', 'install.bat')"; ./install.bat
```

3. Install charm-gems
```
set ITK_DIR=ITK-install
python setup.py install
``` 