# ifndef SUPER_BLOCK_INTERFACE_H_
# define SUPER_BLOCK_INTERFACE_H_

# include <algorithm> // min
# include "netxpto_20180815.h"

class SuperBlockInterface : public Block {

	/* input parameters */


	/* state variables */


public:

	SuperBlockInterface() :Block() {};
	SuperBlockInterface(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig);

	void initialize(void);

	bool runBlock(void);
		
};

#endif
