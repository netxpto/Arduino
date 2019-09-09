# include "../include/alice_20190401.h"

Alice::Alice(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals) : SuperBlock(inputSignals, outputSignals) {

	//###################################### Default Input Parameters Values #########################################################
	
	double bitRate{ 1e3 };
	int numberOfBits{ 1000 };
	int frameLength{ 10 };
	int headerLength{ 0 };
	vector<vector<t_real>> iqAmplitudesValues{ { 0, 0, 0, 1 },{ PI / 2, 0, 1,1 },{ -PI / 4, 1, 0, 1 },{ PI / 4, 1, 1, 1 } };
	// the in-phase componente is used to select both the basis and the bit, 00 => 0 (basis=0, bit=0), 01 => PI/2, 10 => -PI/4, 11 => PI/
	// the quadrature componente is used as a clock
	pulse_shapper_filter_type filterType{ pulse_shapper_filter_type::Square };
	int samplesPerSymbol{ 16 };
	long int numberOfSymbols{ 1000 };
	
	
	//############################################# Blocks ##########################################################################

	QuantumFramer_.setFrameLength(frameLength);
	QuantumFramer_.setHeaderLength(headerLength);

	MQamMapper_.setIqAmplitudes(iqAmplitudesValues);

	DiscreteToContinuousTime_Basis.setNumberOfSamplesPerSymbol(samplesPerSymbol);

	PulseShaper_Basis.setFilterType(filterType);
	PulseShaper_Basis.setPulseWidth(1 / bitRate);

	DiscreteToContinuousTime_Bits.setNumberOfSamplesPerSymbol(samplesPerSymbol);

	PulseShaper_Bits.setFilterType(filterType);
	PulseShaper_Bits.setPulseWidth(1 / bitRate);
	
	DiscreteToContinuousTime_.setNumberOfSamplesPerSymbol(samplesPerSymbol);

	PulseShaper_.setFilterType(filterType);
	PulseShaper_.setSaveImpulseResponse(true);

	DiscreteToContinuousTime_1.setNumberOfSamplesPerSymbol(samplesPerSymbol);

	TimeDelay_.setTimeDelay(1 / bitRate / 2);
	TimeDelay_.setTimeDelay(0);

	Sink_MQM_Alice_bits.setDisplayNumberOfSamples(true);

};

void Alice::initialize()
{
		BS_in_1.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
		BS_in_1.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
		BS_in_1.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
		BS_in_2.setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
		BS_in_2.setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
		BS_in_2.setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());

		//########################################################### SYSTEM ###################################################################

		setSuperBlockSystem(
			{
				&QuantumFramer_,
				&MQamMapper_,
				&DiscreteToContinuousTime_Basis,
				&PulseShaper_Basis,
				&Sink_MQM_Alice_basis,
				&DiscreteToContinuousTime_Bits,
				&PulseShaper_Bits,
				&Sink_MQM_Alice_bits,
				&DiscreteToContinuousTime_,
				&PulseShaper_,
				&DiscreteToContinuousTime_1,
				&TimeDelay_,
				&SinglePhotonSource_,
				&Polarizer_ }
		);

		outputSignals[0]->setSymbolPeriod(Alice_out_2.getSymbolPeriod());
		outputSignals[0]->setSamplingPeriod(Alice_out_2.getSamplingPeriod());
		outputSignals[0]->setFirstValueToBeSaved(Alice_out_2.getFirstValueToBeSaved());

		setLogValue(logValue);
		setLogFileName(logFileName);
		setSignalsFolderName(signalsFolderName);
}

bool Alice::runBlock()
{
	/*Input*/
	auto ready_in = min(inputSignals[0]->ready(), inputSignals[1]->ready());

	auto space_in{ MAX_BUFFER_LENGTH };

	auto space_in_1 = BS_in_1.getBufferLength();
	space_in_1 = BS_in_1.space();
	auto space_in_2 = BS_in_2.getBufferLength();
	space_in_2 = BS_in_2.space();
	space_in = min(space_in_1, space_in_2);
	
	auto processIn = min(ready_in, space_in);

	for (auto k = 0; k < processIn; k++) {
		t_binary BasisIn, BitIn;
		inputSignals[0]->bufferGet(&BasisIn);
		inputSignals[1]->bufferGet(&BitIn);

		BS_in_1.bufferPut((t_binary)BasisIn);
		BS_in_2.bufferPut((t_binary)BitIn);
	}

	/*Output*/
	bool alive = SuperBlock::runBlock(signalsFolderName);

	if (firstTime) {
		setOpenFile(false);
		firstTime = false;
	};

	auto ready = Alice_out_2.ready();

	auto space{ MAX_BUFFER_LENGTH };
	for (auto k : outputSignals)
	{
		space = min(space, k->space());
	}
	
	auto process = min(ready, space);
	if (process > 0) alive = true;

	for (auto k = 0; k != process; k++)
	{
		t_complex_xy photon_xy_value = Alice_out_2.bufferGet();
		outputSignals[0]->bufferPut(photon_xy_value);
	}

	return alive;
}
