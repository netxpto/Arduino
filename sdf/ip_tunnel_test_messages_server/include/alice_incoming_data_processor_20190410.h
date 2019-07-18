#ifndef ALICE_PROCESS_INCOMING_DATA_H_
#define ALICE_PROCESS_INCOMING_DATA_H_

#include "../../../include/netxpto_20180815.h"

class AliceIncomingDataProcessor: public Block {

public:

	AliceIncomingDataProcessor(initializer_list<Signal*> InputSig, initializer_list<Signal*> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

};


#endif // ! ALICE_PROCESS_INCOMING_DATA_H_