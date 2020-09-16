#pragma once

#include <stdlib.h>

template <typename A>
class Queue{
	protected:
		const int minsize;
		int arraysize = 8;
		A** list; // this essentially means an array of pointers (that point to the object)
		int head = 0; // where we will put the next inserted item
		int tail = 0; // what item we dequeue next
		//virtual void check() = 0;
	public:
		Queue(int size) : minsize(size), arraysize(size){
	        list = (A**) malloc(arraysize*sizeof(A*));
        };
		virtual ~Queue(){free(list);}
		virtual void enqueue(A** array, int length) = 0;
		virtual void enqueue(A* array) = 0;
		//void moveHead(int shift);
		virtual int dequeue(A** fq, int length) = 0;
		virtual A* dequeue() = 0;
		virtual A* peek() = 0;
		virtual bool isEmpty() = 0;
		virtual int getSize() = 0;
		virtual void report() = 0;

};