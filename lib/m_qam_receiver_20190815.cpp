# include "../include/m_qam_receiver_20190815.h"

void MQAMReceiver::initialize()
{
	OpticalSignal_In.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	OpticalSignal_In.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	OpticalSignal_In.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	LocalOscillator_In.setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	LocalOscillator_In.setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	LocalOscillator_In.setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());

	CoherentReceiverOpticalFrontEnd_.setSaveInternalSignals(getSaveInternalSignals());
	CoherentReceiverOpticalFrontEnd_.setLogValue(getLogValue());

	CoherentReceiverDSP_.setSaveInternalSignals(getSaveInternalSignals());
	CoherentReceiverDSP_.setLogValue(getLogValue());

	setSuperBlockSystem(
		{	
			&CoherentReceiverOpticalFrontEnd_,
			&CoherentReceiverDSP_
		}
	);

	outputSignals[0]->setSymbolPeriod(BinaryData_Out.getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(BinaryData_Out.getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(BinaryData_Out.getFirstValueToBeSaved());

	setSuperBlockSystemValues();

};

bool MQAMReceiver::runBlock()
{
	string signalsFolderName = getSignalsFolderName();

	if (!experimental::filesystem::is_directory(signalsFolderName) || !experimental::filesystem::exists(signalsFolderName)) {
		experimental::filesystem::create_directory(signalsFolderName);
	}

	int ready = inputSignals[0]->ready();
	int space = OpticalSignal_In.space();
	int process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_complex opticalIn;
		inputSignals[0]->bufferGet(&opticalIn);
		OpticalSignal_In.bufferPut(opticalIn);
	}

	ready = inputSignals[1]->ready();
	space = LocalOscillator_In.space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_complex opticalIn;
		inputSignals[1]->bufferGet(&opticalIn);
		LocalOscillator_In.bufferPut(opticalIn);
	}

	bool alive = SuperBlock::runBlock(signalsFolderName);

	if (firstTime) {
		setOpenFile(false);
		firstTime = false;
	}

	ready = BinaryData_Out.ready();
	space = outputSignals[0]->space();

	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary binaryDataOut = BinaryData_Out.bufferGet();
		outputSignals[0]->bufferPut(binaryDataOut);
	}

	return alive;
};
