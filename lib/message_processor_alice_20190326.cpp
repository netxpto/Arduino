#include "../include/message_processor_alice_20190326.h"

void MessageProcessorAlice::initialize(void) {

	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplesPerSymbol(inputSignals[0]->getSamplesPerSymbol());
//	outputSignals[0]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	outputSignals[1]->setSamplingPeriod(0.001);
	outputSignals[1]->setSymbolPeriod(0.001);
	outputSignals[1]->setSamplesPerSymbol(inputSignals[1]->getSamplesPerSymbol());

	for (unsigned int i = 0; i <= numPasses ; i++) {
		vector<t_binary> key;
		keys.push_back(key);
	}

	for (unsigned int i = 0; i <= numPasses ; i++) {
		vector<t_binary> key;
		scrambledKeys.push_back(key);
	}

	for (unsigned int i = 0; i <= numPasses; i++) {
		vector<t_integer> key;
		scrambledKeysIdx.push_back(key);
	}
}


bool MessageProcessorAlice::runBlock(void) {

	bool process{ false }, alive{ false };
	int isEmptyInputMsg = inputSignals[0]->getBufferEmpty();
	int isEmptyOutputMsg = outputSignals[0]->getBufferEmpty();

	do {
		process = ProcessDataAndBasisToStore();
		alive = process;
		process = toggleTransmissionStatus();
		alive = alive || process;
		process = processStoredMessages();
		alive = alive || process;
		process = processInMessages();
		alive = alive || process;
		process = processStoredMessages();
		alive = alive || process;
		alive = startCascade() || alive;
		alive = outputKey() || alive;
	} while (alive);


	if ((keys[numPasses].empty() && cascadeNumbitsAlice<=0) || (!isEmptyInputMsg && !isEmptyOutputMsg) || !keys[numPasses].empty()) {
		alive = true;
	}

	return alive;
}

bool MessageProcessorAlice::startCascade() {
	bool alive{ false };
	int process = 0;
	int scrambledKeySize{ 0 };
	int space = outputSignals[0]->space();  // Messages Out
	t_integer passLevel; 
	t_integer m{ 0 };
	t_integer n{ 0 };

	if (cascadeActive || space <= 0) return false;

	//for (unsigned int i = 0; i < numPasses; i++) {
	for (unsigned int i = 0; i < numPasses; i++) {
		if ((i >= 1)) {
			cout << "Review time.";
		}
		if (!waitForFullKey) {
			passLevel = i + 1;
			if (passLevel == 1) {
				process = min((int)floor((int)keys[i].size() / (cascadeWindowSize)), cascadeMaxWindows);
				if (process > 0) {
					break;
					//cascadeWindowSize = startingCascadeWindowSize;
				}
			} else {
					if (passLevel > 1 
						&& scrambledKeys[passLevel].empty() 
						&& !(keys[passLevel-1].size()<cascadeWindowSize))
					{

//						cascadeWindowSize = cascadeWindowSize * passLevel;
						m = cascadeWindowSize;
						n = m + 1;
						keyIdx = 0;
						alive = cascadeScrambleKeyWithMatrix(m, n, keyIdx, keys[passLevel - 1], scrambledKeys[passLevel], scrambledKeysIdx[passLevel]);
						keys[passLevel - 1] = {};
						if (passLevel % 1 == 0) {
							reverse(scrambledKeys[passLevel].begin(), scrambledKeys[passLevel].end());
							reverse(scrambledKeysIdx[passLevel].begin(), scrambledKeysIdx[passLevel].end());
						}
						process = min((int)floor((int)scrambledKeys[passLevel].size() / (cascadeWindowSize)), cascadeMaxWindows);
					}
					else if (passLevel > 1) {
						process = min((int)floor((int)scrambledKeys[passLevel].size() / (cascadeWindowSize)), cascadeMaxWindows);
					}
					if (process > 0) break;
				
			}
		}
		else if (keys[i].size() >= fullKeySize) {
			process = (floor(fullKeySize / cascadeWindowSize));
			passLevel = i + 1;
		}
	} 

	if (process <= 0) return alive;

	cascadeActive = true;
	cascadeStartPositions={};
	cascadeEndPositions={};
	cascadeWindows = {};
	currentCascadeWindowSet++;

	string mData{ "" };
	mData.append(to_string(passLevel));
	mData.append("-");
	mData.append(to_string(currentCascadeWindowSet));
	mData.append("-");

	for (unsigned int w = 0; w < process; w++) {
		int parity = 0;
		if (passLevel == 1) {
			for (unsigned int i = 0; i < (cascadeWindowSize ); i++) {

				parity += (keys[passLevel - 1][i]);
			}
		} else {
			for (unsigned int i = 0; i < (cascadeWindowSize ); i++) {

				parity += (scrambledKeys[passLevel][i]);
			}
		}
		parity = parity % 2;

		mData.append(to_string(parity));

		cascadeStartPositions.push_back(0);
		cascadeEndPositions.push_back(ceil(cascadeWindowSize / 2));

		if (passLevel == 1) {
			vector<t_binary> cascadeWindow(keys[passLevel-1].begin(), keys[passLevel-1].begin() + (cascadeWindowSize ));
			cascadeWindows.push_back(cascadeWindow);
			totalPossibleSearchSize.push_back((cascadeWindowSize ));
			keys[passLevel-1].erase(keys[passLevel-1].begin(), keys[passLevel-1].begin() + (cascadeWindowSize ));
		} else {
			vector<t_binary> cascadeWindow(scrambledKeys[passLevel].begin(), scrambledKeys[passLevel].begin() + (cascadeWindowSize ));
			cascadeWindows.push_back(cascadeWindow);
			totalPossibleSearchSize.push_back((cascadeWindowSize ));
			scrambledKeys[passLevel].erase(scrambledKeys[passLevel].begin(), scrambledKeys[passLevel].begin() + (cascadeWindowSize ));
		}
	}

	t_message messageOut;
	messageOut.messageType = Cascade;
	messageOut.messageDataLength = to_string((int)mData.size());
	messageOut.messageData = mData;

	if (mData.size() != 0)
	{
		outputSignals[0]->bufferPut((t_message)messageOut); // Messages Out - Sending BR
	}
	alive = true;

	return alive;
}


bool MessageProcessorAlice::ProcessDataAndBasisToStore() {
	bool alive{ false };

	int ready1 = inputSignals[1]->ready(); // Basis In
	int ready2 = inputSignals[2]->ready(); // Basis In
	int ready = min(ready1, ready2);

	int space = maxOfStoredBasis - numberOfStoredBasis;

	int process = min(ready, space);

	if (process > 0) {
		for (auto k = 0; k < process; k++) {
			t_binary dataIn;
			t_binary basisIn;
			inputSignals[2]->bufferGet(&basisIn); // Basis In
			inputSignals[1]->bufferGet(&dataIn); // Data In

			storedBasis.push_back((int)basisIn);
			storedData.push_back((int)dataIn);
			numberOfStoredBasis++;
			alive = true;
		}
	}
	return alive;
}

bool MessageProcessorAlice::processStoredMessages() {

	bool alive{ false };
	/*
		int ready1 = inputSignals[1]->ready(); // Basis In
		int ready2 = inputSignals[2]->ready(); // Basis In
		int ready = min(ready1, ready2);
	*/

//	int ready = numberOfStoredBasis;
	int ready = numberOfStoredMessages;
	int readyBasis = numberOfStoredBasis;

	int space3 = outputSignals[3]->space();	// Sifted key out
	int space1 = outputSignals[1]->space(); // Keys Out
	int space0 = outputSignals[0]->space(); // Messages out
	int space = min(space0, space1);

	int process = min(space, ready);

	if (process <= 0) return alive;

	for (auto n = 0; n < numberOfStoredMessages; n++) {

		t_message_type mType = getMessageType(storedMessages[n]);
		t_message_data_length mDataLength = getMessageDataLength(storedMessages[n]);
		t_message_data mData;
		t_message messageOut;
		int readyMessage;
		int dLength;

		string mDataOut{ "" };
		int processMessage{ 0 };
		switch (mType) {

		case BasisReconciliation:
			
			mData = getMessageData(storedMessages[n], mDataLength);
			readyMessage = min(readyBasis, (int)mData.size());
			processMessage = min(readyMessage, space3);

			if (processMessage < 0) return alive;

			for (auto k = 0; k < processMessage; k++) {
				/*					t_binary basisIn;
									t_binary dataIn;
									inputSignals[2]->bufferGet(&basisIn); // Basis in
									inputSignals[1]->bufferGet(&dataIn); // Data in
									*/
				alive = true;

				if (storedBasis[k] == mData[k]) {
					numBits++;
					outputSignals[3]->bufferPut((t_binary)storedData[k]); // Key out
					mDataOut.append(to_string((int)1));
					keys[0].push_back((t_binary)storedData[k]);
				}
				else {
					mDataOut.append(to_string((int)0));
				}
			}

			//	int dLength = mDataLength - processMessage;
			mData.erase(mData.begin(), mData.begin() + processMessage);
			dLength = (int)mData.size();
			storedData.erase(storedData.begin(), storedData.begin() + processMessage);
			storedBasis.erase(storedBasis.begin(), storedBasis.begin() + processMessage);
			numberOfStoredBasis = (int)storedBasis.size();
			if (dLength == 0) {
				storedMessages.erase(storedMessages.begin(), storedMessages.begin() + n + 1);
				numberOfStoredMessages = (int)storedMessages.size();
			}
			else {
				string mDataUpdated{ "" };
				for (unsigned int m = 0; m < mData.size(); m++) {
					mDataUpdated.append(to_string(mData[m]));
				}
				storedMessages[n].messageData = mDataUpdated;
				storedMessages[n].messageDataLength = to_string(storedMessages[n].messageData.size());
			}

			messageOut.messageType = mType;
			messageOut.messageDataLength = to_string((int)mDataOut.size());
			messageOut.messageData = mDataOut;

			if (mDataOut.size() != 0) {
				outputSignals[0]->bufferPut((t_message)messageOut); // Messages Out - Sending BR
			} else;
			
			break;
		case Cascade:
			vector<t_integer> bobParities;
			vector<t_integer> aliceParities;
			vector<t_integer> aliceParitiesResponse;
			t_integer receivedCascadeWindowSet;
			string mDataOut = "";
			t_integer passLevel;

			alive = cascadeProcessMessage(storedMessages[n], mDataLength, bobParities, receivedCascadeWindowSet, passLevel);
			
			if (cascadeTestIfNoErrors(bobParities)) {
				for (unsigned int w = 0; w < (int)bobParities.size(); w++) {
					keys[passLevel].insert(keys[passLevel].end(), cascadeWindows[w].begin(), cascadeWindows[w].end());
					cascadeActive = false;
				}

				totalPossibleSearchSize = {};
			} else {
				mDataOut.append(to_string(passLevel));
				mDataOut.append("-");
				mDataOut.append(to_string(receivedCascadeWindowSet));
				mDataOut.append("-");
				for (unsigned int w = 0; w < (int)cascadeWindows.size(); w++) {
					int oldSz = cascadeEndPositions[w] - cascadeStartPositions[w];
					if (bobParities[w] == 2) {
						aliceParities.push_back(2);
						aliceParitiesResponse.push_back(2);
						totalPossibleSearchSize[w] = 1;
					} else {
						aliceParities.push_back(cascadeGetParity(cascadeWindows[w], cascadeStartPositions[w], cascadeEndPositions[w]));
						if (aliceParities[w] == bobParities[w]) {
							aliceParitiesResponse.push_back(1);
						} else {
							aliceParitiesResponse.push_back(0);
						}
						
						cascadeUpdateStartAndEndPositions(aliceParitiesResponse[w], cascadeStartPositions[w], cascadeEndPositions[w], (cascadeWindowSize* passLevel), totalPossibleSearchSize[w]);
						if (aliceParities[w] == bobParities[w]) {

							totalPossibleSearchSize[w] = totalPossibleSearchSize[w] - oldSz;
						} else {

							totalPossibleSearchSize[w] = oldSz;
						}
							
					}
					mDataOut.append(to_string(aliceParitiesResponse[w]));
				}
				messageOut.messageType = Cascade;
				messageOut.messageDataLength = to_string((int)mDataOut.size());
				messageOut.messageData = mDataOut;

				if (mDataOut.size() != 0)
				{
					outputSignals[0]->bufferPut((t_message)messageOut); // Messages Out - Sending BR
				} else;
			}
			storedMessages.erase(storedMessages.begin(), storedMessages.begin() + n + 1);
			numberOfStoredMessages = (int)storedMessages.size();
			break;
		}
	}
	return alive;
}


bool MessageProcessorAlice::processInMessages() {

	bool alive{ false };

	int ready = inputSignals[0]->ready(); // Messages In
	if (ready > 0) {
		if (numberOfStoredMessages < maxNumberOfStoredMessages) {
			t_message msgIn;
			inputSignals[0]->bufferGet(&msgIn); // Messages In
			storedMessages.push_back(msgIn);
			numberOfStoredMessages++;
			alive = true;
		}
	}

	return alive;

}

t_message_data_length MessageProcessorAlice::getMessageDataLength(const t_message& msg) {
	if ((msg.messageDataLength).size() != 0)
		return stoi(msg.messageDataLength);
	else
		return 0;
}


t_message_data MessageProcessorAlice::getMessageData(const t_message& msg, t_message_data_length dataLength) {

	string mDataString = msg.messageData;

	vector <int> mDataVector;

	for (int k = 0; k < dataLength; k++) {
		char data = mDataString.at(k);
		
		if(data == '1')
			mDataVector.push_back(1);
		else if (data == '0')
			mDataVector.push_back(0);
		else if (data == '-') {
			char aux_data = mDataString.at(k + 1);
			if (aux_data == '2') {
				mDataVector.push_back(-2);
				k++;
			}
			else if (aux_data == '1') {
				mDataVector.push_back(-1);
				k++;
			}
		}
	}

	return mDataVector;
}

bool MessageProcessorAlice::toggleTransmissionStatus() {
	bool alive{ false };

	int space = outputSignals[2]->space();

	int process = space;

	if (process > 0) {
		for (auto k = 0; k < process; k++) {
			outputSignals[2]->bufferPut((t_real)transmissionStatus);
			alive = true;
		}
	}
	return alive;
}

bool MessageProcessorAlice::outputKey() {
	bool alive{ false };
	//vector<t_binary> *key = &keys[0];
	vector<t_binary> *key = (&keys[numPasses]);

	int space = outputSignals[1]->space();
	int ready = key->size();
	int process = min(ready, space);

	if (process <= 0) return alive;

	for (unsigned int i = 0; i < process; i++) {
		outputSignals[1]->bufferPut((*key)[i]);
		cascadeNumbitsAlice++;
	}
	key->erase(key->begin(), key->begin() + process);
	alive = true;

	return alive;
}


// Cascade functions
/*
bool MessageProcessorAlice::cascadeProcessMessageToAlice(const t_message& msg, t_message_data_length dataLength, vector<t_integer> &parities, t_integer &currentCascadeWindowSet) {

	bool gettingCurrentCascadeWindowSet = true;

	string mDataString = msg.messageData;
	string currentWindowSet_{ "" };

	vector <int> mDataVector;

	for (int k = 0; k < dataLength; k++) {
		switch (gettingCurrentCascadeWindowSet) {
		case true:
			char data = mDataString.at(k);
			if (data == '-') {
				currentCascadeWindowSet = stoi(currentWindowSet_);
				gettingCurrentCascadeWindowSet = false;
			}
			else {
				currentWindowSet_.append(to_string(data));
			}
			break;
		case false:
			char data = mDataString.at(k);

			if (data == '1')
				parities.push_back((t_integer)1);
			else if (data == '0')
				parities.push_back((t_integer)0);
			else if (data == '2')
				parities.push_back((t_integer)2);
			break;
		}
	}
	return true;
}
*/