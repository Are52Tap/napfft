#include "streamqueue.h"

#include <stdlib.h>
#include <iostream>
#include <stdio.h>

#include "mutex_wrapper.h"
#include "audiostream.h"

void StreamQueue::check(Mutex waffel){
	// if the tail has caught up to the head, we can move the head and tail to the beginning
	if(tail == head){
		waffel.lock();
		head = 0;
		tail = 0;
	} 
	// if the tail is too far in, we may want to shift all the data back towards the beginning
	else if(2*(tail)+2 > arraysize){ /* aka: tail+1 > arraysize/2 */
		waffel.lock();
		int put = 0;
		for(int i = tail; i < head; i++)
			*(list+put++) = *(list+i);
		tail = 0;
		head = put;
	}
	
	// if thre are 1/8 items in the array, and the size of the arraysize won't be too small, then we shrink the array to save space
	if(arraysize > 2*minsize && arraysize > 8*head){
		waffel.lock();
		arraysize/=2;
		list = (AudioStream**) realloc(list, arraysize*sizeof(AudioStream*));
	}
}

// Initalizes the minsize to be size, and arraysize to size
StreamQueue::StreamQueue(int size) : Queue(size){
	//list = (MessageFrame**) malloc(arraysize*sizeof(MessageFrame*));
}

// // Release the queue or array from heap memory
// FrameQueue::~FrameQueue(){
// 	//free(list);
// }

// Last In, Last Out
void StreamQueue::enqueue(AudioStream* ch){
    Mutex waffel(wafer);
	// if we reach the end of the array, resize it by double the size.
	if(head == arraysize){
		waffel.lock();
		arraysize*=2;
		list = (AudioStream**) realloc(list, arraysize*sizeof(AudioStream*));
	}
	waffel.lock();
	*(list+head++) = ch;
	check(waffel);
	waffel.unlock();
}

void StreamQueue::enqueue(AudioStream** array, int length){
    Mutex waffel(wafer);
	if(head+length-1 >= arraysize){
		waffel.lock();
		while(head+length-1 >= arraysize)
			arraysize*=2;
		list = (AudioStream**) realloc(list, arraysize*sizeof(AudioStream*));
	}
	waffel.lock();
	for(int i = 0; i < length; i++){
		*(list+head++) = *(array+i);
	}
	check(waffel);
	waffel.unlock();
}

/*char* FrameQueue::getPointer(int length){
	if(head+length-1 >= arraysize){
		while(head+length-1 >= arraysize)
			arraysize*=2;
		list = (MessageFrame**) realloc(list, arraysize*sizeof(MessageFrame*));
	}
	return list+tail;
}*/

/*void FrameQueue::moveHead(int shift){
	head+=shift;
	check(waffel);
}*/

int StreamQueue::dequeue(AudioStream** buffer, int length){
    Mutex waffel(wafer);
	int pos = 0;
	waffel.lock();
	while(tail < head && pos < length)
		*(buffer+pos++) = *(list+tail++);
	check(waffel);
	waffel.unlock();
	return pos;
}

// First In, First Out
AudioStream* StreamQueue::dequeue(){
    Mutex waffel(wafer);
	// if the tail is at the head, then there are no objects in the queue
	if(tail > head-1)
		return nullptr;
	waffel.lock();
	AudioStream* fr = list[tail++];
	//list[tail++] = nullptr;
	check(waffel);
	waffel.unlock();
	return fr;
}

/*int FrameQueue::peek(char* buffer, int length){
	int pos = 0;
	while(tail+pos < head && pos < length){
		*(buffer+pos) = *(list+tail+pos);
		pos++;
	}
	return pos;
}*/

// look at what would dequeue would return
/*char FrameQueue::peek(){
	if(tail > head-1)
		return '\0';
	else return list[tail];
}*/

// look at what would dequeue would return
AudioStream* StreamQueue::peek(){
	if(tail > head-1)
		return nullptr;
	else return list[tail];
}

int StreamQueue::getSize(){
	return head-tail;
}

bool StreamQueue::isEmpty(){
	return head-tail == 0;
}

void StreamQueue::report(){
	printf("Objects: %d,  Tail: %d,  Head: %d,  Minsize: %d,  Currentsize: %d\n", head-tail, tail, head, minsize, arraysize);
}