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
extentions_info = [
    {
        "cython_sourcefile": "knapsack_python/python/cython/mthm.pyx"
    },
    {
        "cython_sourcefile": "knapsack_python/python/cython/assign_all.pyx"
    }
]
include_path = [np.get_include(), 'knapsack_python/cpp/include']

extensions = []
for extention_info in extentions_info:
    sourcefiles = extention_info.get("cpp_sourcefiles", [])
    cython_sourcefile = extention_info.get("cython_sourcefile")
    # NOTE: extension name must match .pyx file name
    extension_name = cython_sourcefile.replace('.pyx', '').replace(os.sep, '.')
    if use_cython:
        sourcefiles += [cython_sourcefile]
    else:
        sourcefiles += [cython_sourcefile.replace('.pyx', '.cpp')]
    extensions.append(Extension(extension_name,
                                sources=sourcefiles,
                                include_dirs=include_path,
                                language='c++'))


if use_cython:
    ext_modules += \
        cythonize(extensions, compiler_directives={'embedsignature': True,
                                                   'profile': True})
else:
    ext_modules += extensions

long_description = read('README.rst', 'CHANGELOG.rst')

setup(
    name="knapsack_python",
    version='0.1.1',
    url='https://github.com/jhetherly/python_knapsack',
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
