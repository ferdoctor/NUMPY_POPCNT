/* NUMPY_POPCNT
*
* Copyright (c) 2014-2015 Dr. Fernando Rodriguez Salazar
*
* This file is part of NUMPY_POPCNT, a numpy extension to do population 
* count of bits in numpy arrays.
*
*    NUMPY_POPCNT is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 2 of the License, or
*    (at your option) any later version.
*
*    NUMPY_POPCNT is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NUMPY_POPCNT.  If not, see <http://www.gnu.org/licenses/>
*/

#include "Python.h"
#include <numpy/arrayobject.h>
#include "numpy_popcnt.h"
#include <math.h>


/*static int calcbitdiff(uint32_t a, uint32_t b) {
	int n=0;
	n+=numbitsData[(a&0xff)^(b&0xff)];  a>>=8; b>>=8;
	n+=numbitsData[(a&0xff)^(b&0xff)];  a>>=8; b>>=8;
	n+=numbitsData[(a&0xff)^(b&0xff)];  a>>=8; b>>=8;
	n+=numbitsData[a^b];
	return n;
}*/





static char vector_bitdiff_doc[]=
"Returns a new vector containing the number of bits that are different for each element"
"\nInterface:   vector_bitdiff(a,b) where a,b are input vectors of Integer type"
"\nReturns a new vector c, such that c[i]= number of different bits in a[i] and b[i]"
"that is to say   c[i]=bitcount(a[i]^b[i])";
static PyObject *vector_bitdiff(PyObject *self, PyObject *args)  
{
    PyArrayObject *vecin, *vecin2, *vecout;
    int64_t *cin, *cin2;   //Pointers to vectors data
    int8_t *cout; //Output vector (8 bits, as input vectors are 64 bits, then mas bitdiff fits into 7 bits)
    int i,n, dims[2];
    /* Parse tuples separately since args will differ between C fcns */
    if (!PyArg_ParseTuple(args, "O!O!", 
        &PyArray_Type, &vecin, &PyArray_Type, &vecin2))  return NULL;
    if (NULL == vecin || NULL == vecin2)  return NULL;

    if (not_intvector(vecin))   return NULL; //Can only operate with int vectors
    n=dims[0]=vecin->dimensions[0];
    dims[1]=0;
    if (not_intvector(vecin2)) return NULL; //Can only operate with int vectors
    /* Get the dimension of the input */
    if (n!=vecin2->dimensions[0]) //Dimensions need to be exaclty the same
    {
		PyErr_SetString(PyExc_ValueError,
			"Input arrays differ in size/shape");
		return NULL;		
	}


	//Prepare output array; Make a new vector of same dimension 
    vecout=(PyArrayObject *) PyArray_SimpleNew(1,(npy_intp *) dims, NPY_INT8);
    if (NULL == vecout)
		return PyErr_NoMemory();
			
    cin=(int64_t *) vecin->data;
    cin2=(int64_t *) vecin2->data;
    cout=(int8_t *) vecout->data;

    //Do the calculations
   	if (PyArray_ISCONTIGUOUS(vecin)&&PyArray_ISCONTIGUOUS(vecin2)&&PyArray_ISCONTIGUOUS(vecout)) {
		//If arrays are contiguous it is faster to access the data directly
		for ( i=0; i<n; i++)  {
					cout[i]=__builtin_popcount(cin[i]^cin2[i]);
		}
	} else {
		printf("Info: ARRAYS are not CONTIGUOUS.  Consider creating contiguous arrays for faster runtimes\n");
		//Arrays are not contiguous; use macros to access elements
		for ( i=0; i<n; i++)  {
					*(int8_t *)PyArray_GETPTR1(vecout, i) =
						__builtin_popcount( 
								*(int64_t *)PyArray_GETPTR1(vecin, i)  ^ 
								*(int64_t *)PyArray_GETPTR1(vecin2, i) 
						);
		}
	}

    return PyArray_Return(vecout);
}


static char vector_bitdiff_sum_doc[]=
"Returns the number of bit differences in two input vectors"
"\nInterface:   vector_bitdiff_sum(a,b) where "
" a,b are input vectors of Integer type and of equal size"
"\nReturns a scalar (integer) c = number of different bits in a[i] and b[i]"
" that is to say   c=sum ( bitcount(a[i]^b[i]) ) ";
 
static PyObject *vector_bitdiff_sum(PyObject *self, PyObject *args)  
{
    PyArrayObject *vecin, *vecin2;
    int64_t *cin, *cin2;   // Pointers to vector data
    int i,n, dims[2];
    int count=0;
    /* Parse tuples separately since args will differ between C fcns */
    if (!PyArg_ParseTuple(args, "O!O!", 
        &PyArray_Type, &vecin, &PyArray_Type, &vecin2))  return NULL;
    if (NULL == vecin || NULL == vecin2)  return NULL;

    if (not_intvector(vecin)) return NULL;   //Can only operate with integer vectors
        n=dims[0]=vecin->dimensions[0];
        dims[1]=0;
    if (not_intvector(vecin2)) return NULL;   //Can only operate with int vectors
    /* Get the dimension of the input */
    if (n!=vecin2->dimensions[0]) return NULL; //Dimensions need to be exaclty the same


    cin=(int64_t *) vecin->data;
    cin2=(int64_t *) vecin2->data;

    /* Do the calculation. */
  	if (PyArray_ISCONTIGUOUS(vecin)&&PyArray_ISCONTIGUOUS(vecin2)) {
		for ( i=0; i<n; i++)  {
				//count+= calcbitdiff(cin[i],cin2[i]);  //Use this for testing 
			count+=__builtin_popcount(cin[i]^cin2[i]);  //Use this in production; uses builtin popcount (whatever that maybe)
		}
	} else {
		printf("Info: ARRAYS are not CONTIGUOUS.  Consider creating contiguous arrays for faster runtimes\n");
		for ( i=0; i<n; i++)  {
			count+=__builtin_popcount(
								*(int64_t *)PyArray_GETPTR1(vecin, i)  ^ 
								*(int64_t *)PyArray_GETPTR1(vecin2, i) 
						);
			
		}
	}

    return Py_BuildValue("i",count);

}



/* ==== Check that PyArrayObject is a one dimensional integer type vector ====
    return 1 if an error and raise exception */ 
int  not_intvector(PyArrayObject *vec)  {
    if (vec->descr->type_num != NPY_INT64)  {
       	PyErr_SetString(PyExc_ValueError,
            "Vector must be of type Integer.");
        return 1;  }
    if (vec->nd != 1)  {
        PyErr_SetString(PyExc_ValueError,
            "Vector must be 1 dimensional.");
        return 1;  }

    return 0;
}



static char vector_bitdiff_product_doc[]=
" Returns a matrix containing the number of bits when sliding one input vector over the other"
"\nInterface:   vector_bitdiff_product(a,b) where a,b are input vectors of Integer type"
"\nReturns a matrix m, such that m[j][i]= number of different bits in a[(i+j)%n] and b[j] "
" that is to say   m[j][i]=bitcount(a[(i+j)%n]^b[j])"
"\nThis is usefull for matching patterns\n";
static PyObject *vector_bitdiff_product(PyObject *self, PyObject *args)  
{
    PyArrayObject *vecin, *vecin2, *vecout;
    //uint8_t **mout;
    int64_t *cin, *cin2;   	// Pointers to the data of the input vectors 
	uint8_t *cout;
	npy_intp dims[2];

    int i,j,n,m;
    /* Parse tuples separately since args will differ between C fcns */
    if (!PyArg_ParseTuple(args, "O!O!", 
        &PyArray_Type, &vecin2, &PyArray_Type, &vecin))  return NULL;
    if (NULL == vecin || NULL == vecin2)  return NULL;

    if (not_intvector(vecin)) return NULL;   //Can only operate with integer vectors
    	n=dims[0]=vecin->dimensions[0];
    if (not_intvector(vecin2)) return NULL;   //Can only operate with integer vectors
	m=dims[1]=vecin2->dimensions[0];


	vecout=(PyArrayObject *)  PyArray_SimpleNew(2, dims, NPY_UINT8);  //We should never have more than 255 bits in difference, unless an exotic array type (with more than 255 bits is used)
	if (NULL == vecout)
		return PyErr_NoMemory();


	if (PyArray_ISCONTIGUOUS(vecin)&&PyArray_ISCONTIGUOUS(vecin2)&&PyArray_ISCONTIGUOUS(vecout)) {
	//if (1) {
		//If arrays are contiguous it is faster to access the data directly
		cin=(int64_t *) vecin->data;
		cin2=(int64_t *) vecin2->data;
		cout=(uint8_t *) vecout->data;

		for ( i=0; i<n; i++)  {
		 for (j=0; j<m; j++) {
			 cout[j+m*((i+j)%n)]=__builtin_popcount(
					cin[(i+j)%n] ^ cin2[j]);
				
		 }
		}
	} else {
		//Arrays are not contiguous, access the data using the macros
		printf("Info: ARRAYS are not CONTIGUOUS.  Consider creating contiguous arrays for faster runtimes\n");
		for ( i=0; i<n; i++)  {
		 for (j=0; j<m; j++) {
			*(uint8_t *)PyArray_GETPTR2(vecout, (i+j)%n, j)= __builtin_popcount(
				*(int64_t *)PyArray_GETPTR1(vecin, (i+j)%n) ^
				*(int64_t *)PyArray_GETPTR1(vecin2, j) );
		 }
		}
	}
    return PyArray_Return(vecout);

}





/* ==== Set up the methods table ====================== */
static PyMethodDef module_Methods[] = {
    {"vector_bitdiff", vector_bitdiff, METH_VARARGS, vector_bitdiff_doc},
    {"vector_bitdiff_sum", vector_bitdiff_sum, METH_VARARGS, vector_bitdiff_sum_doc},
    {"vector_bitdiff_product", vector_bitdiff_product, METH_VARARGS, vector_bitdiff_product_doc},
    {NULL, NULL}     //End of structure marker
};

// ==== Initialize the module functions 
// Module name must be same when compiled and linked 
void initnumpy_popcnt()  {
    (void) Py_InitModule("numpy_popcnt", module_Methods);
    import_array();  // NumPy needs this called first after above line.
}



