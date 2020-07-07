#!/bin/bash -e
# This is an auxiliary script to build SimNIBS wheels
PYTHON_PATH=$1
$PYTHON_VERSION -m pip install --user --upgrade setuptools wheel
$PYTHON_VERSION setup.py bdist_wheel
$PYTHON_VERSION -m pip install charm-gems -f dist/
$PYTHON_VERSION -c 'import charm_gems'

