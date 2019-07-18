#ifndef ERROR_CORRECTION_H_
#define ERROR_CORRECTION_H_

#include "../../../include/netxpto_20180815.h"
#include "../include/message_processor_common_20190410.h"

enum class error_correction_role { Alice, Bob, Undefined };

class ErrorCorrection : public Block {

public:
	ErrorCorrection(initializer_list<Signal*> InputSig, initializer_list<Signal*> OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);

	bool runBlock(void);
	void setErrorCorrectionRole(error_correction_role role) { errorCorrectionRole = role; }
	void setPartitionSize(t_integer wsz) { if (wsz <0 || wsz > 150) { wsz = 150; } partitionSize = wsz; currentPartitionSize = wsz; }
	void setNumberOfPasses(t_integer np) { numPasses= np;}
	void setDoublePartitionSize(bool db) { doubleWindowSize = db; }
	void setMaximumSyndromeSize(t_integer maxss) { maximumSyndromeSize = maxss; }
	void setMinimumSyndromeSize(t_integer minss) { minimumSyndromeSize= minss; }
	void setMinimumNumberOfPartitions(t_integer mnp) { minimumNumberOfPartitions = mnp; }
	
	long int getInputBits(void) { return inputBits; }
	long int getOutputBits(void) { return outputBits; }
	long int getNumberOfExchangedParities(void) { return measuredParities; }

private:
	// Input Variables
	error_correction_role errorCorrectionRole{ error_correction_role::Undefined };
	t_integer partitionSize{ 10 };
	t_integer numPasses{ 4 };
	bool doubleWindowSize{ false };
	t_integer minimumSyndromeSize{ 512 };
	t_integer maximumSyndromeSize{ 512 };	// Maximum buffer sizes limit this. Can be increased as long as it is not higher than the variables
											// maximumECSyndromeSize on the AliceQKD and BobQKD blocks. This variable cannot be defined after initialization, as
											// currently it is not possible to change the buffer sizes.

	// State Variables
	long int inputBits{ 0 };
	long int outputBits{ 0 };
	long int measuredParities{ 0 };

	t_real minimumBER{ 0.001 };
	t_integer partitionSizeLB{ 0 };
	bool alreadyUsedBER{ true };
	t_real BER{ 0 };
	t_integer numBits{ 0 };
	t_integer maxData{ 512 };
	t_integer currentPartitionSize{ 10 };
	unsigned int currentPass{ 0 };
	t_integer numberOfPartitions{ 0 };
	bool errorCorrectionStatus{ false };
	bool currentPassStarted{ false };
	bool lookBackDone{ false };
	bool doingLookBack{ false };
	bool lookBackStarted{ false };
	vector <t_integer> scrambledKey{};
	vector <t_integer> scrambledKeyIdx{};
	vector<t_integer> startPositions{};
	vector<t_integer> endPositions{};
	vector<t_integer> totalSearchSpace{};
	vector <t_integer> parities{};
	vector <t_integer> parityResponses{};
	vector <t_integer> lookBackIdx{};
	t_integer parityCount{ 0 };
	t_integer testCount{ 0 };
	vector <t_integer> scramblingIdx{};
	//t_integer seed{ 129237 };
	vector<t_integer> dataOnHold{};
	t_integer correctParitiesInLoopBack{ 0 };
	t_integer correctedInLoopBack{ 0 };
	t_integer correctedBitsInLoopBack{ 0 };
	t_integer correctedBitsInPass{ 0 };
	t_integer minimumNumberOfPartitions{ 10 };
	t_integer correctedInLookBack{ 0 };
	t_integer correctParitiesInLookBack{ 0 };
	t_integer correctedBitsInLookBack{ 0 };
	unsigned int lookBackPass{ 0 };
	vector <vector <t_integer>> lookBackPassesIdx;
	vector <vector <t_integer>> keyIdx;
	vector <vector <t_integer>> reverseKeyIdx;
	vector <vector <t_integer>> keys;
	t_real q{ 0.73 };
	int leftoverBits{ 0 }; // There may be a difference between the number of processed bits and the number of received bits
	vector <t_integer> paritiesMsgOutput{};
	bool finishedSendingParitiesMsg{ true };
	bool sendingParitiesMsg{ false };
	vector <t_integer> paritiesMsgInput{};
	bool finishedReceivingParitiesMsg{ true };
	bool receivingParitiesMsg{ false };
	t_integer numberOfParitiesToReceive{ 0 };

	// Private methods
//	t_integer getParity(vector<t_integer> &scrambledKeyVector, const t_integer startPosition, const t_integer endPosition, t_integer& count, vector<t_integer>& originalKey, const vector<t_integer>& originalKeyIdx);
	t_integer getParity(vector<t_integer> &keyVector, const t_integer startPosition, const t_integer endPosition, t_integer& count, vector<vector<t_integer>>& keyVectors, const vector<vector<t_integer>>& keyIdxVectors, const vector<vector<t_integer>>& reverseKeyIdxVectors, const unsigned int pass);
	bool updateStartAndEndPositions(const t_integer& aliceParityResponse, t_integer& startPosition, t_integer& endPosition, const t_integer windowSize, const t_integer totalSearchSize);
	t_integer correctBit(t_integer bitValue);
	bool outputKey(vector<Signal*> outputSignals, vector<t_integer> &correctedKey);
//	bool generatePermutationsIdx(const vector <t_integer>& originalKey, vector <t_integer>& scrambledKey, vector <t_integer>& scrambledKeyIdx);
	bool generatePermutationsIdx_testing(const t_integer seed, const vector <t_integer>& originalKey, vector <t_integer>& scrambledKey, vector <t_integer>& scrambledKeyIdx, vector <t_integer>& newReverseScrambledKeyIdx);
	t_integer ber2partitionSize(t_real qValue, t_real BER);
//	bool scrambleKeyWithMatrix(const t_integer M, const t_integer N, t_integer& keyIdx, const vector<t_integer>& ORIGINAL_KEY, vector<t_integer>& scrambledKey, vector<t_integer>& bitsIdx);

};

#endif // !ERROR_CORRECTION_H_
