#pragma once

#include "audiostream.h"
#include "queue.h"
#include "mutex_wrapper.h"
#include <mutex>

class StreamQueue : public Queue<AudioStream>{
    private: 
        std::mutex wafer;
	protected:
		void check(Mutex waffel);
	public:
		StreamQueue(int size);
		void enqueue(AudioStream** array, int length);
		void enqueue(AudioStream* array);
		//void moveHead(int shift);
		int dequeue(AudioStream** fq, int length);
		AudioStream* dequeue();
		AudioStream* peek();
		bool isEmpty();
		int getSize();
		void report();

};
