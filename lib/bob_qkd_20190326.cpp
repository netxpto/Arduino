#include "../include/bob_qkd_20190326.h"

void BobQKD::initialize()
{

	// Setup Input Signals
	Messages_In.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	Messages_In.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	Messages_In.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	TransmissionEnable.setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	TransmissionEnable.setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	TransmissionEnable.setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());

	DataFrame_In.setSymbolPeriod(inputSignals[2]->getSymbolPeriod());
	DataFrame_In.setSamplingPeriod(inputSignals[2]->getSamplingPeriod());
	DataFrame_In.setFirstValueToBeSaved(inputSignals[2]->getFirstValueToBeSaved());

	Basis_In.setSymbolPeriod(inputSignals[3]->getSymbolPeriod());
	Basis_In.setSamplingPeriod(inputSignals[3]->getSamplingPeriod());
	Basis_In.setFirstValueToBeSaved(inputSignals[3]->getFirstValueToBeSaved());

	setSuperBlockSystem(
		{
			&Framer_Basis_,
			&MessageProcessorBob_
		}
	);


	// Setup Output Signals
	outputSignals[0]->setSymbolPeriod(Messages_Out.getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(Messages_Out.getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(Messages_Out.getFirstValueToBeSaved());

	outputSignals[1]->setSymbolPeriod(Basis_Out.getSymbolPeriod());
	outputSignals[1]->setSamplingPeriod(Basis_Out.getSamplingPeriod());
	outputSignals[1]->setFirstValueToBeSaved(Basis_Out.getFirstValueToBeSaved());

	outputSignals[2]->setSymbolPeriod(SiftedKey.getSymbolPeriod());
	outputSignals[2]->setSamplingPeriod(SiftedKey.getSamplingPeriod());
	outputSignals[2]->setFirstValueToBeSaved(SiftedKey.getFirstValueToBeSaved());

	// Setup Superblock state variables
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
	int space = Messages_In.space();
	int process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_message messageIn;
		inputSignals[0]->bufferGet(&messageIn);
		Messages_In.bufferPut(messageIn);
	}

	ready = inputSignals[1]->ready();
	space = TransmissionEnable.space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_real transmissionIn;
		inputSignals[1]->bufferGet(&transmissionIn);
		TransmissionEnable.bufferPut(transmissionIn);
	}

	ready = inputSignals[2]->ready();
	space = DataFrame_In.space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_real dataIn;
		inputSignals[2]->bufferGet(&dataIn);
		DataFrame_In.bufferPut(dataIn);
	}

	ready = inputSignals[3]->ready();
	space = min(Basis_In.space(), Basis_Out.space());
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary basisIn;
		inputSignals[3]->bufferGet(&basisIn);
		Basis_In.bufferPut(basisIn);
		Basis_Out.bufferPut(basisIn);
	}
	///////////// RUN BLOCKS ///////////////

	bool alive = SuperBlock::runBlock(signalsFolderName);

	if (firstTime) {
		setOpenFile(false);
		firstTime = false;
	}

	////////// OUTPUT SIGNALS ////////////

	ready = Messages_Out.ready();
	space = outputSignals[0]->space();

	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_message messageOut = Messages_Out.bufferGet();
		outputSignals[0]->bufferPut(messageOut);
	}
	
	/////////////

	ready = Basis_Out.ready();
	space = outputSignals[1]->space();

	process = min(ready, space);


	for (auto k = 0; k != process; k++)
	{
		t_binary basisOut = Basis_Out.bufferGet();
		outputSignals[1]->bufferPut(basisOut);
	}


	/////////////

	

	ready = CorrectedKey.ready();
	space = outputSignals[2]->space();

	process = min(ready, space);


	for (auto k = 0; k != process; k++)
	{
		t_binary keyOut = CorrectedKey.bufferGet();
		outputSignals[2]->bufferPut(keyOut);
	}


	/////
	ready = SiftedKey.ready();
	space = outputSignals[3]->space();

	process = min(ready, space);


	for (auto k = 0; k != process; k++)
	{
		t_binary keyOut = SiftedKey.bufferGet();
		outputSignals[3]->bufferPut(keyOut);
	}

	/////////////////////////////////////





	return alive;
}