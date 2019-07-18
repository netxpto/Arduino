#ifndef BOB_PROCESS_INCOMING_DATA_H_
#define BOB_PROCESS_INCOMING_DATA_H_

#include "../../../include/netxpto_20180815.h"

class BobIncomingDataProcessor : public Block {

public:

	BobIncomingDataProcessor(initializer_list<Signal*> InputSig, initializer_list<Signal*> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	void setShowNumberOfSamples(bool sns) { showNumberOfSamples = sns; };
	void setShowNumberOfSamplesInterval(t_integer nsi) { showNumberOfSamplesInterval = nsi; };

	
private:
	// Input parameters
	t_real controlBitNumber{ 5 };
	bool showNumberOfSamples{ true };
	t_integer showNumberOfSamplesInterval{ 50000 };

	// state variables
	t_real numberOfSamples{ 0 };
	t_real oldNumberOfSamples{ 0 };
};


#endif // !BOB_PROCESS_INCOMING_DATA_H_