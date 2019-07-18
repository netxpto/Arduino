#include "../include/quantum_channel_emulator_20190328.h"

void QuantumChannelEmulator::initialize(void) {

	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplesPerSymbol());

}

bool QuantumChannelEmulator::runBlock(void) {
	

	int ready0 = inputSignals[0]->ready();	// alice data
	int ready1 = inputSignals[1]->ready();	// alice basis
	int ready2 = inputSignals[2]->ready();	// transmission mode
	int ready3 = inputSignals[3]->ready();	// bob basis

	int ready = min(ready0, ready1);
	ready = min(ready, ready2);
	ready = min(ready, ready3);

	int space = outputSignals[0]->space();

	int process = min(ready, space);

	if (0 == process) return false;

	for (auto i = 0; i < process; i++) {
		
		t_real randomRollClicks = uniformeRandomNumberBetweenZeroAndOne();
		t_real randomRollError = uniformeRandomNumberBetweenZeroAndOne();

		t_binary dataIn;
		inputSignals[0]->bufferGet(&dataIn);

		t_binary aliceBasis;
		inputSignals[1]->bufferGet(&aliceBasis);

		t_binary bobBasis;
		inputSignals[3]->bufferGet(&bobBasis);

		t_binary transmissionMode;
		inputSignals[2]->bufferGet(&transmissionMode);
		outputSignals[1]->bufferPut(transmissionMode);

		if (randomRollClicks <= probabilityOfNoClick) {
			outputSignals[0]->bufferPut(noClickNumber);
		}
		else if (randomRollClicks <= probabilityOfNoClick + probabilityOfBothClick) {
			outputSignals[0]->bufferPut(doubleClickNumber);
		} else {
			if (randomRollError <= probabilityOfError) {
				switch ((t_binary)dataIn) {
				case 1:
					outputSignals[0]->bufferPut((t_real)0);
					break;
				case 0:
					outputSignals[0]->bufferPut((t_real)1);
					break;
				}
			}
			else {
				t_real randomRollErrorFromWrongBasis = uniformeRandomNumberBetweenZeroAndOne();
				
				if ((aliceBasis == bobBasis) || (randomRollErrorFromWrongBasis<0.5)){
					switch ((t_binary)dataIn) {
					case 1:
						outputSignals[0]->bufferPut((t_real)1);
						break;
					case 0:
						outputSignals[0]->bufferPut((t_real)0);
						break;
					default:
						outputSignals[0]->bufferPut((t_real)-3);
						break;
					}
				} else {
					switch ((t_binary)dataIn) {
					case 1:
						outputSignals[0]->bufferPut((t_real)0);
						break;
					case 0:
						outputSignals[0]->bufferPut((t_real)1);
						break;
					default:
						outputSignals[0]->bufferPut((t_real)-3);
						break;
					}
				}
			}
		}
	}

	return true;
}
