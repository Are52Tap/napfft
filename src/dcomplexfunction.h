#pragma once
#include "fouriertypes.h"

class DComplexFunction{
	protected:
		CArray* array;
		uint64_t size;
	public:
		inline CArray& getArray(){return *array;}
		DComplexFunction(CArray* array, uint64_t size) : array(array), size(size){}
		~DComplexFunction(){
			free(array);
		}
};