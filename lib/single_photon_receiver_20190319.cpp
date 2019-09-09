#include "single_photon_receiver_20190319.h"

void SinglePhotonReceiver::initialize(){
	SinglePhotonDectection_In_1.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	SinglePhotonDectection_In_1.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	SinglePhotonDectection_In_1.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	
	SinglePhotonDectection_In_2.setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	SinglePhotonDectection_In_2.setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	SinglePhotonDectection_In_2.setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());

	setSuperBlockSystem(
	{ 
		&B1,
		&B2,
		&B3,
		&B4,
		&B5,
		&B6,
		&B7,
		&B8,
		&B9
//		&B10
	});

	outputSignals[0]->setSymbolPeriod(MeasurementResult_Out.getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(MeasurementResult_Out.getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(MeasurementResult_Out.getFirstValueToBeSaved());

	SPR05.setSaveSignal(true);
	SPR06.setSaveSignal(true);
	SPR07.setSaveSignal(true);
	SPR08.setSaveSignal(true);
	MeasurementResult_Out.setSaveSignal(true);

	setLogValue(logValue);
	setLogFileName(logFileName);
	setSignalsFolderName(signalsFolderName);
};


bool SinglePhotonReceiver::runBlock()
{

	if (!experimental::filesystem::is_directory(signalsFolderName) || !experimental::filesystem::exists(signalsFolderName)) {
		experimental::filesystem::create_directory(signalsFolderName);
	}

	int ready = inputSignals[0]->ready();
	int space = SinglePhotonDectection_In_1.space();
	int process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_real sp1In;
		inputSignals[0]->bufferGet(&sp1In);
		SinglePhotonDectection_In_1.bufferPut(sp1In);
	}

	ready = inputSignals[1]->ready();
	space = SinglePhotonDectection_In_2.space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_real sp2In;
		inputSignals[1]->bufferGet(&sp2In);
		SinglePhotonDectection_In_2.bufferPut(sp2In);
	}

	bool alive = SuperBlock::runBlock(signalsFolderName);

	if (firstTime) {
		setOpenFile(false);
		firstTime = false;
	}

	ready = MeasurementResult_Out.ready();
	space = outputSignals[0]->space();

	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_real measOut = MeasurementResult_Out.bufferGet();
		outputSignals[0]->bufferPut(measOut);
	}

	return alive;
	
}