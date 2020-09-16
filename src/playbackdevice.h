#pragma once

#include "streamformat.h"

#include <alsa/pcm.h>
#include <iostream>
#include <mutex>

class PlaybackDevice{
    private:
    	snd_pcm_t* pcm_handle = nullptr;
	    snd_pcm_hw_params_t* params = nullptr;
        StreamFormat* pf = nullptr;
        std::mutex wafer;

    public:
        PlaybackDevice(const PlaybackDevice &p2) {std::cout << "Copy Constructor called" << std::endl;} 
        PlaybackDevice(std::string name);
        ~PlaybackDevice();
        //PlaybackDevice(std::string& name, StreamFormat pf);
        void setup();
        void letdown();
        int preparePlayback(StreamFormat& pf);
        int write(const uint8_t* buffer, snd_pcm_uframes_t length);
        void drain();

        

};