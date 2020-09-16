#include "audioplayer.h"

#include <thread>
#include <iostream>

AudioPlayer::AudioPlayer(PlaybackDevice& pbd) : pbd(pbd){
    sq = new StreamQueue(4);
}

/*AudioPlayer::AudioPlayer(PlaybackDevice pbd, StreamFormat playFormat) : AudioPlayer(), pf(playFormat){
}*/

AudioPlayer::~AudioPlayer(){
    // not thread safe
    int len = sq->getSize();
    AudioStream* as[len];
    len = sq->dequeue(as,len);
    for(int i = 0; i < len; i++)
        delete(as[i]);
    delete(sq);
}

void AudioPlayer::addToQueue(AudioStream* as){
    sq->enqueue(as);
}


void AudioPlayer::playAsync(){
	if(notifier != nullptr)
        notifier->lock();
    start();
    pbd.setup();
    AudioStream* asptr = sq->peek();
    if(asptr != nullptr){
        if(pbd.preparePlayback(asptr->getStreamFormat()) < 0) stop();
    }
    uint8_t* buffer;
    uint64_t frame = 0;
    while(state != STOP){
        if(state == PAUSE){
			std::this_thread::yield();
    		std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } else{
            if(asptr == nullptr) break;
            const AudioSample& as = *(asptr->getSample());
            // TODO: don't copy into a buffer, get the byte buffer itself and max length then send that
            unsigned int rframes = as.getFrames(&buffer, frame, 128);
            //std::cout << "rframes: " << rframes << std::endl;
            if(frame >= as.getFrames()){
                std::cout << "All written. Waiting to be complete." << std::endl;
	            pbd.drain();
                sq->dequeue();
                if(asptr != nullptr)
                    delete(asptr);
                asptr = nullptr;
                if(sq->peek() != nullptr){
                    asptr = sq->peek();
                    frame = 0;
                    if(pbd.preparePlayback(asptr->getStreamFormat()) < 0) break;
                }
            } else { 
                int wframes = pbd.write(buffer, rframes);
                //std::cout << "wframes: " << wframes << std::endl;
                free(buffer);
                if(wframes < 0) break;
                else frame+=wframes;
                //std::cout << "frames: " << frame << std::endl << std::endl;
                //std::cout << std::endl;
            }
        }
    }
    stop();
	if(notifier != nullptr)
        notifier->unlock();
}

void AudioPlayer::playSync(){
    notifier = new notify_lock;
    thread = new std::thread(&AudioPlayer::playAsync, this);
    thread->detach();
}

void AudioPlayer::wait(){
    notifier->wait();
}

void AudioPlayer::getSample(int relativeOffset, unsigned int length){

}
