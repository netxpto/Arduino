# include "../include/BobQRx_20190410.h"

BobQRx::BobQRx(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals) : SuperBlock(inputSignals, outputSignals) {

	//###################################### Default Input Parameters Values #########################################################
	
	double bitRate{ 1e3 };
	int numberOfBits{ 1000 };
	vector<vector<t_real>> iqAmplitudesValues{ { 0, 1 },{ PI / 4, 1 } };
	// the in-phase componente is used to select the basis, 0 => Linear,  PI/4 => Diagonal
	// the quadrature componente can be used as a clock
	pulse_shapper_filter_type filterType{ pulse_shapper_filter_type::Square };
	int samplesPerSymbol{ 16 };
	double SPDProbabilityDarkCount{ 0.0 };
	double clockDetectorPhase{ 0.0 };
	double decisionDetectorLevel{ 0.5 };
	double pulseWidthDetection{ 0.2 };
	double qberThreshold{ 0.02 };
	double initialQber{ 0.25 };
	string controlSequence{ "01" };
	double timeDelayCoincDetector{ 0.0 };

	
	//############################################# Blocks ##########################################################################
	MQamMapper_.setIqAmplitudes(iqAmplitudesValues);
	MQamMapper_.setFirstTime(true);

	DiscreteToContinuousTime_1.setNumberOfSamplesPerSymbol(samplesPerSymbol);

	PulseShaper_clk.setFilterType(filterType);
	PulseShaper_clk.setPulseWidth(1 / bitRate / 1.5);

	DiscreteToContinuousTime_.setNumberOfSamplesPerSymbol(samplesPerSymbol);

	PulseShaper_.setFilterType(filterType);
	PulseShaper_.setPulseWidth(1 / bitRate);
	PulseShaper_.setSaveImpulseResponse(false);

	SinglePhotonDetector_0.setPath(0);
	SinglePhotonDetector_0.setProbabilityDarkCount(SPDProbabilityDarkCount);
	SinglePhotonDetector_1.setPath(1);
	SinglePhotonDetector_1.setProbabilityDarkCount(SPDProbabilityDarkCount);

	Clock_.setClockPeriod(3*(1 / bitRate ));
	Clock_.setSamplingPeriod((3 * (1 / bitRate)) / samplesPerSymbol);

	TimeDelay_clk.setTimeDelay(timeDelayCoincDetector);

	PulseShaper_cd.setFilterType(filterType);
	PulseShaper_cd.setPulseWidth(((1 / bitRate)*3) / 2);
	PulseShaper_cd.setSaveImpulseResponse(true);

	CoincidenceDetector_.setDecisionLevel(decisionDetectorLevel);

	RandomSopCompensation_.setDataSequence(controlSequence);
	RandomSopCompensation_.setQberTh(qberThreshold);
	//	RandomSopCompensation_.setQberInitial(initialQber);

	DiscreteToContinuousTheta1.setNumberOfSamplesPerSymbol(samplesPerSymbol);
	DiscreteToContinuousTheta2.setNumberOfSamplesPerSymbol(samplesPerSymbol);
	DiscreteToContinuousTheta3.setNumberOfSamplesPerSymbol(samplesPerSymbol);

};

void BobQRx::initialize()
{
	Bob_in.setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	Bob_in.setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	Bob_in.setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());

	BasisQRx.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	BasisQRx.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	BasisQRx.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	//########################################################### SYSTEM ###################################################################

	setSuperBlockSystem({
		&MQamMapper_,
		&DiscreteToContinuousTime_1,
		&TimeDelay_,
		&PulseShaper_clk,
		&Fork_,
		&DiscreteToContinuousTime_,
		&PulseShaper_,
		&ElectronicPC_ ,
		&PolarizationRotator_ ,
		&PolarizationBeamSplitter_ ,
		&SinglePhotonDetector_0 ,
		&SinglePhotonDetector_1 ,
		&Clock_ ,
		&TimeDelay_clk ,
		&PulseShaper_cd ,
		&CoincidenceDetector_ ,
		&RandomSopCompensation_ ,
		&DiscreteToContinuousTheta1 ,
		&DiscreteToContinuousTheta2 ,
		&DiscreteToContinuousTheta3 });

	outputSignals[0]->setSamplingPeriod(CD2_out.getSamplingPeriod());
	outputSignals[0]->setSymbolPeriod(CD2_out.getSymbolPeriod());
	outputSignals[0]->setSamplesPerSymbol(CD2_out.getSamplesPerSymbol());

	outputSignals[1]->setSamplingPeriod(ModeSelection.getSamplingPeriod());
	outputSignals[1]->setSymbolPeriod(ModeSelection.getSymbolPeriod());
	outputSignals[1]->setSamplesPerSymbol(ModeSelection.getSamplesPerSymbol());

	setLogValue(logValue);
	setLogFileName(logFileName);
	setSignalsFolderName(signalsFolderName);
}

bool BobQRx::runBlock()
{
	bool alive = SuperBlock::runBlock(signalsFolderName);

	if (firstTime) {
		setOpenFile(false);
		firstTime = false;
	}

	auto ready_4 = inputSignals[0]->ready();

	auto space_4{ MAX_BUFFER_LENGTH };

	space_4 = BasisQRx.getBufferLength();

	auto process4 = min(ready_4, space_4);

	for (auto k = 0; k < process4; k++) {
	
		t_binary basis;
		inputSignals[0]->bufferGet(&basis);
		BasisQRx.bufferPut((t_binary)basis);
	}

	/*Input 2*/
	auto ready_1 = inputSignals[1]->ready();

	auto space_1{ MAX_BUFFER_LENGTH };

	space_1 = Bob_in.getBufferLength();
	auto space_2 = Bob_in.space();
	space_1 = min(space_1, space_2);
	auto process = min(ready_1, space_1);

	for (auto k = 0; k < process; k++) {
		t_complex_xy photonIn;
		inputSignals[1]->bufferGet(&photonIn);
		Bob_in.bufferPut((t_complex_xy)photonIn);
	}

	/*Output 1*/
	auto ready_out1 = CD2_out.ready();
	auto space_out1 = outputSignals[0]->space();

	auto process1_out = min(ready_out1, space_out1);

	for (auto m = 0; m < process1_out; m++) {
		t_real outData = CD2_out.bufferGet();
		outputSignals[0]->bufferPut((t_real)outData);
	}

	/*Output 2*/
	auto ready_out2 = ModeSelection.ready();
	auto space_out2 = outputSignals[1]->space();

	auto process2_out = min(ready_out2, space_out2);

	for (auto m = 0; m < process2_out; m++) {
		t_binary modeData = ModeSelection.bufferGet();
		outputSignals[1]->bufferPut((t_binary)modeData);
	}
	
	
	return alive;
}