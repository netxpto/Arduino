# include "../include/andGate_20190410.h"

void AndGate::initialize(void) {

	for (auto k : outputSignals) {
		k->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
		k->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
		k->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	}
}

bool AndGate::runBlock(void) {
	int ready{ MAX_BUFFER_LENGTH };
	for (auto k : inputSignals) {
		ready = min(k->ready(), ready);
	}

	int space = { MAX_BUFFER_LENGTH };
	for (auto k : outputSignals) {
		space = min(k->space(), space);
	}

	int process = min(ready, space);

	if (process <= 0) return false;

	for (auto k = 0; k < process; k++) {
		t_binary in0{ 0 }, in1{ 0 };
		inputSignals[0]->bufferGet(&in0);
		inputSignals[1]->bufferGet(&in1);

		if ((in0 == 1) && (in1 == 1))
			outputSignals[0]->bufferPut((t_binary)1);
		else
			outputSignals[0]->bufferPut((t_binary)0);
	}

	return true;
}