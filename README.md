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
* ZLib

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
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
msbuild ALL_BUILD.vcxproj
msbuild INSTALL.vcxproj
cd ..
```

2. Install zlib (in an administrator prompt)
```
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://raw.githubusercontent.com/horta/zlib.install/master/install.bat', 'install.bat')"; ./install.bat
```

3. Install charm-gems
```
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
set ITK_DIR=ITK-install
python setup.py install
``` 