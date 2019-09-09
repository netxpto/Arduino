# include "../include/mux2To1_20190404.h"

void Mux2To1::initialize(void) {

	for (auto k : outputSignals) {
		k->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
		k->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
		k->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	}
}

bool Mux2To1::runBlock(void) {

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

	t_binary  in0, in1;
	for (auto k = 0; k < process; k++) {
		if (inputSignals.size() == 4) {
			t_binary enableSel{ 0 };
			inputSignals[3]->bufferGet(&enableSel);
			if (enableSel == 1)
				commutate = true;
			else
				commutate = false;
		}
		else
			commutate = true;

		if (firstTime) {
			commutate = true;
			firstTime = false;
		}
		if(commutate)
			inputSignals[2]->bufferGet(&sel);

		if (sel == 0) {
			inputSignals[0]->bufferGet(&in0);
			outputSignals[0]->bufferPut((t_binary)in0);
		}
		else {
			inputSignals[1]->bufferGet(&in1);
			outputSignals[0]->bufferPut((t_binary)in1);
		}
	}

	return true;
}