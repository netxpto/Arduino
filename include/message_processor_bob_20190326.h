#ifndef MESSAGE_PROCESSOR_BOB_H
#define MESSAGE_PROCESSOR_BOB_H

#include <sstream>
#include <iostream>
#include <vector>
#include "../include/bb84_st_message_processors_common.h"

using namespace std;

class MessageProcessorBob : public Block {

public:
	/*Public Methods*/
	MessageProcessorBob() {};
	MessageProcessorBob(initializer_list<Signal*> InputSig, initializer_list<Signal*> OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	void setCascadeWindowSize(t_integer cw) { cascadeWindowSize = cw; }
	void setCascadeNumPasses(t_integer np) { numPasses = np; }
private:
	/*Input Parameters*/
	t_message_data_length maxMessageDataLength{ 100 };
	t_integer maxNumberOfStoredMessages{ 1 };
	t_integer maxOfStoredBasis{ 8192 };
	t_integer maxOfStoredData{ 1000 };
	t_integer cascadeWindowSize{ 10 };
	t_integer windowsPerMessage{ 10 };

	/*State Variables*/
	vector<t_message> storedMessages;
	int numberOfStoredMessages{ 0 };
	int numberOfStoredBasis{ 0 };
	int numberOfFrames{ 0 };
	vector<t_integer> storedBasis;
	vector<t_real> storedData;
	vector<vector<int>> storedBasisFrames;
	vector<vector<int>> storedDataFrames;
	vector<int> storedReceivedData;
	vector<int> storedReceivedBasis;
	int transmissionStatus{ 1 };
	int numBits{ 0 };
	vector<vector<t_binary>> keys; // Initialized in .cpp
	vector<vector<t_binary>> scrambledKeys; // Initialized in .cpp
	vector<vector<t_integer>> scrambledKeysIdx; // Initialized in .cpp

	t_integer numPasses{ 1 };
	int corrected1{ 0 };
	int corrected2{ 0 };
	int corrected3{ 0 };
	t_integer keyIdx{ 0 };

	// Cascade State Variables
	t_integer currentCascadeWindowSet{0};
	vector<t_integer> cascadeStartPositions{};
	vector<t_integer> cascadeEndPositions{};
	vector<t_integer> totalPossibleSearchSize{};
	vector<vector<t_binary>> cascadeWindows{};
	t_integer cascadeNumbitsBob{ 0 };


	/*Private Methods*/
	t_message_data_length getMessageDataLength(const t_message& msg);
	void setMessageDataLength(t_message msg, t_message_data_length mDataLength) {msg.messageDataLength = to_string(mDataLength);};
	
	void setMessageType(t_message msg, t_message_type mType) { msg.messageType = mType; };
	t_message_data getMessageData(const t_message& msg, t_message_data_length dataLength);
	void setMessageData(t_message msg, string mData) { msg.messageData = mData; };

	

	bool ProcessDataAndBasisToStore();
	bool ProcessBasisReconcilliationToSend();
	bool ProcessReceivedMessage();
	bool ProcessStoredMessage();
	bool transmissionControl();
	bool outputKey();
/*	bool cascadeProcessMessageToBob(const t_message& msg, t_message_data_length dataLength, vector<t_integer> &parities, t_integer &currentCascadeWindowSet);
	t_integer cascadeGetParity(const vector<t_binary> cascadeWindow, const t_integer startPosition, const t_integer endPosition);
	bool cascadeUpdateStartAndEndPositions(const t_integer &aliceParity, t_integer &startPosition, t_integer &endPosition);
	void cascadeCorrectBit(t_binary &bitValue);
	bool cascadeTestIfNoErrors(vector<t_integer> &bobParities);*/
	//bool ProcessSiftedKey();
};
	
#endif // !MESSAGE_PROCESSOR_BOB_H

