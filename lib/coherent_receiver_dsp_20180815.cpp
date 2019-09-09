# include "../include/coherent_receiver_dsp_20180815.h"


void CoherentReceiverDSP::initialize()
{
	InPhase_In.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	InPhase_In.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	InPhase_In.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	Quadrature_In.setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	Quadrature_In.setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	Quadrature_In.setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());

	setSuperBlockSystem(
		{	
			&ReceiverFilter_0_,
			&ReceiverFilter_1_,
			&Sampler_0_,
			&Sampler_1_,
			&Decoder_
		}
	);

	outputSignals[0]->setSymbolPeriod(BinaryData_Out.getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(BinaryData_Out.getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(BinaryData_Out.getFirstValueToBeSaved());

	setSuperBlockSystemValues();
};

bool CoherentReceiverDSP::runBlock()
{
	string signalsFolderName = getSignalsFolderName();

	if (!experimental::filesystem::is_directory(signalsFolderName) || !experimental::filesystem::exists(signalsFolderName)) {
		experimental::filesystem::create_directory(signalsFolderName);
	}

	int ready = inputSignals[0]->ready();
	int space = InPhase_In.space();
	int process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_real electricalIn;
		inputSignals[0]->bufferGet(&electricalIn);
		InPhase_In.bufferPut(electricalIn);
	}

	ready = inputSignals[1]->ready();
	space = Quadrature_In.space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_real electricalIn;
		inputSignals[1]->bufferGet(&electricalIn);
		Quadrature_In.bufferPut(electricalIn);
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
