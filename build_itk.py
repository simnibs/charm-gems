# Build the ITK library
import os
import sys
import argparse
import subprocess
import tempfile

ITK_SOURCE_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), 'ITK'))


def build_itk_posix(itk_install_dir):
    itk_install_dir = os.path.abspath(itk_install_dir)
    with tempfile.TemporaryDirectory() as tmpdir:
        os.chdir(tmpdir)
        subprocess.run([
            'cmake',
            '-DBUILD_SHARED_LIBS=OFF',
            '-DBUILD_TESTING=OFF',
            '-DBUILD_EXAMPLES=OFF',
            f'-DCMAKE_INSTALL_PREFIX={itk_install_dir}',
            ITK_SOURCE_DIR],
            check=True
        )
        subprocess.run(['make', 'install'], check=True)

def build_itk_win(itk_install_dir):
    itk_install_dir = os.path.abspath(itk_install_dir)
    with tempfile.TemporaryDirectory() as tmpdir:
        tmpdir = 'ITK-TMP2'
        os.makedirs(tmpdir, exist_ok=True)
        os.chdir(tmpdir)
        subprocess.run([
            r'C:\CMake\bin\cmake',
            '-G', 'Visual Studio 16 2019',
            '-DBUILD_SHARED_LIBS=OFF',
            '-DBUILD_TESTING=OFF',
            '-DBUILD_EXAMPLES=OFF',
            f'-DCMAKE_INSTALL_PREFIX={itk_install_dir}',
            ITK_SOURCE_DIR],
            check=True
        )
        subprocess.run(['make', 'install'], check=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        prog="build_itk",
        description="Builds the ITK library"
    )
    parser.add_argument(
        "itk_install_dir",
        help="Directory where the ITK library will be installed"
    )
    args = parser.parse_args(sys.argv[1:])
    if sys.platform == 'win32':
        build_itk_win(args.itk_install_dir)
    else:
        build_itk_posix(args.itk_install_dir)
