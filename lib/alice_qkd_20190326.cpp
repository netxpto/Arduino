#include "../include/alice_qkd_20190326.h"

void AliceQKD::initialize()
{
	// Setup Input Signals
	AliceData_In.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	AliceData_In.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	AliceData_In.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	AliceBasis_In.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	AliceBasis_In.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	AliceBasis_In.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	Messages_In.setSymbolPeriod(inputSignals[2]->getSymbolPeriod());
	Messages_In.setSamplingPeriod(inputSignals[2]->getSamplingPeriod());
	Messages_In.setFirstValueToBeSaved(inputSignals[2]->getFirstValueToBeSaved());

	/*
	//	BA4.setNumberOfSamples(1000);
		//BA4.setDisplayNumberOfSamples(true);
	BA11.setBitPeriod(0.0005);
	BA11.setMode(BinarySourceMode::DeterministicCyclic);
	BA11.setBitStream("01");
	BA6.setSymbolPeriod(alice_8.getSymbolPeriod() * 2);
	BA13.setMode(BinarySourceMode::DeterministicCyclic);
	BA13.setBitStream("10");
	BA13.setBitPeriod(0.0005);
	BA8.setSymbolPeriod(alice_11.getSymbolPeriod() * 2);
	BA9.setSymbolPeriod(0.001);
	//	BA15.setNumberOfSamples(500);
	//	BA15.setDisplayNumberOfSamples(true);
	*/
	
	// Setup System Block
	setSuperBlockSystem(
		{
			&Framer_Data_,
			&Framer_Basis_,
			&MessageProcessorAlice_
		}
	);

	// Setup Output Signals
	outputSignals[0]->setSymbolPeriod(Messages_Out.getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(Messages_Out.getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(Messages_Out.getFirstValueToBeSaved());

	outputSignals[1]->setSymbolPeriod(TransmissionEnable.getSymbolPeriod());
	outputSignals[1]->setSamplingPeriod(TransmissionEnable.getSamplingPeriod());
	outputSignals[1]->setFirstValueToBeSaved(TransmissionEnable.getFirstValueToBeSaved());

	outputSignals[2]->setSymbolPeriod(Framer_Data_Out_Bob.getSymbolPeriod());
	outputSignals[2]->setSamplingPeriod(Framer_Data_Out_Bob.getSamplingPeriod());
	outputSignals[2]->setFirstValueToBeSaved(Framer_Data_Out_Bob.getFirstValueToBeSaved());

	outputSignals[3]->setSymbolPeriod(Framer_Basis_Out_Bob.getSymbolPeriod());
	outputSignals[3]->setSamplingPeriod(Framer_Basis_Out_Bob.getSamplingPeriod());
	outputSignals[3]->setFirstValueToBeSaved(Framer_Basis_Out_Bob.getFirstValueToBeSaved());

	outputSignals[4]->setSymbolPeriod(CorrectedKey.getSymbolPeriod());
	outputSignals[4]->setSamplingPeriod(CorrectedKey.getSamplingPeriod());
	outputSignals[4]->setFirstValueToBeSaved(CorrectedKey.getFirstValueToBeSaved());

	// Setup Superblock state variables
	setLogValue(logValue);
	setLogFileName(logFileName);
	setSignalsFolderName(signalsFolderName);
}

bool AliceQKD::runBlock() {
	if (!experimental::filesystem::is_directory(signalsFolderName) || !experimental::filesystem::exists(signalsFolderName)) {
		experimental::filesystem::create_directory(signalsFolderName);
	}


	////////// INPUT SIGNALS /////////////
	int ready = inputSignals[0]->ready();
	int space = AliceData_In.space();
	int process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary dataIn;
		inputSignals[0]->bufferGet(&dataIn);
		AliceData_In.bufferPut(dataIn);
	}

	ready = inputSignals[1]->ready();
	space = AliceBasis_In.space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary basisIn;
		inputSignals[1]->bufferGet(&basisIn);
		AliceBasis_In.bufferPut(basisIn);
	}
	
	ready = inputSignals[2]->ready();
	space = Messages_In.space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_message messageIn;
		inputSignals[2]->bufferGet(&messageIn);
		Messages_In.bufferPut(messageIn);
	}

	///////////// RUN BLOCKS //////////////

	bool alive = SuperBlock::runBlock(signalsFolderName);

	////////// OUTPUT SIGNALS ////////////

	if (firstTime) {
		setOpenFile(false);
		firstTime = false;
	}

	ready = Messages_Out.ready();
	space = outputSignals[0]->space();

	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_message messageOut = Messages_Out.bufferGet();
		outputSignals[0]->bufferPut(messageOut);
	}

	ready = TransmissionEnable.ready();
	space = outputSignals[1]->space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_real enableOut = TransmissionEnable.bufferGet();
		outputSignals[1]->bufferPut(enableOut);
	}

	ready = Framer_Data_Out_Bob.ready();
	space = outputSignals[2]->space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary dataOut = Framer_Data_Out_Bob.bufferGet();
		outputSignals[2]->bufferPut(dataOut);
	}


	ready = Framer_Basis_Out_Bob.ready();
	space = outputSignals[3]->space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary basisOut = Framer_Basis_Out_Bob.bufferGet();
		outputSignals[3]->bufferPut(basisOut);
	}

	ready = CorrectedKey.ready();
	space = outputSignals[4]->space();

	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary keyOut = CorrectedKey.bufferGet();
		outputSignals[4]->bufferPut(keyOut);
	}

	ready = SiftedKey.ready();
	space = outputSignals[5]->space();

	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary keyOut = SiftedKey.bufferGet();
		outputSignals[5]->bufferPut(keyOut);
	}
	/////////////////////////////////////
	
	return alive;
}

/*
//Basic configuration
void AliceQKD::set(int opt) {

	if (opt == 0) {
		setNumberOfSamplesPerSymbol(16);
		setIqAmplitudes({ { -45.0,0.0 },{ 0.0,0.0 },{ 45.0,0.0 },{ 90.0,0.0 } });
	//	setNumberOfSamples(1000);
	}

	return;
}
*/