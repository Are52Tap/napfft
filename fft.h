#pragma once

#include "dft.h"

class FFT : private DFT{
	private:
		inline DComplexFunction dft(DRealFunction dts){return fft(dts);}
	public:
		virtual DComplexFunction fft(DRealFunction dts) = 0;
};