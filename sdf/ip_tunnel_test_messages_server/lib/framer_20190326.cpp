# include "../include/framer_20190326.h"

void calculateHeader(t_integer bPosition, t_binary &bitValue, frame_header_mode framerMode, t_integer headerLen, t_integer headerSigLen, t_integer &toAdd);

void Framer::initialize(void)
{

	for (auto k : outputSignals)
	{
		k->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
		k->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	}

};

bool Framer::runBlock(void)
{

	bool blockActive{ false };

	auto ready = inputSignals[0]->ready();

	auto space = MAX_BUFFER_LENGTH;
	for (auto k : outputSignals)
	{
		space = (space < k->space()) ? space : k->space();
	}


	int nFrames = (int)floor((t_real)ready / ((t_real)frameLength - (t_real)headerLength));   // one bit in the frame needs two random bits (one bit value and one basis value)
	
	auto process = min(nFrames * frameLength, space);

	auto m{ 0 };
	signal_value_type signalType = outputSignals[0]->getValueType();
	switch (signalType) {
	case (signal_value_type::t_binary):
		while (m < process)
		{
			blockActive = true;

			t_binary bitValue;

			if (bitPosition < headerLength)
			{
				if (0 == bitPosition) {
					toAdd = frameCount;
				}

				calculateHeader(bitPosition, bitValue, framerMode, headerLength, headerSignalLength, toAdd);

				for (auto k : outputSignals)
				{
					k->bufferPut(bitValue);
				}
			}
			else
			{
				inputSignals[0]->bufferGet(&bitValue);

				for (auto k : outputSignals)
				{
					k->bufferPut(bitValue);
				}
			}
			m = m + 1;
			bitPosition++;
			bitPosition = bitPosition % frameLength;
		}
		break;
	case (signal_value_type::t_real):
		while (m < process)
		{
			blockActive = true;

			t_binary bitValue;
			t_real realSigValue;
			t_binary binSigValue;
			signal_value_type inputSignalType = inputSignals[0]->getValueType();



			if (bitPosition < headerLength)
			{
				if (0 == bitPosition) {
					toAdd = frameCount;
				}

				calculateHeader(bitPosition, bitValue, framerMode, headerLength, headerSignalLength,  toAdd);

				for (auto k : outputSignals)
				{
					k->bufferPut((t_real)bitValue);
				}
			}
			else
			{
				switch (inputSignalType) {
				case (signal_value_type::t_binary):
					inputSignals[0]->bufferGet(&binSigValue);

					for (auto k : outputSignals)
					{
						switch (binSigValue) {
						case (0):
							k->bufferPut((t_real)0);
							break;
						case (1):
							k->bufferPut((t_real)1);
							break;
						}
					}
					break;

				case (signal_value_type::t_real):
					inputSignals[0]->bufferGet(&realSigValue);

					for (auto k : outputSignals)
					{
						k->bufferPut(realSigValue);
					}
					break;
				}
			}
			m = m + 1;
			bitPosition++;
			bitPosition = bitPosition % frameLength;
		}
	}

	/*
	while (m < process)
	{
		blockActive = true;

		t_binary bitValue;

		if (bitPosition < headerLength)
		{
			if (0 == bitPosition) {
				toAdd = frameCount;
			}

			calculateHeader(bitPosition, bitValue, framerMode, headerLength, headerSignalLength, frameCount, toAdd);

			for (auto k : outputSignals)
			{
				k->bufferPut(bitValue);
			}
		}
		else
		{
			inputSignals[0]->bufferGet(&bitValue);

			for (auto k : outputSignals)
			{
				k->bufferPut(bitValue);
			}
		}
		m = m + 1;
		bitPosition++;
		bitPosition = bitPosition % frameLength;
	}
	*/

	return blockActive;
}

void calculateHeader(t_integer bPosition, t_binary &bitValue, frame_header_mode framerMode, t_integer headerLen, t_integer headerSigLen, t_integer &toAdd)
{
	switch (framerMode) {
	case frame_header_mode::Sequential: {
		if (bPosition < headerSigLen-2) {
			bitValue = 1;
		} else if (bPosition == headerSigLen - 2) {
			bitValue = 0;
		} else if (bPosition == headerSigLen - 1) {
			bitValue = 1;
		} else {
			bitValue = 1;
			int exponent = headerLen - headerSigLen - bPosition - 1;
			if (pow(2, exponent) > toAdd) bitValue = 0;
			toAdd -= bitValue * (int) pow(2, exponent);
		}
		break;
	}
	default: {
		cout << "Frame mode unavailable.";
		bitValue = 0;
		break;
	}
	}

}