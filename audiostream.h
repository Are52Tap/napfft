#pragma once

#include "streamformat.h"
#include "audiosample.h"

#include <string>
#include <iostream>
#include <fstream>

class AudioStream{
    private:
        StreamFormat* sf = nullptr;
        const AudioSample* asample = nullptr;
        double seconds = -1;

        
    public:
        AudioStream(StreamFormat& sf);
        int load(const std::string& path);
        int convert();
        inline double getDuration(){return seconds;}
        int getSamples();
        inline StreamFormat& getStreamFormat(){return *sf;};
        inline const AudioSample* getSample(){return asample;}
};