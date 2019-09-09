# include "../include/polarization_modulator_20190408.h"

void PolarizationModulator::initialize(void) {
	for (auto k : outputSignals)
	{
		k->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
		k->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
		k->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	}
}

bool PolarizationModulator::runBlock(void) {
	int ready{ MAX_BUFFER_LENGTH };
	for (auto k : inputSignals) {
		ready = min(k->ready(), ready);
	}
	int space = outputSignals[0]->space();

	int process = min(ready, space);

	if (process <= 0) return false;

	for (auto k = 0; k < process; k++) {
		t_real rfVoltage;
		inputSignals[1]->bufferGet(&rfVoltage);
		t_complex inOpticalSignal;
		inputSignals[0]->bufferGet(&inOpticalSignal);

		int index{ 0 };
		for (auto j = 0; j < (int)inVoltages.size(); j++) {
			if (rfVoltage == inVoltages[j]) index = j;
		}

		t_real Theta = valuesToRotate[index][0];
		
		t_complex valueXout = cos(Theta)*inOpticalSignal;
		t_complex valueYout = sin(Theta)*inOpticalSignal;
		

		t_complex_xy valueXYout = { valueXout, valueYout };

		outputSignals[0]->bufferPut((t_complex_xy)valueXYout);

	}

	return true;
}