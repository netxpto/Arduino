#include "..\include\bob_incoming_data_processor_20190410.h"

void BobIncomingDataProcessor::initialize(void)
{
	outputSignals[0]->setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());

	outputSignals[1]->setSymbolPeriod(inputSignals[2]->getSymbolPeriod());
	outputSignals[1]->setSamplingPeriod(inputSignals[2]->getSamplingPeriod());
	outputSignals[1]->setFirstValueToBeSaved(inputSignals[2]->getFirstValueToBeSaved());

}

bool BobIncomingDataProcessor::runBlock(void)
{
	
	bool alive{ false };
	t_real dataIn;
	t_binary basisIn;
	t_binary transmissionStatus;

	int ready{ MAX_BUFFER_LENGTH };
	int space{ MAX_BUFFER_LENGTH };
//	int transmissionReady{ inputSignals[0]->ready() };

	for (auto k = 1; k < (int)inputSignals.size(); ++k) {
		ready = (ready < inputSignals[k]->ready()) ? ready : inputSignals[k]->ready();
	}

	//	for (auto k : inputSignals) { ready = (ready < k->ready()) ? ready : k->ready(); }
	for (auto k : outputSignals) { space = (space < k->space()) ? space : k->space(); }

	int process = min(ready, space);
	if (0 == process) return alive;

	/*
	for (unsigned int i = 0; i < process; i++) {
		inputSignals[0]->bufferGet(&transmissionStatus);
		inputSignals[1]->bufferGet(&dataIn);
		inputSignals[2]->bufferGet(&basisIn);

		if (1 == transmissionStatus) {
			outputSignals[0]->bufferPut(dataIn);
			outputSignals[2]->bufferPut(dataIn);
			outputSignals[1]->bufferPut(basisIn);
			outputSignals[3]->bufferPut(basisIn);
		}
	}*/

	for (auto k = 0; k < process; ++k) {
		inputSignals[0]->bufferGet(&transmissionStatus);
		inputSignals[1]->bufferGet(&dataIn);
		inputSignals[2]->bufferGet(&basisIn);

		if (dataIn != controlBitNumber) {
			outputSignals[0]->bufferPut(dataIn);
			outputSignals[1]->bufferPut(basisIn);
		}
		numberOfSamples++;
	}

	if (numberOfSamples > oldNumberOfSamples+showNumberOfSamplesInterval) {
		oldNumberOfSamples = numberOfSamples;
		cout << "Received qubits:  " << numberOfSamples << "\n";
	}

	alive = true;

	return alive;
	
}
