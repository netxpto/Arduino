#include "../include/quantum_channel_emulator_20190328.h"

void QuantumChannelEmulator::initialize(void) {

	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplesPerSymbol());

	outputSignals[1]->setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	outputSignals[1]->setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	outputSignals[1]->setSamplingPeriod(inputSignals[1]->getSamplesPerSymbol());

}

bool QuantumChannelEmulator::runBlock(void) {
	

	int ready0 = inputSignals[0]->ready();
	int ready1 = inputSignals[1]->ready();
	int ready2 = inputSignals[2]->ready();
	
	int space0 = outputSignals[0]->space();
	int space1 = outputSignals[1]->space();

	int ready = min(min(ready0, ready1), ready2);
	int space = min(space0, space1);

	int process = min(ready, space);

	if (0 == process) return false;

	for (int i = 0; i < process; i++) {
		t_real transmissionIn;
		t_binary dataIn;
		t_binary basisIn;
		t_real randomRollClicks = uniformeRandomNumberBetweenZeroAndOne();
		t_real randomRolLError = uniformeRandomNumberBetweenZeroAndOne();

		inputSignals[0]->bufferGet(&transmissionIn);
		inputSignals[1]->bufferGet(&dataIn);
		inputSignals[2]->bufferGet(&basisIn);

		outputSignals[0]->bufferPut(transmissionIn);

		if (randomRollClicks <= probabilityOfNoClick) {
			outputSignals[1]->bufferPut((t_real)-1);
		}
		else if (randomRollClicks <= probabilityOfNoClick + probabilityOfBothClick) {
			outputSignals[1]->bufferPut((t_real)-2);
		} else {
			if (randomRolLError <= probabilityOfError) {
				switch (dataIn) {
				case 1:
					outputSignals[1]->bufferPut((t_real)0);
					break;
				case 0:
					outputSignals[1]->bufferPut((t_real)1);
					break;
				default:
					outputSignals[1]->bufferPut((t_real)-3);
					break;
				}
			} else {
				switch (dataIn) {
				case 1:
					outputSignals[1]->bufferPut((t_real)1);
					break;
				case 0:
					outputSignals[1]->bufferPut((t_real)0);
					break;
				default:
					outputSignals[1]->bufferPut((t_real)-3);
					break;
				}
			}
		}

//		outputSignals[1]->bufferPut(dataIn);

	/*	if (randomRoll <= probabilityOfError) {
			switch (dataIn) {
			case 1:
				outputSignals[1]->bufferPut((t_real)0);
				break;
			case 0:
				outputSignals[1]->bufferPut((t_real)1);
				break;
			default:
				outputSignals[1]->bufferPut((t_real)-3);
				break;
			}
		} else {*/
		/*
			switch (dataIn) {
			case 1:
				outputSignals[1]->bufferPut((t_real)1);
				break;
			case 0:
				outputSignals[1]->bufferPut((t_real)0);
				break;
			default:
				outputSignals[1]->bufferPut((t_real)-3);
				break;
			}*/
//		}
	}
	return true;
}
