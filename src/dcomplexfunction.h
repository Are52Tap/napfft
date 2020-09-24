#pragma once
#include "fouriertypes.h"

class DComplexFunction{
	protected:
		CArray* array; 
	public:
		inline CArray& getArray(){return *array;}
		uint64_t getSize(){return array->size();}
		DComplexFunction(CArray* array) : array(array){}
		~DComplexFunction(){
			free(array);
		}
};