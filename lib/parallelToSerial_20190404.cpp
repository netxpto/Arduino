# include "../include/parallelToSerial_20190404.h"

// Este conversor só funciona com 3 entradas e 1 saída

void ParallelToSerial::initialize(void) {

	for (auto k : outputSignals) {
		k->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
		k->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
		k->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	}
}

bool ParallelToSerial::runBlock(void) {

	int ready{ MAX_BUFFER_LENGTH };
	for (auto k : inputSignals) {
		ready = min(k->ready(), ready);
	}

	int space = { MAX_BUFFER_LENGTH };
	for (auto k : outputSignals) {
		space = min(k->space(), space);
	}

	int process = min(3*ready, space);

	if (process <= 0) return false;

	for (auto k = 0; k < process; k++) {
		t_binary  in0{ 0 }, in1{ 0 }, in2{ 0 };

		switch (inputOut)
		{
		case 0:
			inputSignals[0]->bufferGet(&in0);
			outputSignals[0]->bufferPut((t_binary)in0);
			//outputSignals[1]->bufferPut((t_binary)1);
			inputOut++;
			break;
		case 1:
			inputSignals[1]->bufferGet(&in1);
			outputSignals[0]->bufferPut((t_binary)in1);
			//outputSignals[1]->bufferPut((t_binary)0);
			inputOut++;
			break;
		case 2:
			inputSignals[2]->bufferGet(&in2);
			outputSignals[0]->bufferPut((t_binary)in2);
			//outputSignals[1]->bufferPut((t_binary)0);
			inputOut = 0;
			break;
		default:
			break;
		}
	}

	return true;
}