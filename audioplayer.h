#pragma once

#include "streamqueue.h"
#include "notify_lock.h"
#include "playbackdevice.h"


#include <alsa/asoundlib.h>
#include <inttypes.h>

#include <thread>
#include <atomic>

typedef enum audioplayerstate{
    STOP,
    PAUSE,
    PLAY
} auioplayerstate_t;

class AudioPlayer{
    private:
        PlaybackDevice& pbd;
        StreamQueue* sq;

        //StreamFormat pf; // or
        //const StreamFormat pFormat; // or ^ 

        //PCMQueue* pcmq;
        std::atomic<audioplayerstate> state = {STOP};
        std::thread* thread;
		notify_lock* notifier = nullptr;
    public:
        AudioPlayer(PlaybackDevice& pbd);
        //AudioPlayer(PlaybackDevice pbd, StreamFormat playFormat);
        ~AudioPlayer();
        inline void stop(){
            state = STOP;
        }
        inline void pause(){
            state = PAUSE;
        }
        inline void start(){
            state = PLAY;
        }
        inline audioplayerstate getState(){
            return state;
        }
        
        void addToQueue(AudioStream* as);
        void playAsync();
        void playSync();
        void wait();
        void getSample(int relativeOffset, unsigned int length);

};