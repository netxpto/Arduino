#ifndef BINARY_MAPPER_H
#define BINARY_MAPPER_H
	
#include "netxpto_20180815.h"

class BinaryMapper : public Block {
	/* State Variables */

public:
	/* Input Parameters */
	int high{ 45 };
	int low{ 0 };

	/*Methods*/
	BinaryMapper() {};
	BinaryMapper(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);

	bool runBlock(void);

	void setHigh(int h) { high = h; }
	void setLow(int l) { low = l; }

};



#endif // !BINARY_MAPPER_H

