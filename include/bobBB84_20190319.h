#ifndef  BobBB84_H
#define BobBB84_H

# include "netxpto_20180815.h"

class bobBB84 : public Block {
	/* Input Parameters */

public:
	/*	Methods	*/
	bobBB84() {};

	bobBB84(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};
	void initialize(void);
	bool runBlock(void);

};

#endif // ! BobBB84_H

