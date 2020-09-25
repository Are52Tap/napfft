#pragma once

#include "dft.h"


/**
 * Input will either be an audio stream, supporting relative (PlaybackStream) and absolute origin (current AudioStream).
 */
class FourierAnalyser{
	private:
		DFT& dft;
	public:
		FourierAnalyser(DFT& dft/*, AudioStream interface??*/) : dft(dft){}

};