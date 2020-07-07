@echo off

python -m pip install --user --upgrade setuptools wheel || exit /b
python setup.py bdist_wheel                             || exit /b
python -m pip install charm-gems -f .\dist\             || exit /b
python -c 'import charm_gems'                           || exit /b
del charm_gems\*.pyd gems_python\Releases\*.pyd         || exit /b

