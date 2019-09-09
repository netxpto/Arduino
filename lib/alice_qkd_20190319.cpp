#include "alice_qkd_20190319.h"

void AliceQKD::initialize()
{
	ClockAlice_In.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	ClockAlice_In.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	ClockAlice_In.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	BinaryData_In_1.setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	BinaryData_In_1.setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	BinaryData_In_1.setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());

	BinaryData_In_2.setSymbolPeriod(inputSignals[2]->getSymbolPeriod());
	BinaryData_In_2.setSamplingPeriod(inputSignals[2]->getSamplingPeriod());
	BinaryData_In_2.setFirstValueToBeSaved(inputSignals[2]->getFirstValueToBeSaved());

	BinaryData_In_3.setSymbolPeriod(inputSignals[3]->getSymbolPeriod());
	BinaryData_In_3.setSamplingPeriod(inputSignals[3]->getSamplingPeriod());
	BinaryData_In_3.setFirstValueToBeSaved(inputSignals[3]->getFirstValueToBeSaved());

	Messages_In.setSymbolPeriod(inputSignals[4]->getSymbolPeriod());
	Messages_In.setSamplingPeriod(inputSignals[4]->getSamplingPeriod());
	Messages_In.setFirstValueToBeSaved(inputSignals[4]->getFirstValueToBeSaved());

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

	setSuperBlockSystem(
		{
			&BA1,
			&BA2,
			&BA4,
			&BA3,
			&BA11,
			&BA6,
			&BA12,
			&BA13,
			&BA8,
			&BA14,
			&BA7,
			&BA9,
			&BA15
		}
	);

//	outputSignals[0]->setSymbolPeriod(ClockAlice_Out.getSymbolPeriod());
//	outputSignals[0]->setSamplingPeriod(ClockAlice_Out.getSamplingPeriod());
//	outputSignals[0]->setFirstValueToBeSaved(ClockAlice_Out.getFirstValueToBeSaved());

	outputSignals[1]->setSymbolPeriod(Polarization_out.getSymbolPeriod());
	outputSignals[1]->setSamplingPeriod(Polarization_out.getSamplingPeriod());
	outputSignals[1]->setFirstValueToBeSaved(Polarization_out.getFirstValueToBeSaved());

	outputSignals[2]->setSymbolPeriod(MutualInformation_out.getSymbolPeriod());
	outputSignals[2]->setSamplingPeriod(MutualInformation_out.getSamplingPeriod());
	outputSignals[2]->setFirstValueToBeSaved(MutualInformation_out.getFirstValueToBeSaved());

	outputSignals[3]->setSymbolPeriod(Messages_Out.getSymbolPeriod());
	outputSignals[3]->setSamplingPeriod(Messages_Out.getSamplingPeriod());
	outputSignals[3]->setFirstValueToBeSaved(Messages_Out.getFirstValueToBeSaved());

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
	int space = ClockAlice_In.space();
	int process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_real clockIn;
		inputSignals[0]->bufferGet(&clockIn);
		ClockAlice_In.bufferPut(clockIn);
	}

	ready = inputSignals[1]->ready();
	space = BinaryData_In_1.space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary binaryIn;
		inputSignals[1]->bufferGet(&binaryIn);
		BinaryData_In_1.bufferPut(binaryIn);
	}

	ready = inputSignals[2]->ready();
	space = BinaryData_In_2.space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary binaryIn;
		inputSignals[2]->bufferGet(&binaryIn);
		BinaryData_In_2.bufferPut(binaryIn);
	}

	ready = inputSignals[3]->ready();
	space = BinaryData_In_3.space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary binaryIn;
		inputSignals[3]->bufferGet(&binaryIn);
		BinaryData_In_3.bufferPut(binaryIn);
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

	////////// OUTPUT SIGNALS ////////////

	if (firstTime) {
		setOpenFile(false);
		firstTime = false;
	}

	ready = ClockAlice_In.ready();
	space = outputSignals[0]->space();

	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_real clockOut = ClockAlice_In.bufferGet();
		outputSignals[0]->bufferPut(clockOut);
	}

	ready = Polarization_out.ready();
	space = outputSignals[1]->space();

	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_real polOut = Polarization_out.bufferGet();
		outputSignals[1]->bufferPut(polOut);
	}

	ready = MutualInformation_out.ready();
	space = outputSignals[2]->space();

	process = min(ready, space);
	if (ready >= 512){
		for (auto k = 0; k != process; k++)
		{
			t_binary miOut = MutualInformation_out.bufferGet();
			outputSignals[2]->bufferPut(miOut);
		}
	}


	
	//////////// MESSAGES? //////////////
		ready = Messages_Out.ready();
	space = outputSignals[3]->space();

	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_message messagesOut = Messages_Out.bufferGet();
		outputSignals[3]->bufferPut((t_message)messagesOut);
	}
	/////////////////////////////////////
	

	return alive;
}

//Basic configuration
void AliceQKD::set(int opt) {

	if (opt == 0) {
		setNumberOfSamplesPerSymbol(16);
		setIqAmplitudes({ { -45.0,0.0 },{ 0.0,0.0 },{ 45.0,0.0 },{ 90.0,0.0 } });
	//	setNumberOfSamples(1000);
	}

	return;
}