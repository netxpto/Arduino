# include "../include/interspace0s_20190319.h"

void Interspace0s::initialize(void) {

	outputSignals[0]->setSymbolPeriod (inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod (inputSignals[0]->getSamplingPeriod() / upSamplingFactor);
	outputSignals[0]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

}

bool Interspace0s::runBlock(void) {

	bool alive{ false };

	int ready = inputSignals[0]->ready();
	int space = outputSignals[0]->space();
	int process = min(ready, space);

	if (process == 0) return false;

	signal_value_type inSignalType = inputSignals[0]->getValueType();

	if (index != 0) {
		int length = min(upSamplingFactor - index, space);
		if (length > 0) alive = true;

		switch (inSignalType) {
		case signal_value_type::t_complex:
			for (int i = 0; i < length; i++) {
				outputSignals[0]->bufferPut((t_complex) 0.0);
				index++;
			}
			index = index % upSamplingFactor;
			break;
		default: cout << "ERRO: interspace0s.cpp - invalide signal type" << "\n";
		}

	};

	if (index != 0) return alive;

	int length = min(space, ready*upSamplingFactor);
	if (length <= 0) return alive;

	switch (inSignalType) {
		case signal_value_type::t_complex:
		t_complex value;
		for (int i = 0; i < length; i++) {
			if (index != 0) {
				outputSignals[0]->bufferPut((t_complex) 0.0);
			}
			else {
				(inputSignals[0])->bufferGet(&value);
				outputSignals[0]->bufferPut(value);
			}
			space--;
			index++;
			index = index % upSamplingFactor;
		}
		break;
	};

	return alive;
};
