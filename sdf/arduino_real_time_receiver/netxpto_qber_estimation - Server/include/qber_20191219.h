# ifndef QBER_H
# define QBER_H

# include "netxpto_20180815.h"

enum class sync_mode { On_BobAhead, On_AliceAhead, Off };

class Qber : public Block {
public:
	//##############################################################################################################

	//Qber(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig) : Block(InputSig, OutputSig) {};
	Qber(vector<Signal *> InputSig, vector<Signal *> OutputSig) : Block(InputSig, OutputSig) {}; // Deixar estar

	void initialize(void);
	bool runBlock(void);

	void setSyncMode(sync_mode sMode) { syncMode = sMode; }
	sync_mode const getSyncMode(void) { return syncMode; }

	void setBerLimit(double bLimit) { berLimit = bLimit; }
	double const getBerLimit(void) { return berLimit; }

	void setBufferSize(int bSize) { bufferSize = bSize; }
	int const getBufferSize(void) { return bufferSize; }

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
	int m{ 8000 };
	int totalReceivedBits{ 0 };
	vector <int> windowBuffer;
	int window{ 1000 };
	int inBuffer{ 0 };
	bool firstFull{ true };
	double QBER{ 0.0 };
	int lengthWindow{ 1000 };
	bool emptyArray{ true };
	int cBit{ 0 };

	int numberSyncLossReport = 0;
	int inBufferSync{ 0 };
	double coincidencesSync = 0.0;
	bool discard = false;
	bool nSync = true; // if it is not synchronized, this bool is true
	double berLimit = 10; // value of BER (in percentage) under which the sequences are assumed to be synchronized
	int bufferSize = 1000; // number of bits used to test synchronization status
	sync_mode syncMode{ sync_mode::Off }; // flag to set functioning mode
};

#endif // !ANDGATE_H

