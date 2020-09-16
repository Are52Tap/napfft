#include "fft.h"

class CooleyTukeyFFT : FFT{
	public:
		DComplexFunction fft(DRealFunction dts);
	private:
		void fft_ct(CArray& x);
};