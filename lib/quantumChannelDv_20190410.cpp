# include "../include/quantumChannelDv_20190410.h"

QuantumChannel::QuantumChannel(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals) : SuperBlock(inputSignals, outputSignals) {

	//###################################### Default Input Parameters Values #########################################################
	double attenuationDbm_perKm{ 0.0 };
	double fiberLength{ 1.0 };
	SOPType channelType{ SOPType::Stocastic };
	double polLineWidth{ 900e-9 };

	//############################################# Blocks ##########################################################################
	OpticalAttenuator_.setAttenuationCoef(attenuationDbm_perKm);
	OpticalAttenuator_.setFiberLength(fiberLength);

	SopModulator_.setSOPType(channelType);
	SopModulator_.setDeltaP(polLineWidth);
}

void QuantumChannel::initialize() {

	inSinglePhoton.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	inSinglePhoton.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	inSinglePhoton.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	
	//########################################################### SYSTEM ###################################################################

	setSuperBlockSystem(
		{	&OpticalAttenuator_,
			&SopModulator_
			 }
	);

	setLogValue(logValue);
	setLogFileName(logFileName);
	setSignalsFolderName(signalsFolderName);
}

bool QuantumChannel::runBlock() {
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