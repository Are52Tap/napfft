#include "audiosample.h"

#include <iostream>
//#include <stdio.h>

AudioSample::AudioSample(uint8_t* buffer, uint64_t size, const StreamFormat sampleFormat, uint64_t frames, unsigned int channels) : 
    array(buffer),
    size(size),
    //DRealFunction(buffer,size),
    format(sampleFormat),
    frames(frames),
    channels(channels){
        assert(size%(channels*frames)==0);
    }


unsigned int AudioSample::getFrames(uint8_t** subbuffer, uint64_t frame, unsigned int count) const{
    //std::cout << "Size: " << size << std::endl;
    //std::cout << "Frames: " << frames << std::endl;
    if(count == 0) return 0;
    unsigned int bytes_frame = size/frames; // bytes per frame
    //std::cout << "B/F: " << bytes_frame << std::endl;
    unsigned int length = count*bytes_frame;
    //std::cout << "Length: " << length << std::endl;
    *subbuffer = (uint8_t*) calloc(count, length*sizeof(uint8_t));
    //std::cout << "pre: f*b_f: " << frame*bytes_frame << std::endl;
    unsigned int i = 0;
    for(;i < length; ++i){
        unsigned int pos = frame*bytes_frame+i;
        if(pos>size) break;
        (*subbuffer)[i] = array[pos];
        //printf("%u,",i);
    }
    //std::cout << "post: f*b_f+i: " << frame*bytes_frame+i << std::endl;
    //std::cout << "i/bytes_frame: " << (i/bytes_frame) << std::endl;
    return i/bytes_frame;
}

void AudioSample::getFilledFrames(uint8_t** subbuffer, uint64_t frame, unsigned int count) const{
    unsigned int bytes_frame = size/frames; // bytes per frame
    unsigned int length = count*bytes_frame;
    *subbuffer = (uint8_t*) calloc(count, length*sizeof(uint8_t));
    unsigned int i = 0;
    for(; i < length; ++i){
        unsigned int pos = frame*bytes_frame+i;
        if(pos>size) break;
        (*subbuffer)[i] = array[pos];
    }
    for(; i < length; ++i)
        *subbuffer[i] = 0; // fill with silence
}