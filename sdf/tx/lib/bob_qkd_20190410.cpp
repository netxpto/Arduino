#include "../include/bob_qkd_20190410.h"

void BobQKD::initialize(void) {
	// Setup Input Signals

	Data_In.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	Data_In.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	Data_In.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	Basis_In.setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	Basis_In.setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	Basis_In.setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());

	TransmissionMode_In.setSymbolPeriod(inputSignals[2]->getSymbolPeriod());
	TransmissionMode_In.setSamplingPeriod(inputSignals[2]->getSamplingPeriod());
	TransmissionMode_In.setFirstValueToBeSaved(inputSignals[2]->getFirstValueToBeSaved());

	Messages_In.setSymbolPeriod(inputSignals[3]->getSymbolPeriod());
	Messages_In.setSamplingPeriod(inputSignals[3]->getSamplingPeriod());
	Messages_In.setFirstValueToBeSaved(inputSignals[3]->getFirstValueToBeSaved());

	// Setup System Block
	setSuperBlockSystem(
		{
			&BobIncomingDataProcessor_,
			&MessageProcessorReceiver_,
			&BasisReconciliation_,
			&ParameterEstimation_,
			&ErrorCorrection_,
			&MessageProcessorTransmitter_,
			&PrivacyAmplification_
		}
	);

	// Setup Output Signals

	outputSignals[0]->setSymbolPeriod(BasisIn.getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(BasisIn.getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(BasisIn.getFirstValueToBeSaved());

	outputSignals[1]->setSymbolPeriod(Key_Out.getSymbolPeriod());
	outputSignals[1]->setSamplingPeriod(Key_Out.getSamplingPeriod());
	outputSignals[1]->setFirstValueToBeSaved(Key_Out.getFirstValueToBeSaved());

	outputSignals[2]->setSymbolPeriod(Messages_Out.getSymbolPeriod());
	outputSignals[2]->setSamplingPeriod(Messages_Out.getSamplingPeriod());
	outputSignals[2]->setFirstValueToBeSaved(Messages_Out.getFirstValueToBeSaved());

	// Optional sifted Key
	if (outputSignals.size() >= 4) {
		outputSignals[3]->setSymbolPeriod(Key_Out.getSymbolPeriod());
		outputSignals[3]->setSamplingPeriod(Key_Out.getSamplingPeriod());
		outputSignals[3]->setFirstValueToBeSaved(Key_Out.getFirstValueToBeSaved());
	}

	// Configure blocks as necessary
	BasisReconciliation_.setBasisReconciliationRole(basis_reconciliation_role::Bob);
	ParameterEstimation_.setParameterEstimationRole(parameter_estimation_role::Bob);
	ErrorCorrection_.setErrorCorrectionRole(error_correction_role::Bob);
	//MessageProcessorTransmitter_.setSignalTypes(transmitterMsgTypes);

	// Setup Superblock state variables
	setSuperBlockSystemValues();

}

bool BobQKD::runBlock(void) {

	string sFolderName = getSignalsFolderName();

	if (!experimental::filesystem::is_directory(sFolderName) || !experimental::filesystem::exists(sFolderName)) {
		experimental::filesystem::create_directory(sFolderName);
	}

	////////// INPUT SIGNALS /////////////

	int ready = inputSignals[0]->ready();
	int space = Data_In.space();
	int process = min(ready, space);

	if (inputSignals[0]->getType() == "Binary") {
		{
			for (auto k = 0; k != process; k++)
			{
				t_binary dataIn;
				inputSignals[0]->bufferGet(&dataIn);
				Data_In.bufferPut((t_real)dataIn);
			}
		}
	}
	else if ((inputSignals[0]->getType() == "TimeContinuousAmplitudeDiscreteReal") || (inputSignals[0]->getType() == "TimeDiscreteAmplitudeContinuousReal")) {
		for (auto k = 0; k != process; k++)
		{
			t_real dataIn;
			inputSignals[0]->bufferGet(&dataIn);
			Data_In.bufferPut(dataIn);
		}
	}
	receivedQubits = receivedQubits + process;

	ready = inputSignals[1]->ready();
	space = min(Basis_In.space(), outputSignals[0]->space());
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary basisIn;
		inputSignals[1]->bufferGet(&basisIn);
		Basis_In.bufferPut(basisIn);
		outputSignals[0]->bufferPut(basisIn);
	}

	ready = inputSignals[2]->ready();
	space = TransmissionMode_In.space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary transmissionModeIn;
		inputSignals[2]->bufferGet(&transmissionModeIn);
		TransmissionMode_In.bufferPut(transmissionModeIn);
	}

	ready = inputSignals[3]->ready();
	space = Messages_In.space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_message messageIn;
		inputSignals[3]->bufferGet(&messageIn);
		Messages_In.bufferPut(messageIn);
	}

	ready = inputSignals[4]->ready();
	//	space = ;
	process = ready;

	for (auto k = 0; k != process; k++)
	{
		t_real berIn;
		inputSignals[3]->bufferGet(&berIn);
		//		Messages_In.bufferPut(messageIn);
	}

	///////////// RUN BLOCKS //////////////

	bool alive = SuperBlock::runBlock(sFolderName);

	if (alive == false)
	{
		cout << "";
	}
	////////// OUTPUT SIGNALS ////////////

	if (firstTime) {
		setOpenFile(false);
		firstTime = false;
	}

	ready = Key_Out.ready();
	space = outputSignals[1]->space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary keyOut = Key_Out.bufferGet();
		outputSignals[1]->bufferPut(keyOut);
		keyBits++;
	}

	if (keyBits >= lastKeyBits + dispBitsInterval) {
		cout << "Bob   - Current Key Size: " << keyBits << "\n";
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
		myfile << "Total received qubits:            " << receivedQubits << "\n";
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
		myfile << "Overall efficiency:               " << (double)paBitsOut / (double)receivedQubits << "\n";
		myfile.close();
	}

	ready = Messages_Out.ready();
	space = outputSignals[2]->space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_message messageOut = Messages_Out.bufferGet();
		outputSignals[2]->bufferPut(messageOut);
	}

	if (outputSignals.size() >= 4) {
		ready = SiftedKeys.ready();
		space = outputSignals[3]->space();
		process = min(ready, space);

		for (auto k = 0; k != process; k++)
		{
			t_binary siftedKeys = SiftedKeys.bufferGet();
			outputSignals[3]->bufferPut(siftedKeys);
		}
	}
	else {
		ready = SiftedKeys.ready();
		process = ready;
		t_binary siftedKeys;

		for (auto k = 0; k != process; k++)
		{
			siftedKeys = SiftedKeys.bufferGet();
		}
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
		myfile << "Total received qubits:            " << receivedQubits << "\n";
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
		myfile << "Overall efficiency:               " << (double)paBitsOut / (double)receivedQubits << "\n";
		myfile.close();
	}
	/*	ready = TransmissionMode_Out.ready();
		space = outputSignals[2]->space();
		process = min(ready, space);

		for (auto k = 0; k != process; k++)
		{
			t_binary transmissionOut = TransmissionMode_Out.bufferGet();
			outputSignals[2]->bufferPut(transmissionOut);
		}
	*/	/////////////////////////////////////

	return alive;
}
