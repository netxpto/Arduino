
# include "../include/amplitude_modulator_20190408.h"


void AmplitudeModulator::initialize(void){

	firstTime = false;

	for (auto k : outputSignals) {
		k->setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
		k->setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
		k->setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());
		k->setCentralWavelength(inputSignals[1]->getCentralWavelength());
		k->setCentralFrequency(inputSignals[1]->getCentralWavelength());
	}
	
}


bool AmplitudeModulator::runBlock(void){
	
	int ready{ MAX_BUFFER_LENGTH };
	for (auto k : inputSignals) {
		ready = min(k->ready(), ready);
	}

	int space = outputSignals[0]->space();
	
	int process = min(ready, space);

	if (process == 0) return false;
	
	for (int i = 0; i < process; i++) {

		t_complex in;
		t_real inRFSignal;
		t_complex out;

		inputSignals[0]->bufferGet(&in);
		inputSignals[1]->bufferGet(&inRFSignal);

		out = in*inRFSignal;

		outputSignals[0]->bufferPut(out);

	}
	return true;
}
