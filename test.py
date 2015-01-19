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

from numpy_popcnt import vector_bitdiff, vector_bitdiff_sum, vector_bitdiff_product
import numpy, numpy.random
import cProfile
import unittest
import time

numbitsData=[0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8]

def bitDifference(a,b):
                sum=numbitsData[(a^b)&0xff]
                sum+=numbitsData[((a^b)>>8)&0xff]
                sum+=numbitsData[((a^b)>>16)&0xff]
                sum+=numbitsData[((a^b)>>24)&0xff]
                return sum

def slow_bitDifferenceSum(a,b):
				c=0
				if len(a)!=len(b):
					print "Error: lengths are different"
				else:
					for i in xrange(len(a)):
						c+=bitDifference(a[i],b[i])
				return c

def slow_bitDifference(a,b):
				c=numpy.zeros(len(a))
				if len(a)!=len(b):
					print "Error: lengths are different"
				else:
					for i in xrange(len(a)):
						c[i]=bitDifference(a[i],b[i])
				return c

def bitdiff_product(pattern_data, stream_data):
                xmax=len(stream_data)
                ymax=len(pattern_data)
                match_matrix=numpy.zeros((xmax,ymax))
                for i in range(xmax):
                        for j in range(ymax):
                               	match_matrix[(i+j)%xmax][j]=bitDifference(stream_data[(i+j)%xmax], pattern_data[j])
                return match_matrix
                


class testPopcntFunctions(unittest.TestCase):
	def __init__(self, *args, **kwargs):
		super(testPopcntFunctions,  self).__init__(*args, **kwargs)
		#create some data for use in testing
		self.a=numpy.array([1,2,3,4,5])
		self.b=numpy.array([5,2,7,3,5])
		ma1=numpy.array([6,4,3,4,5,3,1,2,3,4,5,6])
		self.ma=ma1[::2]		#This is a non-contiguous array
		self.mb=numpy.array([5,2,7,3,5,8])
		
	def test_vector_bitdiff(self):
		c=vector_bitdiff(self.a,self.b)
		c_slow=slow_bitDifference(self.a,self.b)
		self.assertTrue((c == c_slow).all())
		

	def test_vector_bitdiff_noncontiguous(self):
		c=vector_bitdiff(self.ma,self.mb)
		c_slow=slow_bitDifference(self.ma,self.mb)
		self.assertTrue((c == c_slow).all())		

	def test_vector_bitdiff_sum(self):
		c=vector_bitdiff_sum(self.a,self.b)
		c_slow=slow_bitDifferenceSum(self.a,self.b)
		self.assertTrue(c==c_slow)
		
	def test_vector_bitdiff_sum_noncontiguous(self):
		c=vector_bitdiff_sum(self.ma,self.mb)
		c_slow=slow_bitDifferenceSum(self.ma,self.mb)
		self.assertTrue(c==c_slow)

	def test_bitdiff_product(self):
		c=vector_bitdiff_product(self.a,self.b)
		c_slow=bitdiff_product(self.a,self.b)
		self.assertTrue((c == c_slow).all())		

	def test_bitdiff_product_noncontiguous(self):
		c=vector_bitdiff_product(self.ma,self.mb)
		c_slow=bitdiff_product(self.ma,self.mb)
		self.assertTrue((c == c_slow).all())	
	
	def test_bitdiff_product_large(self):
		a=numpy.random.randint(0,1<<32,10000)
		b=numpy.random.randint(0,1<<32,200)
		tstart=time.time()
		c=vector_bitdiff_product(a,b)
		tpop=time.time()-tstart
		tstart=time.time()
		c_slow=bitdiff_product(a,b)
		tslow=(time.time()-tstart)
		print "Times:  popcnt: %f s   python: %f s    Speedup=%f"%(tpop, tslow, tslow/tpop)
		self.assertTrue((c == c_slow).all())		


if __name__=='__main__':
	unittest.main()

