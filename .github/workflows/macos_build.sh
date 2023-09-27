#!/bin/bash -e
# This is an auxiliary script to build SimNIBS wheels
PYTHON_PATH=$1
$PYTHON_PATH -m pip wheel . -w ./dist  --no-deps
$PYTHON_PATH -m pip install charm-gems -f dist/
$PYTHON_PATH -c 'import charm_gems'
rm charm_gems/*.so gems_python/*.so
