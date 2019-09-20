# include "../include/counter_n_20190426.h"

void CounterN::initialize(void) {

	for (auto k : outputSignals) {
		k->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
		k->setSamplingPeriod(inputSignals[0]->getSamplingPeriod() / n);
		k->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	}
}

bool CounterN::runBlock(void) {

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
		t_binary inMode{ 0 };
		inputSignals[0]->bufferGet(&inMode);

		if ((inMode != state)) {
			reset = true;
			state = inMode;
		}

		if (reset) {
			m = 0;
			reset = false;
		}
		
		if (inMode == 0) {
			if (m == 0)
				outputSignals[0]->bufferPut((t_binary)0);
			else {
				outputSignals[0]->bufferPut((t_binary)1);
				if (m == (n-1))
					reset = true;
			}
			m++;
		}
		else
			outputSignals[0]->bufferPut((t_binary)0);
		
	}

	return true;
}