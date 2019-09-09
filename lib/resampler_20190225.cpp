# include "resampler_20190225.h"

# include <complex>
# include "..\algorithms\fft\fft_20180208.h"

Resampler::Resampler(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals) : SuperBlock(inputSignals, outputSignals) {
};

void Resampler::initialize()
{
	SignalIn.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	SignalIn.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	SignalIn.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	setSuperBlockSystem(
		{
			&Interspace0s_,
			&Upsampler_
		}
	);

	outputSignals[0]->setSymbolPeriod(SignalIn.getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(SignalIn.getSamplingPeriod() / Interspace0s_.getUpsamplingFactor());

	setLogValue(logValue);
	setLogFileName(logFileName);
	setSignalsFolderName(signalsFolderName);

};

bool Resampler::runBlock()
{

	if (!experimental::filesystem::is_directory(signalsFolderName) || !experimental::filesystem::exists(signalsFolderName)) {
		experimental::filesystem::create_directory(signalsFolderName);
	}

	int ready = inputSignals[0]->ready();
	int space = SignalIn.space();
	int process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_complex dataIn;
		inputSignals[0]->bufferGet(&dataIn);
		SignalIn.bufferPut(dataIn);
	}

	bool alive = SuperBlock::runBlock(signalsFolderName);

	if (firstTime) {
		setOpenFile(false);
		firstTime = false;
	}

	ready = Upsampled.ready();
	space = outputSignals[0]->space();

	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_complex opticalOut = Upsampled.bufferGet();
		outputSignals[0]->bufferPut(opticalOut);
	}

	return alive;
}
