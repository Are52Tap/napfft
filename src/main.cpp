#include "audiostream.h"
#include "streamformat.h"
#include "audioplayer.h"
#include "playbackdevice.h"
//#include "fft.h"

#include <iostream>
#include <string>

#define PCM_FORMAT PCM_FORMAT_S32
#define PCM_ACCESS PCM_ACCESS_INTERLEAVED

int main(int argc, char const *argv[]){
	StreamFormat sf(PCM_FORMAT, PCM_ACCESS);
	AudioStream& as = *(new AudioStream(sf));
	if(as.load(argv[1]) < 0) return -1;
	std::cout << "Song duration: " << as.getDuration() << std::endl;
	//StreamFormat pf(PCM_FORMAT, PCM_ACCESS, 144000);
	PlaybackDevice pd("default");
	AudioPlayer ap(pd);
		//ap.pause();
		ap.addToQueue(&as);
		//ap.playAsync();
			std::cout << "Start Play:" << std::endl;
		ap.playSync();
			std::cout << "Start Wait: " << std::endl;
		ap.wait();
			std::cout << "End Wait: " << std::endl;
	return 0;
}