#ifndef PARALLELTOSERIAL_H
#define PARALLELTOSERAL_H

# include "../include/netxpto_20180815.h"

class ParallelToSerial : public Block {
public:
	//##############################################################################################################

	ParallelToSerial(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig) : Block(InputSig, OutputSig) {};
	ParallelToSerial(vector<Signal *> &InputSig, vector<Signal *> &OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

	//###############################################################################################################
private:
	//###############################################################################################################
	//State Variables################################################################################################
	int inputOut{ 0 };
};

#endif // !PARALLELTOSERIAL_H
