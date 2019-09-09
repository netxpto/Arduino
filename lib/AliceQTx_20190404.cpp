# include "../include/AliceQTx_20190404.h"

AliceQTx::AliceQTx(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals) : SuperBlock(inputSignals, outputSignals) {
	
	//###################################### Default Input Parameters Values #########################################################
//	vector<vector<t_real>> iqAmplitudesValues{ { 0, 0, 0, 1, 0.1 },{ 0.5, 0, 0, 0, 0.1},{ 0.25, 0, 1, 1, 0.1},{ 0.75, 0, 1, 0, 0.1},{ 0, 1, 0, 1, 5 },{ 0.5, 0, 0, 0, 5},{ 0.25, 0, 1, 1, 5},{ 0.75, 0, 1, 0, 5} };
	vector<vector<t_real>> iqAmplitudesValues{ { 0, 0.1 },{ 0.5, 0.1},{ 0.25,  0.1},{ 0.75,  0.1},{ 0, 5 },{ 0.5, 5},{ 0.25, 5},{ 0.75, 5} };
	long int numberOfSymbols{ 100 };
	t_real bitRate{ 1e3 };
	int samplesPerSymbol{ 16 };
	double LaserPower_dBm{ 45 };
	double Attenuation_dB{ 15.0 };

	//############################################# Blocks ##########################################################################
	BSSelMonitoringMode_.setMode(BinarySourceMode::DeterministicCyclic);
	BSSelMonitoringMode_.setBitStream("0");
	BSSelMonitoringMode_.setBitPeriod(1 / bitRate);

	BSSelActuationMode_.setMode(BinarySourceMode::DeterministicCyclic);
	BSSelActuationMode_.setBitStream("1");
	BSSelActuationMode_.setBitPeriod(1 / bitRate);

	BSData_.setMode(BinarySourceMode::DeterministicCyclic);
	BSData_.setBitStream("0");
	BSData_.setBitPeriod(1 / bitRate);

	BSControl_.setMode(BinarySourceMode::DeterministicCyclic);
	BSControl_.setBitStream("1");
	BSControl_.setBitPeriod(1 / bitRate);

	BSControlBasis_.setMode(BinarySourceMode::DeterministicCyclic);
	BSControlBasis_.setBitStream("0101");// Base 45/-45 = '0'; Base circular ='1'
	BSControlBasis_.setBitPeriod(1 / bitRate);

	BSControlBits_.setMode(BinarySourceMode::DeterministicCyclic);
	BSControlBits_.setBitStream("1100"); // 45/CircDir = '1', -45/CircEsq = '0'
	BSControlBits_.setBitPeriod(1 / bitRate);

	MQamMapper_.setIqAmplitudes(iqAmplitudesValues);
	MQamMapper_.setSystemCausality(false);

	DTCTI_.setNumberOfSamplesPerSymbol(samplesPerSymbol);

	PulseShaperI_.setFilterType(pulse_shapper_filter_type::Square);
	PulseShaperI_.setPulseWidth((1 / bitRate)*3/2);

	TimeDelay_.setTimeDelay((1 / bitRate)*3/1.5);

	DTCTQ_.setNumberOfSamplesPerSymbol(samplesPerSymbol);

	PulseShaperQ_.setFilterType(pulse_shapper_filter_type::Square);
	PulseShaperQ_.setPulseWidth((1 / bitRate)/4);

	Laser_.setOpticalPower_dBm(LaserPower_dBm);
	Laser_.setSymbolPeriod(1 / (3*bitRate) );
	Laser_.setSamplingPeriod(1 / (3 * bitRate) / samplesPerSymbol);

	OpticalAttenuator_.setAttenuationCoef(Attenuation_dB);

}

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
			&BSSelMonitoringMode_,
			&BSSelActuationMode_,
			&MuxSelMode_,
			&BSData_,
			&ParalleToSerialData_,
			&BSControl_,
			&BSControlBasis_,
			&BSControlBits_,
			&ParallelToSerialControl_,
			&AndGate_,
			&MuxOutputBitStream_,
			&MQamMapper_,
		/*	&SinkMQM1_,
			&SinkMQM2_,
			&SinkMQM3_,*/
			&DTCTI_,
			&DTCTQ_,
			&PulseShaperI_,
			&PulseShaperQ_,
			&TimeDelay_,
			&Laser_,
			&AmplitudeModulator_,
			&PolarizationModulator_,
			&OpticalAttenuator_}
	);

	outputSignals[0]->setSamplingPeriod(AliceQTx_out.getSamplingPeriod());
	outputSignals[0]->setSymbolPeriod(AliceQTx_out.getSymbolPeriod());
	outputSignals[0]->setSamplesPerSymbol(AliceQTx_out.getSamplesPerSymbol());

	setLogValue(logValue);
	setLogFileName(logFileName);
	setSignalsFolderName(signalsFolderName);
}

bool AliceQTx::runBlock() {

	int ready{ MAX_BUFFER_LENGTH };
	for (auto k : inputSignals) {
		ready = min(k->ready(), ready);
	}
	int space = min(BasisData.space(), BitsData.space());

	int processIn = min(ready, space);

	for (auto k = 0; k < processIn; k++) {
		t_binary BasisIn, BitIn, enableSel;
		inputSignals[0]->bufferGet(&BasisIn);
		inputSignals[1]->bufferGet(&BitIn);
		inputSignals[2]->bufferGet(&enableSel);

		BasisData.bufferPut((t_binary)BasisIn);
		BitsData.bufferPut((t_binary)BitIn);
		ModeSelection.bufferPut((t_binary)enableSel);
	}

	bool alive = SuperBlock::runBlock(signalsFolderName);

	if (firstTime) {
		setOpenFile(false);
		firstTime = false;
	};

	/*Output*/
	int readyOut = AliceQTx_out.ready();
	int spaceOut = outputSignals[0]->space();

	int process = min(readyOut, spaceOut);

	for (auto k = 0; k < process; k++) {
		t_complex_xy out = AliceQTx_out.bufferGet();
		outputSignals[0]->bufferPut((t_complex_xy)out);
	}

	return alive;
}