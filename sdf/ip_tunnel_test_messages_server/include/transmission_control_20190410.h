#ifndef TRANSMISSION_CONTROL_H_
#define TRANSMISSION_CONTROL_H_

#include "../../../include/netxpto_20180815.h"

class TransmissionControl : public Block {
public:
	TransmissionControl(initializer_list<Signal*> InputSig, initializer_list<Signal*> OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);
};
#endif // !TRANSMISSION_CONTROL_H_