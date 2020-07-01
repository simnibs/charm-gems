from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import os
import sys
import shutil
import glob
import subprocess

__version__ = '1.0.0'

# RUN with ITK_DIR="PATH_TO_ITK" python setup.py

class build_ext_(build_ext):
    def run(self):
        os.makedirs(os.path.join(self.build_lib, 'charm_gems'), exist_ok=True)
        subprocess.run([
            'cmake',
            f'-DPYTHON_EXECUTABLE={sys.executable}'],
            check=True
        )
        subprocess.run(['make'], check=True)
        compiled_lib = glob.glob('gems_python/gemsbindings.cpython-*.so')
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


setup(name='charm_gems',
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
      setup_requires=['pybind11>=2.5.0'],
      install_requires=['numpy'],
      zip_safe=False
     )
