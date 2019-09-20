#ifndef MESSAGE_PROCESSOR_ALICE_H
#define MESSAGE_PROCESSOR_ALICE_H

#include <math.h>
#include <string.h>

#include "../include/bb84_st_message_processors_common.h"

class MessageProcessorAlice : public Block {

public:

	/*Public Methods*/
	MessageProcessorAlice() {};
	MessageProcessorAlice(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals) : Block(inputSignals, outputSignals) {};
	

	void initialize(void);
	bool runBlock(void);

	void setMaxMessageDataLength(t_message_data_length maxDataLength) { maxMessageDataLength = maxDataLength; };
	t_message_data_length getMaxMessageDataLength() { return maxMessageDataLength; };

	void setCascadeWindowSize(t_integer cw) { cascadeWindowSize = cw; }
	void setCascadeMaxWindows(t_integer mw) { cascadeMaxWindows = mw; }
	void setCascadeNumPasses(t_integer np) { numPasses= np; }
private:

	/*Input Parameters*/
	t_message_data_length maxMessageDataLength{ 1000 };
	int maxNumberOfStoredMessages{ 1 };
	int maxOfStoredBasis{ 8192 };
	t_integer scramblingM{ 11 };
	t_integer scramblingN{ 11 };
	/*State Variables*/

	vector<t_message> storedMessages;
	int numberOfStoredMessages{ 0 };
	vector<t_binary> storedBasis;
	vector<t_binary> storedData;
	int numberOfStoredBasis{ 0 };
	int transmissionStatus{ 1 };
	int numBits{ 0 };
	vector<vector<t_binary>> keys; // Initialzed in .cpp
	vector<vector<t_binary>> scrambledKeys; // Initialzed in .cpp
	vector<vector<t_integer>> scrambledKeysIdx; // Initialzed in .cpp
	t_integer numPasses{ 1 };

	// Cascade State Variables
	bool cascadeActive{ false };
	t_integer cascadeWindowSize{ 10 };
	t_integer cascadeMaxWindows{ 10 };
	t_integer currentCascadeWindowSet{ 0 };
	vector<t_integer> cascadeStartPositions{};
	vector<t_integer> cascadeEndPositions{};
	vector<vector<t_binary>> cascadeWindows{};
	t_integer cascadeNumbitsAlice{ 0 };
	vector<t_integer> alicePreviousParityResponse{};
	vector<t_integer> totalPossibleSearchSize{};
	bool waitForFullKey{ false };
	t_integer fullKeySize{ 1024 };
	int keyIdx{ 0 };
	vector <bool> notScrambled;
	

	/*Private Methods*/
	void setMessageType(t_message msg, t_message_type mType) { msg.messageType = mType; };
	t_message_data_length getMessageDataLength(const t_message& msg);
	void setMessageDataLength(t_message msg, t_message_data_length mDataLength) { msg.messageDataLength = to_string(mDataLength); };
	t_message_data getMessageData(const t_message& msg, t_message_data_length dataLength);
	void setMessageData(t_message msg, string mData) { msg.messageData = mData; };




	bool processStoredMessages();
	bool processInMessages();
	bool ProcessDataAndBasisToStore();
	bool toggleTransmissionStatus();
	bool outputKey();
	bool startCascade();
	bool scrambleKeys();
//	bool cascadeProcessMessageToAlice(const t_message& msg, t_message_data_length dataLength, vector<t_integer> &parities, t_integer &currentCascadeWindowSet);

};


#endif
