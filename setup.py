# NUMPY_POPCNT
#
# Copyright (c) 2014-2015 Dr. Fernando Rodriguez Salazar
#
# This file is part of NUMPY_POPCNT, a numpy extension to do population 
# count of bits in numpy arrays.
#
#    NUMPY_POPCNT is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 2 of the License, or
#    (at your option) any later version.
#
#    NUMPY_POPCNT is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with NUMPY_POPCNT.  If not, see <http://www.gnu.org/licenses/>
#

from numpy.distutils.core import setup, Extension
import numpy as np

module_popcnt = Extension('numpy_popcnt',
                    sources = ['numpy_popcnt.c'])

setup (name = 'Numpy_popcnt',
       version = '1.0',
       include_dirs = [np.get_include()],
       description = 'Popcnt extensions for np arrays',
       ext_modules = [module_popcnt])
