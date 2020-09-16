#pragma once
#include "streamformat.h"

#include "drealfunction.h"

/*extern "C" {
	#include <libswresample/swresample.h>
}*/

class AudioSample{
	private:
		uint8_t* array;
		uint64_t size;
		const StreamFormat format;
		uint64_t frames;
		int channels;
	public:
		inline uint64_t getFrames() const {return frames;}
		inline uint64_t getSize() const {return size;}
		unsigned int getFrames(uint8_t** subbuffer, uint64_t frame, unsigned int count) const;
		void getFilledFrames(uint8_t** subbuffer, uint64_t frame, unsigned int count) const;
		AudioSample(uint8_t* buffer, uint64_t size, const StreamFormat sampleFormat, uint64_t frames, unsigned int channels);
		~AudioSample();


};