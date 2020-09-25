#include "fft_ct.h"
#include "functions.h"

#include <iostream>
#include <math.h>


DComplexFunction CooleyTukeyFFT::fft(DRealFunction dts){
	//double* d = dts.getArray()
	DComplexFunction dc = getComplexFromReal(dts);
	fft_ct(dc.getArray());
	return dc;
}



/**
 * This function is from Rosetta Code FDLv1.2
 * https://rosettacode.org/wiki/Fast_Fourier_transform#C.2B.2B
 */

void CooleyTukeyFFT::fft_ct(CArray& x){
	const int half = x.size()/2;
	if(half < 1) return;

	CArray even = x[std::slice(0,half,2)];
	CArray odd = x[std::slice(1,half,2)];

	fft_ct(even);
	fft_ct(odd);

	for(int k = 0 ; k < half ; k++){
		Complex t = std::polar(1.0,-1.0 * M_PI * k / half) * odd[k];
		x[k] = even[k] + t;
		x[k+half] = even[k] - t;
	}

}