# ifndef QBER_H
# define QBER_H

# include "netxpto_20180815.h"

class Qber : public Block {
public:
	//##############################################################################################################

	Qber(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig) : Block(InputSig, OutputSig) {};
	Qber(vector<Signal *> &InputSig, vector<Signal *> &OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################
	//###############################################################################################################
	double alpha = 0.05;
	int limit = 0;
	
private:
	//################################State variables################################################################
	double z;
	int firstPass{ 1 };
	double coincidences = 0.0;
	double receivedBits = 0.0;
	int posDataSeq{ 0 };
	int n{ 0 };
	double noClicks{ 0.0 };
	double doubleClicks{ 0.0 };
	int m{ 5000 }; 
	int totalReceivedBits{ 0 };
	vector <int> windowBuffer;
	int window{ -1 };
	int inBuffer{ 0 };
	bool firstFull{ true };
	double QBER{ 0.0 };
	int lengthWindow{ 1000 };
	bool emptyArray{ true };
	int cBit{ 0 };
};

#endif // !ANDGATE_H

