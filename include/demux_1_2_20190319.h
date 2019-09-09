#ifndef DEMUX_1_2_H
#define DEMUX_1_2_H

#include "netxpto_20180815.h"

class Demux_1_2 : public Block {

public:
	/*input parameters*/
	double symbolPeriod{ 1 };

	/*Methods*/
	Demux_1_2() {};
	Demux_1_2(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);

	bool runBlock(void);

	void setSymbolPeriod(double sPeriod) { symbolPeriod = sPeriod; };
	double getSymbolPeriod() { return symbolPeriod; };

};

#endif
