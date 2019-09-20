#include "../include/click_error_emulator_20190329.h"

#include "../include/quantum_channel_emulator_20190328.h"

void ClickErrorEmulator::initialize(void) {

	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplesPerSymbol());

}

bool ClickErrorEmulator::runBlock(void) {


	int ready = inputSignals[0]->ready();

	int space = outputSignals[0]->space();
	
	int process = min(ready, space);

	if (0 == process) return false;

	for (int i = 0; i < process; i++) {
		
		t_binary basisIn;
		t_real randomRoll = uniformeRandomNumberBetweenZeroAndOne();

		inputSignals[0]->bufferGet(&basisIn);
	
		if (randomRoll <= probabilityOfNoClicks) {
			outputSignals[0]->bufferPut((t_real)-1);
			break;
		}
		else if (randomRoll <= probabilityOfNoClicks + probabilityOfTwoClicks) {
			outputSignals[0]->bufferPut((t_real)-2);
			break;
		} else {
			switch (basisIn) {
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
		}
	}
	return true;
}
