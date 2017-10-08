#! /usr/bin/env python

import os
import io
from distutils.core import setup, Extension
# from setuptools import setup, Extension
import numpy as np

# NOTE: to compile, run in the current directory
# python setup.py build_ext --inplace
# python setup.py develop

try:
    from Cython.Build import cythonize
    # from Cython.Distutils import build_ext
except ImportError:
    use_cython = False
else:
    use_cython = True


def read(*filenames, **kwargs):
    encoding = kwargs.get('encoding', 'utf-8')
    sep = kwargs.get('sep', '\n')
    buf = []
    for filename in filenames:
        with io.open(filename, encoding=encoding) as f:
            buf.append(f.read())
    return sep.join(buf)


cmdclass = {}
ext_modules = []

here = os.path.abspath(os.path.dirname(__file__))
# NOTE: extension name must match .pyx file name
mthm_extension_name = "knapsack_python.python.cython.mthm"
mthm_cpp_sourcefiles = []
mthm_cython_sourcefile = 'knapsack_python/python/cython/mthm.pyx'
assign_all_extension_name = "knapsack_python.python.cython.assign_all"
assign_all_cpp_sourcefiles = []
assign_all_cython_sourcefile = 'knapsack_python/python/cython/assign_all.pyx'
include_path = [np.get_include(), 'knapsack_python/cpp/include']

if use_cython:
    mthm_sourcefiles = mthm_cpp_sourcefiles + [mthm_cython_sourcefile]
    assign_all_sourcefiles = assign_all_cpp_sourcefiles + [assign_all_cython_sourcefile]
    extensions = [
        Extension(mthm_extension_name,
                  sources=mthm_sourcefiles,
                  include_dirs=include_path,
                  language='c++'),
        Extension(assign_all_extension_name,
                  sources=assign_all_sourcefiles,
                  include_dirs=include_path,
                  language='c++')
        ]
else:
    mthm_sourcefiles = mthm_cpp_sourcefiles + \
        [mthm_cython_sourcefile.copy().replace('.pyx', '.cpp')]
    assign_all_sourcefiles = assign_all_cpp_sourcefiles + \
        [assign_all_cython_sourcefile.copy().replace('.pyx', '.cpp')]
    extensions = [
        Extension(mthm_extension_name,
                  sources=mthm_sourcefiles,
                  include_dirs=include_path,
                  language='c++'),
        Extension(assign_all_extension_name,
                  sources=assign_all_sourcefiles,
                  include_dirs=include_path,
                  language='c++')
        ]


if use_cython:
    ext_modules += \
        cythonize(extensions, compiler_directives={'embedsignature': True,
                                                   'profile': True})
else:
    ext_modules += extensions

long_description = read('README.rst', 'CHANGELOG.rst')

setup(
    name="knapsack_python",
    version='0.1.0',
    url='https://github.com/jhetherly/knapsack_python',
    license='MIT',
    author='Jeff Hetherly',
    author_email='jeffrey.hetherly@gmail.com',
    platforms='any',
    description='Solves a variety of knapsack problems',
    long_description=long_description,
    install_requires=['numpy'],
    packages=['knapsack_python'],
    cmdclass=cmdclass,
    ext_modules=ext_modules,
    include_dirs=[np.get_include(), 'knapsack_python/cpp/include']
)
