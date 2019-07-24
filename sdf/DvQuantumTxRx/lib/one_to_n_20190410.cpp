# include "../include/one_to_n_20190410.h"

void OneToN::initialize(void) {

	for (auto k : outputSignals) {
		k->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
		k->setSamplingPeriod(inputSignals[0]->getSamplingPeriod()/n);
		k->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	}
}

bool OneToN::runBlock(void) {

	int ready{ MAX_BUFFER_LENGTH };
	for (auto k : inputSignals) {
		ready = min(k->ready(), ready);
	}

	int space = { MAX_BUFFER_LENGTH };
	for (auto k : outputSignals) {
		space = min(k->space(), space);
	}

	int process = min(ready, (int)round(space/n));

	if (process <= 0) return false;

	for (auto k = 0; k < process; k++) {
		t_binary in{ 0 };
		inputSignals[0]->bufferGet(&in);
		
		for(auto m = 0; m < n; m++)
			outputSignals[0]->bufferPut(in);
	}

	return true;
}