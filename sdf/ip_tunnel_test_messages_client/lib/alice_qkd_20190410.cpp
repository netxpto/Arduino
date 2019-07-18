#include "../include/alice_qkd_20190410.h"

void AliceQKD::initialize(void) {
	// Setup Input Signals
//	TransmissionMode_In.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
//	TransmissionMode_In.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
//	TransmissionMode_In.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	Data_In.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	Data_In.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	Data_In.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	Basis_In.setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	Basis_In.setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	Basis_In.setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());

	Messages_In.setSymbolPeriod(inputSignals[2]->getSymbolPeriod());
	Messages_In.setSamplingPeriod(inputSignals[2]->getSamplingPeriod());
	Messages_In.setFirstValueToBeSaved(inputSignals[2]->getFirstValueToBeSaved());

	// Setup System Block
	setSuperBlockSystem(
		{
			&AliceIncomingDataProcessor_,
			&MessageProcessorReceiver_,
			&BasisReconciliation_,
			&ParameterEstimation_,
			&ErrorCorrection_,
			&MessageProcessorTransmitter_,
			&PrivacyAmplification_
		}
	);

	// Setup Output Signals

	outputSignals[0]->setSymbolPeriod(Data_Out.getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(Data_Out.getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(Data_Out.getFirstValueToBeSaved());

	outputSignals[1]->setSymbolPeriod(Basis_Out.getSymbolPeriod());
	outputSignals[1]->setSamplingPeriod(Basis_Out.getSamplingPeriod());
	outputSignals[1]->setFirstValueToBeSaved(Basis_Out.getFirstValueToBeSaved());

	outputSignals[2]->setSymbolPeriod(Basis_Out.getSymbolPeriod());
	outputSignals[2]->setSamplingPeriod(Basis_Out.getSamplingPeriod());
	outputSignals[2]->setFirstValueToBeSaved(Basis_Out.getFirstValueToBeSaved());

	outputSignals[3]->setSymbolPeriod(Key_Out.getSymbolPeriod());
	outputSignals[3]->setSamplingPeriod(Key_Out.getSamplingPeriod());
	outputSignals[3]->setFirstValueToBeSaved(Key_Out.getFirstValueToBeSaved());

	outputSignals[4]->setSymbolPeriod(Messages_Out.getSymbolPeriod());
	outputSignals[4]->setSamplingPeriod(Messages_Out.getSamplingPeriod());
	outputSignals[4]->setFirstValueToBeSaved(Messages_Out.getFirstValueToBeSaved());

	if (outputSignals.size() >= 6) {
		outputSignals[5]->setSymbolPeriod(Key_Out.getSymbolPeriod());
		outputSignals[5]->setSamplingPeriod(Key_Out.getSamplingPeriod());
		outputSignals[5]->setFirstValueToBeSaved(Key_Out.getFirstValueToBeSaved());
	}

	// Configure blocks as necessary
	BasisReconciliation_.setBasisReconciliationRole(basis_reconciliation_role::Alice);
	ParameterEstimation_.setParameterEstimationRole(parameter_estimation_role::Alice);
	ErrorCorrection_.setErrorCorrectionRole(error_correction_role::Alice);
	//MessageProcessorTransmitter_.setSignalTypes(transmitterMsgTypes);

	setSuperBlockSystemValues();
}

bool AliceQKD::runBlock(void) {

	string sFolderName = getSignalsFolderName();

	if (!experimental::filesystem::is_directory(sFolderName) || !experimental::filesystem::exists(sFolderName)) {
		experimental::filesystem::create_directory(sFolderName);
	}

	////////// INPUT SIGNALS /////////////
/*
	int ready = inputSignals[0]->ready();
	int space = TransmissionMode_In.space();
	int process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary transmissionIn;
		inputSignals[0]->bufferGet(&transmissionIn);
		TransmissionMode_In.bufferPut(transmissionIn);
	}
*/
	int ready = inputSignals[0]->ready();
	int space = Data_In.space();
	int process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary dataIn;
		inputSignals[0]->bufferGet(&dataIn);
		Data_In.bufferPut(dataIn);
	}

	ready = inputSignals[1]->ready();
	space = Basis_In.space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary basisIn;
		inputSignals[1]->bufferGet(&basisIn);
		Basis_In.bufferPut(basisIn);
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

	bool alive = SuperBlock::runBlock(sFolderName);

	////////// OUTPUT SIGNALS ////////////

	if (firstTime) {
		setOpenFile(false);
		firstTime = false;
	}

	ready = Data_Out.ready();
	space = outputSignals[0]->space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary dataOut = Data_Out.bufferGet();
		outputSignals[0]->bufferPut(dataOut);
	}
	sentQubits = sentQubits + process;
	
	ready = Basis_Out.ready();
	space = outputSignals[1]->space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary basisOut = Basis_Out.bufferGet();
		outputSignals[1]->bufferPut(basisOut);
	}

	ready = AliceTransmissionMode_Out.ready();
	space = outputSignals[2]->space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary tmOut = AliceTransmissionMode_Out.bufferGet();
		outputSignals[2]->bufferPut(tmOut);
	}

	if (outputSignals.size() >= 6) {
		ready = SiftedKeys.ready();
		space = outputSignals[5]->space();
		process = min(ready, space);

		for (auto k = 0; k != process; k++)
		{
			t_binary keyOut = SiftedKeys.bufferGet();
			outputSignals[5]->bufferPut(keyOut);
		}
	}
	else {
		ready = SiftedKeys.ready();
		process = ready;
		t_binary keyOut;
		for (auto k = 0; k != process; k++)
		{
			keyOut = SiftedKeys.bufferGet();
		}
	}

	ready = Key_Out.ready();
	space = outputSignals[3]->space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary keyOut = Key_Out.bufferGet();
		outputSignals[3]->bufferPut(keyOut);
		keyBits++;
	}

	if (keyBits >= lastKeyBits + dispBitsInterval) {
		cout << "Alice - Current Key Size: " << keyBits << "\n";
		lastKeyBits = keyBits;

		long int brBitsIn = BasisReconciliation_.getReceivedQubits();
		long int brBitsOut = BasisReconciliation_.getReconciledBits();

		long int peBitsIn = ParameterEstimation_.getInputBits();
		long int peBitsOut = ParameterEstimation_.getOutputBits();
		double averageQBER = ParameterEstimation_.getAverageEstimatedBER();

		long int ecBitsIn = ErrorCorrection_.getInputBits();
		long int ecBitsOut = ErrorCorrection_.getOutputBits();

		long int paBitsIn = PrivacyAmplification_.getInputBits();
		long int paBitsOut = PrivacyAmplification_.getOutputBits();

		ofstream myfile;
		myfile.open(sFolderName + "/" + reportFileName);
		myfile << "Total received qubits:            " << sentQubits << "\n";
		myfile << "Average QBER:                     " << averageQBER << "\n";
		myfile << "Key after basis reconciliation:   " << brBitsOut << "\n";
		myfile << "Key after parameter estimation:   " << peBitsOut << "\n";
		myfile << "Key after error correction:       " << ecBitsOut << "\n";
		myfile << "Final key size:                   " << paBitsOut << "\n";
		myfile << "##########################################################\n";
		myfile << "Basis reconciliation efficiency:  " << (double)brBitsOut / (double)brBitsIn << "\n";
		myfile << "Parameter estimation efficiency:  " << (double)peBitsOut / (double)peBitsIn << "\n";
		myfile << "Error correction efficiency:      " << (double)ecBitsOut / (double)ecBitsIn << "\n";
		myfile << "Privacy amplification key ratio:  " << (double)paBitsOut / (double)paBitsIn << "\n";
		myfile << "Overall efficiency:               " << (double)paBitsOut / (double)sentQubits << "\n";
		myfile.close();
	}

	ready = Messages_Out.ready();
	space = outputSignals[4]->space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_message messageOut = Messages_Out.bufferGet();
		outputSignals[4]->bufferPut(messageOut);
	}

	if (!alive)
	{
		long int brBitsIn = BasisReconciliation_.getReceivedQubits();
		long int brBitsOut = BasisReconciliation_.getReconciledBits();

		long int peBitsIn = ParameterEstimation_.getInputBits();
		long int peBitsOut = ParameterEstimation_.getOutputBits();
		double averageQBER = ParameterEstimation_.getAverageEstimatedBER();

		long int ecBitsIn = ErrorCorrection_.getInputBits();
		long int ecBitsOut = ErrorCorrection_.getOutputBits();

		long int paBitsIn = PrivacyAmplification_.getInputBits();
		long int paBitsOut = PrivacyAmplification_.getOutputBits();

		ofstream myfile;
		myfile.open(sFolderName + "/" + reportFileName);
		myfile << "Total received qubits:            " << sentQubits << "\n";
		myfile << "Average QBER:                     " << averageQBER << "\n";
		myfile << "Key after basis reconciliation:   " << brBitsOut << "\n";
		myfile << "Key after parameter estimation:   " << peBitsOut << "\n";
		myfile << "Key after error correction:       " << ecBitsOut << "\n";
		myfile << "Final key size:                   " << paBitsOut << "\n";
		myfile << "##########################################################\n";
		myfile << "Basis reconciliation efficiency:  " << (double)brBitsOut / (double)brBitsIn << "\n";
		myfile << "Parameter estimation efficiency:  " << (double)peBitsOut / (double)peBitsIn << "\n";
		myfile << "Error correction efficiency:      " << (double)ecBitsOut / (double)ecBitsIn << "\n";
		myfile << "Privacy amplification key ratio:  " << (double)paBitsOut / (double)paBitsIn << "\n";
		myfile << "Overall efficiency:               " << (double)paBitsOut / (double)sentQubits << "\n";
		myfile.close();
	}
	/////////////////////////////////////

	return alive;
}
