#pragma once

#include <alsa/asoundlib.h>
extern "C" { 
	#include <libavutil/samplefmt.h>
}
#include <inttypes.h>
#include <stdexcept>

typedef enum pcm_format{
	PCM_FORMAT_AUTO,
	PCM_FORMAT_U8,
	PCM_FORMAT_S16,
	PCM_FORMAT_S32,
	PCM_FORMAT_FLT,
	PCM_FORMAT_DBL,
	PCM_FORMAT_UNSUPPORTED
} pcm_format_t;

typedef enum pcm_access{
	PCM_ACCESS_AUTO,
	PCM_ACCESS_INTERLEAVED,
	PCM_ACCESS_NONINTERLEAVED,
	PCM_ACCESS_NONPLANAR = PCM_ACCESS_INTERLEAVED,
	PCM_ACCESS_PLANAR = PCM_ACCESS_NONINTERLEAVED,
	PCM_ACCESS_UNSUPPORTED
} pcm_access_t;

typedef enum channel_mode{
	CHANNEL_MODE_DETECT,
	CHANNEL_MODE_AUTO,
	CHANNEL_MODE_BEST,
	CHANNEL_MODE_CONST
} channel_mode_t;

class StreamFormat{
	private:
		enum pcm_format pcmFormat = PCM_FORMAT_AUTO;
		enum pcm_access pcmAccess = PCM_ACCESS_AUTO;
		enum channel_mode channelMode = CHANNEL_MODE_AUTO;
		/*enum _snd_pcm_format alsaFormat;
		enum _snd_pcm_access alsaAccess;
		enum AVSampleFormat avFormat;*/
		int channelCount = -1;
		uint64_t channelLayout;
		uint32_t frameRate = 0;

	public:

		// Get Functions

		inline enum _snd_pcm_format getAlsaFormat() const{
			return getAlsaFormat(pcmFormat);
		}
		inline enum _snd_pcm_access getAlsaAccess() const{
			return getAlsaAccess(pcmAccess);
		}
		inline enum AVSampleFormat getAVFormat() const{
			return getAVFormat(pcmFormat,pcmAccess);
		}
		inline enum pcm_format getFormat() const{
			return pcmFormat;
		}
		inline enum pcm_access getAccess() const{
			return pcmAccess;
		}
		inline enum channel_mode getChannelMode() const{
			return channelMode;
		}
		inline int getChannelCount() const{
			return channelCount;
		}
		inline uint64_t getChannelLayout() const{
			return channelLayout;
		}
		inline uint32_t getFrameRate() const{
			return frameRate;
		}
		
		// Upgrade Functions

		void upgradeAccess(const enum pcm_access access);
		inline void upgradeAccess(const enum _snd_pcm_access access){
			return upgradeAccess(getAccess(access));
		}

		void upgradeFormat(const enum pcm_format format);
		inline void upgradeFormat(const enum AVSampleFormat format){
			upgradeAccess(getAccess(format));
			return upgradeFormat(getFormat(format));
		}
		inline void upgradeFormat(const enum _snd_pcm_format format, const enum _snd_pcm_access access){
			upgradeAccess(getAccess(access));
			return upgradeFormat(getFormat(format));
		}

		void upgradeChannels(int channelCount, uint64_t channelLayout);
		void upgradeRate(uint32_t rate);

		// ALSA & THIS

		static enum _snd_pcm_format getAlsaFormat(enum pcm_format format);
		static enum pcm_format getFormat(enum _snd_pcm_format format);
		static enum _snd_pcm_access getAlsaAccess(enum pcm_access access);
		static enum pcm_access getAccess(enum _snd_pcm_access access);

		// FFMPEG & THIS

		static enum AVSampleFormat getAVFormat(enum pcm_format format, enum pcm_access access);
		static enum pcm_format getFormat(AVSampleFormat format);
		static enum pcm_access getAccess(AVSampleFormat format);

		// FFMPEG & ALSA

		static enum _snd_pcm_format getAlsaFormat(enum AVSampleFormat format);
		static enum _snd_pcm_access getAlsaAccess(enum AVSampleFormat format);
		static enum AVSampleFormat getAVFormat(enum _snd_pcm_format format, enum _snd_pcm_access access);

		// CONSTRUCTORS

		StreamFormat(const enum pcm_format format, const enum pcm_access access, const uint32_t rate) : 
			pcmFormat(format), pcmAccess(access), frameRate(rate){}

		StreamFormat(const enum _snd_pcm_format alsa_format, const enum _snd_pcm_access alsa_access) : 
			pcmFormat(getFormat(alsa_format)),
			pcmAccess(getAccess(alsa_access)){}

		StreamFormat(const enum AVSampleFormat av_format) : 
			pcmFormat(getFormat(av_format)),
			pcmAccess(getAccess(av_format)){}

		StreamFormat(const enum pcm_format format, const enum pcm_access access) : 
			pcmFormat(format), pcmAccess(access){}

		StreamFormat(const StreamFormat &p2, uint32_t rate=0){
			if(rate == 0)
				this->frameRate = p2.frameRate;
			else
				this->frameRate = rate;
		}

};
