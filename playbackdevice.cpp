#include "playbackdevice.h"

#define PCM_DEVICE "default"
#include <iostream>

PlaybackDevice::PlaybackDevice(std::string name){
	int err;
	if ((err = snd_pcm_open(&pcm_handle, name.c_str(), SND_PCM_STREAM_PLAYBACK, 0)) < 0){
		std::cerr << "Could not open PCM device \"" << name << "\". " << snd_strerror(err) << std::endl;
		//snd_pcm_hw_params_free(params); // Hasn't been allocated?
	}
}

PlaybackDevice::~PlaybackDevice(){
	letdown();
	snd_pcm_close(pcm_handle);
	if(pf != nullptr)
		delete(pf);
}

void PlaybackDevice::setup(){
	int err;
	//snd_pcm_hw_params_alloca(&params); // do not use!!!! only for in scope!
	if ((err = snd_pcm_hw_params_malloc(&params)) != 0){
		std::cerr << "Could not allocate params. " << snd_strerror(err) << std::endl;
		return;
	}
	if ((err = snd_pcm_hw_params_any(pcm_handle, params)) < 0){
		std::cerr << "Could not set default PCM parameters. " << snd_strerror(err) << std::endl;
	}
}

void PlaybackDevice::letdown(){
	if(params != nullptr)
		snd_pcm_hw_params_free(params);
}

int PlaybackDevice::preparePlayback(StreamFormat& pf){
	int err;

	/**
	 * This function is similar to source code written by:
	 * 
	 * Copyright (C) 2009 Alessandro Ghedini <alessandro@ghedini.me>
	 * "THE BEER-WARE LICENSE" (Revision 42)
	 * https://gist.github.com/ghedo/963382/
	 * - The comments in particular are reminent
	 * 
	 * Paul Davis' "A Minimal Playback Program" - A Tutorial on Using the ALSA Audio API
	 * GPL Public License
	 * http://equalarea.com/paul/alsa-audio.html
	 * 
	 * Alsa-Projects test/pcm.c
	 * https://www.alsa-project.org/alsa-doc/alsa-lib/_2test_2pcm_8c-example.html
	 * 
	 * 
	 * Of these, I honestly have no recollection who deserves the most credit. Or if my work
	 * is original enough to be distinct, because there is only a few ways to do this type 
	 * of code and much are very similar to Alsa-Project's example test/pmc.c code
	 * 
	 * /



	/* Set parameters */
	if ((err = snd_pcm_hw_params_set_format(pcm_handle, params, pf.getAlsaFormat())) < 0){
		std::cerr << "Could not set PCM format. " << snd_strerror(err) << std::endl;
		return -1;
	}

	if ((err = snd_pcm_hw_params_set_access(pcm_handle, params, pf.getAlsaAccess())) < 0){
		std::cerr << "Could not set PCM access. " << snd_strerror(err) << std::endl;
		return -1;
	}

	if ((err = snd_pcm_hw_params_set_channels(pcm_handle, params, pf.getChannelCount())) < 0){
		std::cerr << "Could not set PCM channel count. " << snd_strerror(err) << std::endl;
		return -1;
	}

	unsigned int fr = pf.getFrameRate();
	if ((err = snd_pcm_hw_params_set_rate_near(pcm_handle, params, (unsigned int*) &fr, 0)) < 0){
		std::cerr << "Could not set PCM frame rate. " << snd_strerror(err) << std::endl;
		return -1;
	}
	
	this->pf = new StreamFormat(pf,fr);

	/* Write parameters */
	if ((err = snd_pcm_hw_params(pcm_handle, params)) < 0){
		std::cerr << "Could not set PCM parameters. " << snd_strerror(err) << std::endl;
		return -1;
	}
	return 0;
}

int PlaybackDevice::write(const uint8_t* buffer, snd_pcm_uframes_t length){
	int len;
	if ((len = snd_pcm_writei(pcm_handle, buffer, length)) == -EPIPE) {
		fprintf(stdout,"XRUN.\n");
		snd_pcm_prepare(pcm_handle);
		return 0;
	} else if (len < 0) {
		std::cerr << "Could not write to PCM device. " << snd_strerror(len) << std::endl;
		return -1;
	}
	return len;
}


void PlaybackDevice::drain(){
	snd_pcm_drain(pcm_handle);
}