# include "phase_mismatch_compensation_20190114.h"

# include <algorithm>
# include <complex>
# include <iostream>
# include <fstream>
# include <random>
# include <numeric>

using namespace std;


void PhaseMismatchCompensation::initialize(void) {

	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());

	samplesForEstimation.resize(numberOfSamplesForEstimation);
	delayLine.resize(pilotRate + 1);

}


bool PhaseMismatchCompensation::runBlock(void) {

	double symbolPeriod = inputSignals[0]->getSymbolPeriod();
	long int ready = inputSignals[0]->ready();
	long int space = outputSignals[0]->space();

	long int process = min(ready, space);


	if (process == 0) return false;
	t_complex in;
	t_complex out;

	if (mode == SequentialMode)
	{
		if (firstRun) {
			firstRun = false;
			int process0 = min(process, pilotRate + 1 - count);

			for (int i = 0; i < process0; i++) {
				inputSignals[0]->bufferGet(&in);
				delayLine[auxQ] = in;
				auxQ++;
				if (auxQ == pilotRate + 1) auxQ = 0;

				ready--;
				if (count % pilotRate == 0)
				{
					samplesForEstimation[auxP] = in;
					auxP++;	
					if (auxP == 2) auxP = 0;
				}
				count++;
			}

			phase = (arg(samplesForEstimation[1]) + arg(samplesForEstimation[0])) / 2.0;
			for (int i = 0; i < pilotRate + 1; i++)
			{
				out = delayLine[i] * polar(1.0, -phase);
				outputSignals[0]->bufferPut(out);
				space--;

			}
		}
		while (ready>0 && space>0)
		{
			inputSignals[0]->bufferGet(&in);
			ready--;

			rotate(delayLine.begin(), delayLine.begin() + 1, delayLine.end());
			delayLine[delayLine.size() - 1] = in;

			
			if (count % pilotRate == 0) {
				rotate(samplesForEstimation.begin(), samplesForEstimation.begin() + 1, samplesForEstimation.end());
				samplesForEstimation[numberOfSamplesForEstimation - 1] = in;
				phaseBefore = phase;
				phase = (arg(samplesForEstimation[1]) + arg(samplesForEstimation[0])) / 2.0;

				if (phase - phaseBefore > PI) wrap += PI;
				else if (phase - phaseBefore < -PI)	wrap -= PI;
				phase += wrap;
				for (int i = 0; i < pilotRate; i++)
				{
					out = delayLine[i+1] * polar(1.0, -phase);
					outputSignals[0]->bufferPut(out);
					space--;
				}
			}
			count++;
		}
	}
	else if (mode == DelayLineMode)
	{
		for (int i = 0; i < process; i++)
		{
			inputSignals[0]->bufferGet(&in);
			if (count % pilotRate == 0) phase = arg(in);
			else
			{
				out = in * polar(1.0, -phase);
				outputSignals[0]->bufferPut(out);
			}
			count++;
		}
	}

	return true;
}
