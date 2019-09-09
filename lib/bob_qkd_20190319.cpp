#include "bob_qkd_20190319.h"

void BobQKD::initialize()
{

	BinaryData_In.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	BinaryData_In.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	BinaryData_In.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	Clock_In.setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	Clock_In.setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	Clock_In.setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());

	SinglePhotonReceiverMeasurement_In.setSymbolPeriod(inputSignals[2]->getSymbolPeriod());
	SinglePhotonReceiverMeasurement_In.setSamplingPeriod(inputSignals[2]->getSamplingPeriod());
	SinglePhotonReceiverMeasurement_In.setFirstValueToBeSaved(inputSignals[2]->getFirstValueToBeSaved());

	MeasurementBasis_In.setSymbolPeriod(inputSignals[3]->getSymbolPeriod());
	MeasurementBasis_In.setSamplingPeriod(inputSignals[3]->getSamplingPeriod());
	MeasurementBasis_In.setFirstValueToBeSaved(inputSignals[3]->getFirstValueToBeSaved());

	Messages_In.setSymbolPeriod(inputSignals[4]->getSymbolPeriod());
	Messages_In.setSamplingPeriod(inputSignals[4]->getSamplingPeriod());
	Messages_In.setFirstValueToBeSaved(inputSignals[4]->getFirstValueToBeSaved());

	BB6.setSymbolPeriod(0.001);

	setSuperBlockSystem(
		{ 
			&BB0,
			&BB1,
			&BB2,
			&BB3,
			&BB4,
			&BB10,
			&BB5,
			&BB7,
			&BB6,
			&BBSink0
//			&BB9
		}
	);
	
	outputSignals[0]->setSymbolPeriod(PolarizationRotatorAngle_Out.getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(PolarizationRotatorAngle_Out.getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(PolarizationRotatorAngle_Out.getFirstValueToBeSaved());

	outputSignals[1]->setSymbolPeriod(ClockBob_Photodetector_Out_1.getSymbolPeriod());
	outputSignals[1]->setSamplingPeriod(ClockBob_Photodetector_Out_1.getSamplingPeriod());
	outputSignals[1]->setFirstValueToBeSaved(ClockBob_Photodetector_Out_1.getFirstValueToBeSaved());

	outputSignals[2]->setSymbolPeriod(ClockBob_Photodetector_Out_2.getSymbolPeriod());
	outputSignals[2]->setSamplingPeriod(ClockBob_Photodetector_Out_2.getSamplingPeriod());
	outputSignals[2]->setFirstValueToBeSaved(ClockBob_Photodetector_Out_2.getFirstValueToBeSaved());

	outputSignals[3]->setSymbolPeriod(Messages_Out.getSymbolPeriod());
	outputSignals[3]->setSamplingPeriod(Messages_Out.getSamplingPeriod());
	outputSignals[3]->setFirstValueToBeSaved(Messages_Out.getFirstValueToBeSaved());

	outputSignals[4]->setSymbolPeriod(MutualInformation_Out.getSymbolPeriod());
	outputSignals[4]->setSamplingPeriod(MutualInformation_Out.getSamplingPeriod());
	outputSignals[4]->setFirstValueToBeSaved(MutualInformation_Out.getFirstValueToBeSaved());

	setLogValue(logValue);
	setLogFileName(logFileName);
	setSignalsFolderName(signalsFolderName);
	
};


bool BobQKD::runBlock()
{

	if (!experimental::filesystem::is_directory(signalsFolderName) || !experimental::filesystem::exists(signalsFolderName)) {
		experimental::filesystem::create_directory(signalsFolderName);
	}
	////////// INPUT SIGNALS /////////////
	int ready = inputSignals[0]->ready();
	int space = BinaryData_In.space();
	int process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary dataIn;
		inputSignals[0]->bufferGet(&dataIn);
		BinaryData_In.bufferPut(dataIn);
	}

	ready = inputSignals[1]->ready();
	space = Clock_In.space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_real clockIn;
		inputSignals[1]->bufferGet(&clockIn);
		Clock_In.bufferPut(clockIn);
	}


	ready = inputSignals[2]->ready();
	space = SinglePhotonReceiverMeasurement_In.space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_real measIn;
		inputSignals[2]->bufferGet(&measIn);
		SinglePhotonReceiverMeasurement_In.bufferPut(measIn);
	}

	ready = inputSignals[3]->ready();
	space = MeasurementBasis_In.space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary basisIn;
		inputSignals[3]->bufferGet(&basisIn);
		MeasurementBasis_In.bufferPut(basisIn);
	}

	//////////// MESSAGES? ///////////////
	ready = inputSignals[4]->ready();
	space = Messages_In.space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_message messagesIn;
		inputSignals[4]->bufferGet(&messagesIn);
		Messages_In.bufferPut(messagesIn);
	}
	/////////////////////////////////////

	bool alive = SuperBlock::runBlock(signalsFolderName);

	if (firstTime) {
		setOpenFile(false);
		firstTime = false;
	}


	////////// OUTPUT SIGNALS ////////////

	ready = PolarizationRotatorAngle_Out.ready();
	space = outputSignals[0]->space();

	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_real polarOut = PolarizationRotatorAngle_Out.bufferGet();
		outputSignals[0]->bufferPut(polarOut);
	}

	/////////////

	ready = ClockBob_Photodetector_Out_1.ready();
	space = outputSignals[1]->space();

	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_real clockOut = ClockBob_Photodetector_Out_1.bufferGet();
		outputSignals[1]->bufferPut(clockOut);
	}

	////////////

	ready = ClockBob_Photodetector_Out_2.ready();
	space = outputSignals[2]->space();

	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_real clockOut = ClockBob_Photodetector_Out_2.bufferGet();
		outputSignals[2]->bufferPut(clockOut);
	}

	////////////

	ready = MutualInformation_Out.ready();
	space = outputSignals[4]->space();

	process = min(ready, space);
	if (ready >= 512){
		for (auto k = 0; k != process; k++)
		{
			count++;
			t_binary miOut = MutualInformation_Out.bufferGet();
			outputSignals[4]->bufferPut(miOut);
		}
	}

	//////////// MESSAGES? //////////////

	ready = Messages_Out.ready();
	space = outputSignals[3]->space();

	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_message messagesOut = Messages_Out.bufferGet();
		outputSignals[3]->bufferPut(messagesOut);
	}
	/////////////////////////////////////
	return alive;
}