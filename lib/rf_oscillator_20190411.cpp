# include "..\include\netxpto_20180815.h"
# include "..\include\rf_oscillator_20190411.h"

# include <algorithm>
# include <complex>
# include <iostream>
# include <fstream>
# include <random>

using namespace std;

void RfOscillator::initialize(void)
{
	outputSignals[0]->setSamplingPeriod(samplingPeriod);
	outputSignals[0]->setSymbolPeriod(symbolPeriod);

	outputSignals[1]->setSamplingPeriod(samplingPeriod);
	outputSignals[1]->setSymbolPeriod(symbolPeriod);

}


bool RfOscillator::runBlock(void)
{
	int space1 = outputSignals[0]->space();
	int space2 = outputSignals[1]->space();
	int space = min(space1, space2);
	int process = space;

	if (process == 0) return false;

	double dt = outputSignals[0]->getSamplingPeriod();

	for (int i = 0; i < process; i++)
	{
		phase = phase + 2*PI*rfFrequency * dt;
		t_complex out = (rfAmplitude)*exp(t_complex(0,exponentialSign*phase));
		outputSignals[0]->bufferPut((t_real)out.real());
		outputSignals[1]->bufferPut((t_real)out.imag());
	}

	return true;

}