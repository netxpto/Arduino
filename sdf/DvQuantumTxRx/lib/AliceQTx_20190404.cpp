#include "../include/AliceQTx_20190404.h"


void AliceQTx::initialize() {

	BitsData.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	BitsData.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	BitsData.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	BasisData.setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	BasisData.setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	BasisData.setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());

	ModeSelection.setSymbolPeriod(inputSignals[2]->getSymbolPeriod());
	ModeSelection.setSamplingPeriod(inputSignals[2]->getSamplingPeriod());
	ModeSelection.setFirstValueToBeSaved(inputSignals[2]->getFirstValueToBeSaved());
	//########################################################### SYSTEM ###################################################################

	setSuperBlockSystem(
		{
			&CounterN_,
			&BSSelMonitoringMode_,
			&BSSelActuationMode_,
			&MuxSelMode_,
			&OneToN_,
			&BSData_,
			&ParalleToSerialData_,
			&BSControl_,
			&BSControlBasis_,
			&BSControlBits_,
			&ParallelToSerialControl_,
			&MuxOutputBitStream_,
			&MQamMapper_,
			&DTCTI_,
			&DTCTQ_,
			&PulseShaperI_,
			&PulseShaperQ_,
			&TimeDelay_,
			&Laser_,
			&AmplitudeModulator_,
			&PolarizationModulator_}
	);

	outputSignals[0]->setSamplingPeriod(Pol_ModOut.getSamplingPeriod());
	outputSignals[0]->setSymbolPeriod(Pol_ModOut.getSymbolPeriod());
	outputSignals[0]->setSamplesPerSymbol(Pol_ModOut.getSamplesPerSymbol());

	setSuperBlockSystemValues();

}

bool AliceQTx::runBlock() {

	string signalsFolderName = getSignalsFolderName();

	if (!experimental::filesystem::is_directory(signalsFolderName) || !experimental::filesystem::exists(signalsFolderName)) {
		experimental::filesystem::create_directory(signalsFolderName);
	}

	int ready{ MAX_BUFFER_LENGTH };
	for (auto k : inputSignals) {
		ready = min(k->ready(), ready);
	}
	int space = min(BasisData.space(), BitsData.space());

	int processIn = min(ready, space);

	for (auto k = 0; k < processIn; k++) {
		t_binary BasisIn, BitIn;
		inputSignals[0]->bufferGet(&BasisIn);
		inputSignals[1]->bufferGet(&BitIn);	

		BasisData.bufferPut((t_binary)BasisIn);
		BitsData.bufferPut((t_binary)BitIn);	
	}

	/* Input 2*/
	int readyInMode = inputSignals[2]->ready();
	int spaceInMode = ModeSelection.space();

	int processMode = min(readyInMode, spaceInMode);

	if (processMode > 0) {
		t_binary enableSel;
		inputSignals[2]->bufferGet(&enableSel);
		ModeSelection.bufferPut((t_binary)enableSel);
	}

	bool alive = SuperBlock::runBlock(signalsFolderName);

	if (firstTime) {
		setOpenFile(false);
		firstTime = false;
	};

	/*Output*/
	int readyOut = Pol_ModOut.ready();
	int spaceOut = outputSignals[0]->space();

	int process = min(readyOut, spaceOut);

	for (auto k = 0; k < process; k++) {
		t_complex_xy out = Pol_ModOut.bufferGet();
		outputSignals[0]->bufferPut((t_complex_xy)out);
	}

	return alive;
}