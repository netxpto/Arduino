# include "../include/BobQRx_20190410.h"


void BobQRx::initialize()
{

	


	Bob_in.setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	Bob_in.setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	Bob_in.setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());

	BasisQRxData.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	BasisQRxData.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	BasisQRxData.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	//########################################################### SYSTEM ###################################################################

	setSuperBlockSystem({
		&BSZero_,
		&BSOne_,
		&CounterN_,
		&MuxMode_,
		&BSBobBasisControl_,
		&MuxBasis_,
		&MQamMapper_,
		&DiscreteToContinuousTime_1,
		&PulseShaper_clk,
		&TimeDelay_,
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

	outputSignals[0]->setSamplingPeriod(Cd_Data_Out.getSamplingPeriod());
	outputSignals[0]->setSymbolPeriod(Cd_Data_Out.getSymbolPeriod());
	outputSignals[0]->setSamplesPerSymbol(Cd_Data_Out.getSamplesPerSymbol());

	outputSignals[1]->setSamplingPeriod(ModeSelection.getSamplingPeriod());
	outputSignals[1]->setSymbolPeriod(ModeSelection.getSymbolPeriod());
	outputSignals[1]->setSamplesPerSymbol(ModeSelection.getSamplesPerSymbol());

	outputSignals[2]->setSamplingPeriod(QBER_QChannel.getSamplingPeriod());
	outputSignals[2]->setSymbolPeriod(QBER_QChannel.getSymbolPeriod());
	outputSignals[2]->setSamplesPerSymbol(QBER_QChannel.getSamplesPerSymbol());

	setSuperBlockSystemValues();
}

bool BobQRx::runBlock()
{
	
	string signalsFolderName = getSignalsFolderName();

	if (!experimental::filesystem::is_directory(signalsFolderName) || !experimental::filesystem::exists(signalsFolderName)) {
		experimental::filesystem::create_directory(signalsFolderName);
	}

	auto ready_4 = inputSignals[0]->ready();

	auto space_4{ MAX_BUFFER_LENGTH };

	space_4 = BasisQRxData.getBufferLength();

	auto process4 = min(ready_4, space_4);

	for (auto k = 0; k < process4; k++) {
	
		t_binary basis;
		inputSignals[0]->bufferGet(&basis);
		BasisQRxData.bufferPut((t_binary)basis);
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

	bool alive = SuperBlock::runBlock(signalsFolderName);

	if (firstTime) {
		setOpenFile(false);
		firstTime = false;
	}

	/*Output 1*/
	auto ready_out1 = Cd_Data_Out.ready();
	auto space_out1 = outputSignals[0]->space();

	auto process1_out = min(ready_out1, space_out1);

	for (auto m = 0; m < process1_out; m++) {
		t_real outData = Cd_Data_Out.bufferGet();
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
	
	/*Output 3*/
	auto ready_out3 = QBER_QChannel.ready();
	auto space_out3 = outputSignals[2]->space();

	auto process3_out = min(ready_out2, space_out2);

	for (auto m = 0; m < process3_out; m++) {
		t_real qberV = QBER_QChannel.bufferGet();
		outputSignals[2]->bufferPut((t_real)qberV);
	}
	
	return alive;
}