# include "../include/quantum_channel_dv_20190410.h"

void QuantumChannel::initialize() {

	

	//####################################################################################################################
	inSinglePhoton.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	inSinglePhoton.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	inSinglePhoton.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	
	//########################################################### SYSTEM ###################################################################

	setSuperBlockSystem(
		{	&OpticalAttenuator_,
			&SopRotator_
			 }
	);

	outputSignals[0]->setSamplingPeriod(driftedSPhoton.getSamplingPeriod());
	outputSignals[0]->setSymbolPeriod(driftedSPhoton.getSymbolPeriod());
	outputSignals[0]->setSamplesPerSymbol(driftedSPhoton.getSamplesPerSymbol());

	setSuperBlockSystemValues();
}

bool QuantumChannel::runBlock() {

	string signalsFolderName = getSignalsFolderName();

	if (!experimental::filesystem::is_directory(signalsFolderName) || !experimental::filesystem::exists(signalsFolderName)) {
		experimental::filesystem::create_directory(signalsFolderName);
	}

	/*Input*/
	int readyIn{ MAX_BUFFER_LENGTH };
	for (auto k : inputSignals) {
		readyIn = min(k->ready(), readyIn);
	}
	int spaceIn = inSinglePhoton.space();

	int processIn = min(readyIn, spaceIn);

	for (auto k = 0; k < processIn; k++) {
		t_complex_xy SinglePhotonIn;
		inputSignals[0]->bufferGet(&SinglePhotonIn);
		inSinglePhoton.bufferPut((t_complex_xy)SinglePhotonIn);
	}

	bool alive = SuperBlock::runBlock(signalsFolderName);

	if (firstTime) {
		setOpenFile(false);
		firstTime = false;
	};

	/*Output*/
	int readyOut = driftedSPhoton.ready();
	int spaceOut = outputSignals[0]->space();

	int process = min(readyOut, spaceOut);

	if (process == 0) alive = false;

	for (auto k = 0; k < process; k++) {
		t_complex_xy out = driftedSPhoton.bufferGet();
		outputSignals[0]->bufferPut((t_complex_xy)out);
	}

	return alive;
}