#include "streamformat.h"

// Upgrade Functions

void StreamFormat::upgradeAccess(const enum pcm_access access){
	if(pcmAccess == PCM_ACCESS_AUTO){
		if(access != PCM_ACCESS_UNSUPPORTED && access != PCM_ACCESS_AUTO){
			pcmAccess = access;
		} else{
			throw std::invalid_argument("Unable to upgrade pcm_access. Invalid argument");
		}
	} else{
		throw std::invalid_argument("Unable to upgrade pcm_access. Invalid state");
	}
}

void StreamFormat::upgradeFormat(const enum pcm_format format){
	if(pcmFormat == PCM_FORMAT_AUTO){
		if(format != PCM_FORMAT_UNSUPPORTED && format != PCM_FORMAT_AUTO){
			pcmFormat = format;
		} else{
			throw std::invalid_argument("Unable to upgrade pcm_format. Invalid argument");
		}
	} else{
		throw std::invalid_argument("Unable to upgrade pcm_format. Invalid state");
	}
}

void StreamFormat::upgradeChannels(int channelCount, uint64_t channelLayout){
	// if !complyingChannelLayout(int, uint64_t) throw;
	if(channelMode != CHANNEL_MODE_CONST){
		this->channelCount = channelCount;
		this->channelLayout = channelLayout;
		channelMode = CHANNEL_MODE_CONST;
	} else{
		throw std::invalid_argument("Unable to upgrade channel count and layout. Invalid state");
	}
}

void StreamFormat::upgradeRate(uint32_t rate){
	// if !complyingRate(uint32_t) throw;
	if(frameRate == 0){
		this->frameRate = rate;
	} else{
		throw std::invalid_argument("Unable to upgrade sampling rate. Invalid state");
	}
}

// ALSA & THIS - STATIC

enum _snd_pcm_format StreamFormat::getAlsaFormat(enum pcm_format format){
	switch(format){
		case PCM_FORMAT_U8: return SND_PCM_FORMAT_U8;
		case PCM_FORMAT_S16: return SND_PCM_FORMAT_S16;
		case PCM_FORMAT_S32: return SND_PCM_FORMAT_S32;
		case PCM_FORMAT_FLT: return SND_PCM_FORMAT_FLOAT;
		case PCM_FORMAT_DBL: return SND_PCM_FORMAT_FLOAT64;
		default: return SND_PCM_FORMAT_UNKNOWN;
	}
}

enum pcm_format StreamFormat::getFormat(enum _snd_pcm_format format){
	switch(format){
		case SND_PCM_FORMAT_U8: return PCM_FORMAT_U8;
		case SND_PCM_FORMAT_S16: return PCM_FORMAT_S16;
		case SND_PCM_FORMAT_S32: return PCM_FORMAT_S32;
		case SND_PCM_FORMAT_FLOAT: return PCM_FORMAT_FLT;
		case SND_PCM_FORMAT_FLOAT64: return PCM_FORMAT_FLT;
		default: return PCM_FORMAT_UNSUPPORTED;
	}
}

enum _snd_pcm_access StreamFormat::getAlsaAccess(enum pcm_access access){
	switch(access){
		case PCM_ACCESS_INTERLEAVED: return SND_PCM_ACCESS_RW_INTERLEAVED;
		case PCM_ACCESS_NONINTERLEAVED: return SND_PCM_ACCESS_RW_NONINTERLEAVED;
		default: break;
	}
	throw std::invalid_argument("Unsupported pcm_access map to SND_PCM_ACCESS_");
}

enum pcm_access StreamFormat::getAccess(enum _snd_pcm_access access){
	switch(access){
		case SND_PCM_ACCESS_RW_INTERLEAVED: return PCM_ACCESS_INTERLEAVED;
		case SND_PCM_ACCESS_RW_NONINTERLEAVED: return PCM_ACCESS_NONINTERLEAVED;
		default: return PCM_ACCESS_UNSUPPORTED;
	}
}

// FFMPEG & THIS - STATIC

enum AVSampleFormat StreamFormat::getAVFormat(enum pcm_format format, enum pcm_access access){	
	if(access == PCM_ACCESS_NONINTERLEAVED){
		switch(format){
			case PCM_FORMAT_U8: return AV_SAMPLE_FMT_U8P;
			case PCM_FORMAT_S16: return AV_SAMPLE_FMT_S16P;
			case PCM_FORMAT_S32: return AV_SAMPLE_FMT_S32;
			case PCM_FORMAT_FLT: return AV_SAMPLE_FMT_FLTP;
			case PCM_FORMAT_DBL: return AV_SAMPLE_FMT_DBLP;
			default: break;
			//case SND_PCM_FORMAT_S64: return AV_SAMPLE_FMT_S64P;
		}
	} else if(access == PCM_ACCESS_INTERLEAVED){
		switch(format){
			case PCM_FORMAT_U8: return AV_SAMPLE_FMT_U8;
			case PCM_FORMAT_S16: return AV_SAMPLE_FMT_S16;
			case PCM_FORMAT_S32: return AV_SAMPLE_FMT_S32;
			case PCM_FORMAT_FLT: return AV_SAMPLE_FMT_FLT;
			case PCM_FORMAT_DBL: return AV_SAMPLE_FMT_DBL;
			default: break;
			//case SND_PCM_FORMAT_S64: return AV_SAMPLE_FMT_S64;
		}
	} else{
		throw std::invalid_argument("Unsupported pcm_access with pcm_format map to AVSampleFormat");
	}
	throw std::invalid_argument("Unsupported pcm_format map to AVSampleFormat");
}

enum pcm_format StreamFormat::getFormat(AVSampleFormat format){
	switch(format){
		case AV_SAMPLE_FMT_U8: return PCM_FORMAT_U8;
		case AV_SAMPLE_FMT_S16: return PCM_FORMAT_S16;
		case AV_SAMPLE_FMT_S32: return PCM_FORMAT_S32;
		case AV_SAMPLE_FMT_FLT: return PCM_FORMAT_FLT;
		case AV_SAMPLE_FMT_DBL: return PCM_FORMAT_DBL;
		case AV_SAMPLE_FMT_U8P: return PCM_FORMAT_U8;
		case AV_SAMPLE_FMT_S16P: return PCM_FORMAT_S16;
		case AV_SAMPLE_FMT_S32P: return PCM_FORMAT_S32;
		case AV_SAMPLE_FMT_FLTP: return PCM_FORMAT_FLT;
		case AV_SAMPLE_FMT_DBLP: return PCM_FORMAT_DBL;
		default: return PCM_FORMAT_UNSUPPORTED;
	}
}

enum pcm_access StreamFormat::getAccess(AVSampleFormat format){
	switch(format){
		case AV_SAMPLE_FMT_U8: return PCM_ACCESS_INTERLEAVED;
		case AV_SAMPLE_FMT_S16: return PCM_ACCESS_INTERLEAVED;
		case AV_SAMPLE_FMT_S32: return PCM_ACCESS_INTERLEAVED;
		case AV_SAMPLE_FMT_FLT: return PCM_ACCESS_INTERLEAVED;
		case AV_SAMPLE_FMT_DBL: return PCM_ACCESS_INTERLEAVED;
		//case AV_SAMPLE_FMT_S64: return SND_PCM_ACCESS_RW_INTERLEAVED;
		case AV_SAMPLE_FMT_U8P: return PCM_ACCESS_INTERLEAVED;
		case AV_SAMPLE_FMT_S16P: return PCM_ACCESS_INTERLEAVED;
		case AV_SAMPLE_FMT_S32P: return PCM_ACCESS_INTERLEAVED;
		case AV_SAMPLE_FMT_FLTP: return PCM_ACCESS_INTERLEAVED;
		case AV_SAMPLE_FMT_DBLP: return PCM_ACCESS_INTERLEAVED;
		default: return PCM_ACCESS_UNSUPPORTED;
	}
}

// FFMPEG & ALSA - STATIC

enum _snd_pcm_format StreamFormat::getAlsaFormat(enum AVSampleFormat format){
	/*try{
		return getAlsaFormat(getFormat(format));
	} catch(std::invalid_argument ia){
		throw std::invalid_argument("Unsupported AVSampleFormat map to AlsaFormat");
	}*/
	// OR
	switch(format){
		case AV_SAMPLE_FMT_U8: return SND_PCM_FORMAT_U8;
		case AV_SAMPLE_FMT_S16: return SND_PCM_FORMAT_S16;
		case AV_SAMPLE_FMT_S32: return SND_PCM_FORMAT_S32;
		case AV_SAMPLE_FMT_FLT: return SND_PCM_FORMAT_FLOAT;
		case AV_SAMPLE_FMT_DBL: return SND_PCM_FORMAT_FLOAT64;
		//case AV_SAMPLE_FMT_S64: return SND_PCM_FORMAT_S64;
		case AV_SAMPLE_FMT_U8P: return SND_PCM_FORMAT_U8;
		case AV_SAMPLE_FMT_S16P: return SND_PCM_FORMAT_S16;
		case AV_SAMPLE_FMT_S32P: return SND_PCM_FORMAT_S32;
		case AV_SAMPLE_FMT_FLTP: return SND_PCM_FORMAT_FLOAT;
		case AV_SAMPLE_FMT_DBLP: return SND_PCM_FORMAT_FLOAT64;
		//case AV_SAMPLE_FMT_S64P: return SND_PCM_FORMAT_S64;
		default: return SND_PCM_FORMAT_UNKNOWN;
	}
}

enum _snd_pcm_access StreamFormat::getAlsaAccess(enum AVSampleFormat format){
	/*try{
		return getAVFormat(getFormat(format),getAccess(access));
	} catch(std::invalid_argument ia){
		throw std::invalid_argument("Unsupported AlsaAccess with AlsaFormat map to AVSampleFormat");
	}*/
	// OR
	switch(format){
		case AV_SAMPLE_FMT_U8: return SND_PCM_ACCESS_RW_INTERLEAVED;
		case AV_SAMPLE_FMT_S16: return SND_PCM_ACCESS_RW_INTERLEAVED;
		case AV_SAMPLE_FMT_S32: return SND_PCM_ACCESS_RW_INTERLEAVED;
		case AV_SAMPLE_FMT_FLT: return SND_PCM_ACCESS_RW_INTERLEAVED;
		case AV_SAMPLE_FMT_DBL: return SND_PCM_ACCESS_RW_INTERLEAVED;
		//case AV_SAMPLE_FMT_S64: return SND_PCM_ACCESS_RW_INTERLEAVED;
		case AV_SAMPLE_FMT_U8P: return SND_PCM_ACCESS_RW_NONINTERLEAVED;
		case AV_SAMPLE_FMT_S16P: return SND_PCM_ACCESS_RW_NONINTERLEAVED;
		case AV_SAMPLE_FMT_S32P: return SND_PCM_ACCESS_RW_NONINTERLEAVED;
		case AV_SAMPLE_FMT_FLTP: return SND_PCM_ACCESS_RW_NONINTERLEAVED;
		case AV_SAMPLE_FMT_DBLP: return SND_PCM_ACCESS_RW_NONINTERLEAVED;
		//case AV_SAMPLE_FMT_S64P: return SSND_PCM_ACCESS_RW_NONINTERLEAVED;
		default: break;
	}
	throw std::invalid_argument("Unsupported AVSampleFormat map to _snd_pcm_access");
}

enum AVSampleFormat StreamFormat::getAVFormat(enum _snd_pcm_format format, enum _snd_pcm_access access){
	/*try{
		return getAVFormat(getFormat(format),getAccess(access));
	} catch(std::invalid_argument ia){
		throw std::invalid_argument("Unsupported AlsaAccess with AlsaFormat map to AVSampleFormat");
	}*/
	// OR
	if(access == SND_PCM_ACCESS_RW_NONINTERLEAVED){
		switch(format){
			case SND_PCM_FORMAT_U8: return AV_SAMPLE_FMT_U8P;
			case SND_PCM_FORMAT_S16: return AV_SAMPLE_FMT_S16P;
			case SND_PCM_FORMAT_S32: return AV_SAMPLE_FMT_S32;
			case SND_PCM_FORMAT_FLOAT: return AV_SAMPLE_FMT_FLTP;
			case SND_PCM_FORMAT_FLOAT64: return AV_SAMPLE_FMT_DBLP;
			//case SND_PCM_FORMAT_S64: return AV_SAMPLE_FMT_S64P;
			default: break;
		}
	} else if(access == SND_PCM_ACCESS_RW_INTERLEAVED){
		switch(format){
			case SND_PCM_FORMAT_U8: return AV_SAMPLE_FMT_U8;
			case SND_PCM_FORMAT_S16: return AV_SAMPLE_FMT_S16;
			case SND_PCM_FORMAT_S32: return AV_SAMPLE_FMT_S32;
			case SND_PCM_FORMAT_FLOAT: return AV_SAMPLE_FMT_FLT;
			case SND_PCM_FORMAT_FLOAT64: return AV_SAMPLE_FMT_DBL;
			//case SND_PCM_FORMAT_S64: return AV_SAMPLE_FMT_S64;
			default: break;
		}
	} else {
		throw std::invalid_argument("Unsupported AlsaAccess with AlsaFormat map to AVSampleFormat");
	}
	throw std::invalid_argument("Unsupported AlsaFormat map to AVSampleFormat");
}