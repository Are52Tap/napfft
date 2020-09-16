#include "audiostream.h"
#include "streamformat.h"
 
extern "C" { 
	#include <libavutil/opt.h>
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswresample/swresample.h>
}

#include <cassert>

AudioStream::AudioStream(StreamFormat& format) : sf(&format){
}

int AudioStream::load(const std::string& path){
	/**
	 * Segments of this function, particularly the beginning of getting audio format are very similar 
	 * and almost verbatim for some parts to my main resource in developing this section work.
	 * Mathieu Rodic's "Libavcodec tutorial":
	 * - getting format from file
	 * - getting stream from file
	 * - opening stream decoder
	 * - preparing resampler
	 * 
	 * However major differences include:
	 * + finding decoder and allocating context
	 * + preparing resampler layouted non-linearly
	 * 
	 * 
	 * Another important modification was made for deprecated avcdoec_decode_audio4() functionality
	 * I was able to get this working by following an explanation by Luca Barbato entitled 
	 * ["New AVCodec API"](#https://blogs.gentoo.org/lu_zero/2016/03/29/new-avcodec-api/) on how
	 * to replace the old avcodec_decode_audio4 functionality with the newer API functions involving:
	 * + sending decoding packets; then
	 * + receiving frames from said packets
	 * 
	 */



	StreamFormat& tSF = *sf;

	int streamIndex = -1;
	AVFrame* frame;
	AVStream* stream;
	AVCodecParameters* codecpar;
	AVCodec* avcodec;
	AVCodecContext* avctx;
	SwrContext* swr;
	AVPacket packet;
	int ret = -1;
	int err;
	uint8_t* buffer = nullptr;
	uint64_t size = 0;
	uint32_t frames = 0;

	// get format
	AVFormatContext* format = avformat_alloc_context();

	if(avformat_open_input(&format, path.c_str(), NULL, NULL) != 0){
		std::cerr << "Could not open file " << path << std::endl;
		goto cleanup1;
	}
	if (avformat_find_stream_info(format, NULL) < 0) {
		std::cerr << "Could not find stream info from file " << path << std::endl;
		goto cleanup1;
	}

	

	// get stream
	for(unsigned int i = 0; i < format->nb_streams; i++){
		if(format->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
			streamIndex = i;
		}
		// Allow options for playing audio from video
	}
	if(streamIndex == -1){
		std::cerr << "Could not find an audio stream from file " << path << std::endl;
		goto cleanup1;
	}
	stream = format->streams[streamIndex]; //AVStream* 

	// get codec context
	codecpar = stream->codecpar; //AVCodecParameters*
	avcodec = avcodec_find_decoder(codecpar->codec_id); //AVCodec*
	avctx = avcodec_alloc_context3(avcodec); //AVCodecContext* 
	avcodec_parameters_to_context(avctx,codecpar);
	if (avcodec_open2(avctx, avcodec, NULL) != 0) {
		std::cerr << "Coult not retrieve or open a decoder for stream from file " << path << std::endl;
		goto cleanup2;
	}
	// be sure to close later
	if(avcodec_is_open(avctx) == 0){
		std::cerr << "Could not validate open decoder for stream from file " << path << std::endl;
		goto cleanup2;
	}

	// prepare resampler
	swr = swr_alloc(); //SwrContect*

	// Channel Count & Layout
	av_opt_set_int(swr, "in_channel_count",  avctx->channels, 0);
	av_opt_set_int(swr, "in_channel_layout",  avctx->channel_layout, 0);
	switch(tSF.getChannelMode()){
		/*case CHANNEL_MODE_BEST: {
			break;
		}
		case CHANNEL_MODE_DETECT: {
			int chan = getDeviceChannels();
			uint64_t chanlay = getDeviceLayout();
			av_opt_set_int(swr, "out_channel_count", chan, 0);
			av_opt_set_int(swr, "out_channel_layout", chanlay, 0);
			sf.upgradeChannels(chan, chanlay);
			break;
		}*/
		case CHANNEL_MODE_CONST: {
			av_opt_set_int(swr, "out_channel_count", tSF.getChannelCount(), 0);
			av_opt_set_int(swr, "out_channel_layout", tSF.getChannelLayout(), 0);
			break;
		}
		default /*CHANNEL_MODE_AUTO*/: {
			av_opt_set_int(swr, "out_channel_count", avctx->channels, 0);
			av_opt_set_int(swr, "out_channel_layout", avctx->channel_layout, 0);
			tSF.upgradeChannels(avctx->channels,avctx->channel_layout);
			break;
		}
	}
	// Sample Rate
	av_opt_set_int(swr, "in_sample_rate", avctx->sample_rate, 0);
	if(tSF.getFrameRate() == 0){
		av_opt_set_int(swr, "out_sample_rate", avctx->sample_rate, 0);
		tSF.upgradeRate(avctx->sample_rate);
	}
	else{
		av_opt_set_int(swr, "out_sample_rate", tSF.getFrameRate(), 0);
	}
	// Sample Format
	av_opt_set_sample_fmt(swr, "in_sample_fmt",  avctx->sample_fmt, 0);
	if(tSF.getFormat() == PCM_FORMAT_AUTO || tSF.getFormat() == PCM_FORMAT_UNSUPPORTED){
		av_opt_set_sample_fmt(swr, "out_sample_fmt", avctx->sample_fmt, 0);
		tSF.upgradeFormat(avctx->sample_fmt);
	}
	else{
		av_opt_set_sample_fmt(swr, "out_sample_fmt", tSF.getAVFormat(), 0);
	}
	// initialize resampler
	swr_init(swr);
	if (!swr_is_initialized(swr)) {
		std::cerr << " Could not initialize resampler from file " << path << std::endl;
		goto cleanup2;
	}


	
	// prepare to read data
	av_init_packet(&packet);
	frame = av_frame_alloc();
	if (!frame) {
		std::cerr << "Could not allocate frame from file " << path << std::endl;
		goto cleanup3;
	}

	// stream decoding loop
	while((err = av_read_frame(format, &packet)) == 0){
		// send decoding packets
		if((err = avcodec_send_packet(avctx,&packet)) < 0){
			if(err == AVERROR_EOF)
				break; // end of packets
			switch(err){
				case AVERROR(EAGAIN): break; // Need to receive_frame
				//case AVERROR_EOF: continue;
				case AVERROR(ENOMEM): { // decoding error
					std::cerr << "Could not decode (1) stream from file " << path << std::endl;
					goto cleanup3;
				}
				case AVERROR(EINVAL): { // assume to be error wants flush
					std::cerr << "Could not decode (2) stream from file " << path << std::endl;
					goto cleanup3;
				}
				default: {
					std::cerr << "Untracked (1) decoding error from file " << path << std::endl;
					goto cleanup3;
				}
			}
		}
		// receive frames from sent packets
		if((err = avcodec_receive_frame(avctx,frame)) < 0){
			if(err == AVERROR_EOF)
				break; // end of packets
			switch(err){
				case AVERROR(EAGAIN): break; // Need to send_packet
				case AVERROR(EINVAL): { // assume to be error wants flush
					std::cerr << "Could not decode (3) stream from file " << path << std::endl;
					goto cleanup3;
				}
				default: {
					std::cerr << "Untracked (2) decoding error from file " << path << std::endl;
					goto cleanup3;
				}
			}
		}
		uint8_t* tBuffer;
		int lineSize;
		if (av_samples_alloc(&tBuffer, &lineSize, frame->channels, frame->nb_samples, tSF.getAVFormat(), 1) < 0) {
			std::cerr << "Coult not retrieve or open a decoder for stream from file " << path << std::endl;
			goto cleanup3;
		}
		int frameCount = swr_convert(swr, &tBuffer, frame->nb_samples, (const uint8_t**) frame->data, frame->nb_samples);
		#ifndef __INTELLISENSE__
		assert(frameCount==(frame->nb_samples));
		//assert((lineSize%(frameCount*(frame->channels)))==0);
		#endif
		//int byteSize = lineSize/(frameCount*frame->channels);
		buffer = (uint8_t*) realloc(buffer, size + (uint64_t)lineSize);
		memcpy(buffer+size,tBuffer,lineSize);
		frames+=(uint64_t)frameCount;
		size+=(uint64_t)lineSize;
	}
	if(err != AVERROR_EOF){
		std::cerr << "Unable to decode stream succesfully to end of file " << path << std::endl;
			switch(err){
				case AVERROR(EAGAIN): { // decoding error
					std::cerr << "Could not decode (0) stream from file " << path << std::endl;
					goto cleanup3;
				}
				case AVERROR(ENOMEM): { // decoding error
					std::cerr << "Could not decode (1) stream from file " << path << std::endl;
					goto cleanup3;
				}
				case AVERROR(EINVAL): { // assume to be error wants flush
					std::cerr << "Could not decode (2) stream from file " << path << std::endl;
					goto cleanup3;
				}
				default: {
					std::cerr << "Untracked (1) decoding error from file " << path << std::endl;
					goto cleanup3;
				}
			}
		goto cleanup3;
	}

	asample = new AudioSample(buffer, size, tSF, frames, frame->channels);
	seconds = ((double)(frames))/((double)tSF.getFrameRate());

	// Success
	ret = 0;
	// Unwinding cleanup for failed operations
	cleanup3:
		av_frame_free(&frame);
	cleanup2:
		avcodec_close(avctx);
	cleanup1:
		avformat_free_context(format);
	return ret;
}