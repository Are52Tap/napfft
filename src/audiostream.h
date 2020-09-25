#pragma once

#include "streamformat.h"
#include "audiosample.h"

#include <string>
#include <iostream>
#include <fstream>

// Consider making this an interface!

class AudioStream{
    private:
        StreamFormat sf;
        const AudioSample* asample = nullptr;
        double seconds = -1;

        
    public:
        AudioStream(StreamFormat sf);
        int load(const std::string& path);
        int convert();
        inline double getDuration(){return seconds;}
        int getSamples();
        inline StreamFormat& getStreamFormat(){return sf;};
        inline const AudioSample* getSample(){return asample;}

        AudioSample* getSample(StreamFormat& sf, double from, double to){
            return nullptr; //unimplemented
        }

        AudioSample* getSample(StreamFormat& sf, uint64_t from, uint64_t){
            return nullptr; //unimplemented
        }

};