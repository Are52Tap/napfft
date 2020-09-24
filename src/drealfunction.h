#pragma once

#include <stdlib.h>
#include <inttypes.h>

class DRealFunction{
	private:
		double* array;
		uint64_t size;
	public:
		DRealFunction(double* array, uint64_t size) : array(array), size(size){}
		~DRealFunction(){
			free(array);
		}
		double* getArray(){return array;}
		uint64_t getSize(){return size;}
};