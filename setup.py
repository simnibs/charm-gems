from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import os
import sys
import shutil
import glob
import subprocess
import tempfile

__version__ = '1.0.0'

# RUN with ITK_DIR="PATH_TO_ITK" python setup.py

class build_ext_(build_ext):
    def run(self):
        os.makedirs(os.path.join(self.build_lib, 'charm_gems'), exist_ok=True)
        package_root = os.path.abspath(os.path.dirname(__file__))
        with tempfile.TemporaryDirectory() as tmpdir:
            subprocess.run([
                'cmake',
                f'-DPYTHON_EXECUTABLE={sys.executable}',
                f'-B{tmpdir}'
                '.'],
                check=True
            )
            if sys.platform == 'win32':
                try:
                    subprocess.run(['msbuild', '/?'], check=True, capture_output=True)
                except WindowsError:
                    raise WindowsError(
                        'Could not find msbuild, have you run vcvars64.bat?\n'
                        r'eg:"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"'
                        )
                subprocess.run(['msbuild', os.path.join(tmpdir, 'ALL_BUILD.vcxproj')], check=True)
                compiled_lib = glob.glob(os.path.join(
                    package_root, 'gems_python', 'Debug', 'gemsbindings.*.pyd'
                ))
            else:
                subprocess.run(['make', '-C', tmpdir], check=True)
                compiled_lib = glob.glob(os.path.join(
                    package_root, 'gems_python', 'gemsbindings.cpython-*.so'
                ))
        if len(compiled_lib) == 0:
            raise OSError(
                'Something went wrong during compilation '
                'did not find any compiled libraries'
            )
        if len(compiled_lib) > 1:
            raise OSError(
                'Find many compile libraries. Please clean it up and try again'
            )
        shutil.copy(compiled_lib[0], os.path.join(self.build_lib, 'charm_gems'))
        shutil.copy(compiled_lib[0], 'charm_gems')


setup(
    name='charm_gems',
    version=__version__,
    description='Something descriptive',
    url='https://github.com/simnibs/charm-gems',
    author='Oula Puonti and Koen Van Leemput',
    author_email='oulap@drcmr.dk',
    packages=['charm_gems'],
    license='GPL3',
    # We define ext_modules to trigger a build_ext run
    ext_modules=[
        Extension(
            'charm_gems.gemsbindings', ['dummy'],
            depends=glob.glob('gems*/*.cxx') + glob.glob('gems*/*.h')
        )],
    cmdclass={
        'build_ext': build_ext_,
      },
    install_requires=['numpy'],
    zip_safe=False
)
