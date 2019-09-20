# include "real2complex.h"


using namespace std;


void Real2Complex::initialize(void) {

	firstTime = false;

	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	

}

bool Real2Complex::runBlock(void) {
	
	
	double symbolPeriod = outputSignals[0]->getSymbolPeriod();


	int ready0 = inputSignals[0]->ready();
	int ready1 = inputSignals[1]->ready();
	int ready = min(ready0, ready1);

	int space = outputSignals[0]->space();

	int process = min(ready, space);

	if (process == 0) return false;

	t_real re, im;
	for (int i = 0; i < process; i++) {

		inputSignals[0]->bufferGet(&re);
		inputSignals[1]->bufferGet(&im);

		t_complex valueX(re, im);
		
		outputSignals[0]->bufferPut(valueX);
			
	}

	return true;	

}
