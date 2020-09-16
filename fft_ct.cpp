#include "fft_ct.h"

#include <iostream>
#include <math.h>


DComplexFunction CooleyTukeyFFT::fft(DRealFunction dts){
	//CArray& x = dts.getArray();

}

void CooleyTukeyFFT::fft_ct(CArray& x){
	const int half = x.size()/2;
	if(half < 1) return;

	CArray even = x[std::slice(0,half,2)];
	CArray odd = x[std::slice(1,half,2)];

	fft_ct(even);
	fft_ct(odd);

	for(int k = 0 ; k < half ; k++)
	{
		Complex t = polar(1.0,-1.0 * M_PI * k / half) * odd[k];
		x[k] = even[k] + t;
		x[k+half] = even[k] - t;
	}

}