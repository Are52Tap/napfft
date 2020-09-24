#pragma once

#include "drealfunction.h"
#include "drealfunction.h"
#include "dcomplexfunction.h"


DRealFunction getRealFromComplex(DComplexFunction& dcf){
    uint64_t size = dcf.getSize();
    double* reals = (double*) calloc(size,sizeof(double));
    for (uint64_t i = 0; i < size; i++){
        reals[i] = dcf.getArray()[i].real();
    }
    return DRealFunction(reals,size);
}

DComplexFunction getComplexFromReal(DRealFunction& drf){
    uint64_t size = drf.getSize();
    CArray* c = new CArray(size);
    for (uint64_t i = 0; i < size; i++){
        c[i] = Complex(drf.getArray()[i],0);
    }
    return DComplexFunction(c);
}

