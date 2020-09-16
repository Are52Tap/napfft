#pragma once

#include "drealfunction.h"
#include "dcomplexfunction.h"

class DFT{
    public:
        virtual DComplexFunction dft(DRealFunction dts) = 0;
};