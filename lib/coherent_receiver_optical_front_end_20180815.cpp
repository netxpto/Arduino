# include "../include/coherent_receiver_optical_front_end_20180815.h"


void CoherentReceiverOpticalFrontEnd::initialize()
{
	OpticalSignal_In.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	OpticalSignal_In.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	OpticalSignal_In.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	LocalOscillator_In.setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	LocalOscillator_In.setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	LocalOscillator_In.setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());

	TiAmplifier_0_.setSaveInternalSignals(getSaveInternalSignals());
	TiAmplifier_0_.setLogValue(getLogValue());
	TiAmplifier_0_.setLogFileName("TiAmplifier_0.txt");

	TiAmplifier_1_.setSaveInternalSignals(getSaveInternalSignals());
	TiAmplifier_1_.setLogValue(getLogValue());
	TiAmplifier_1_.setLogFileName("TiAmplifier_1.txt");

	setSuperBlockSystem(
		{	
			&OpticalHybrid_,
			&BalancedPhotodetector_0_,
			&BalancedPhotodetector_1_,
			&TiAmplifier_0_,
			&TiAmplifier_1_,
		}
	);

	outputSignals[0]->setSymbolPeriod(TiAmplifier_0_Out.getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(TiAmplifier_0_Out.getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(TiAmplifier_0_Out.getFirstValueToBeSaved());

	outputSignals[1]->setSymbolPeriod(TiAmplifier_1_Out.getSymbolPeriod());
	outputSignals[1]->setSamplingPeriod(TiAmplifier_1_Out.getSamplingPeriod());
	outputSignals[1]->setFirstValueToBeSaved(TiAmplifier_1_Out.getFirstValueToBeSaved());

	setSuperBlockSystemValues();
};

bool CoherentReceiverOpticalFrontEnd::runBlock()
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

	ready = TiAmplifier_0_Out.ready();
	space = outputSignals[0]->space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		outputSignals[0]->bufferPut(TiAmplifier_0_Out.bufferGet());
	}

	ready = TiAmplifier_1_Out.ready();
	space = outputSignals[1]->space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		outputSignals[1]->bufferPut(TiAmplifier_1_Out.bufferGet());
	}


	return alive;
};
