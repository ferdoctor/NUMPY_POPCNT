from numpy.distutils.core import setup, Extension
import numpy as np

module_popcnt = Extension('numpy_popcnt',
                    sources = ['numpy_popcnt.c'])

setup (name = 'Numpy_popcnt',
       version = '1.0',
       include_dirs = [np.get_include()],
       description = 'Popcnt extensions for np arrays',
       ext_modules = [module_popcnt])
