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

## Steps
1. Build ITK
```
python build_itk.py ITK-install
```

2. Install
```
ITK_DIR=ITK-install python setup.py install
```