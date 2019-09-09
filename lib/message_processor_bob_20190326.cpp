#include "../include/message_processor_bob_20190326.h"

void MessageProcessorBob::initialize(void) {

	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[0]->setSamplesPerSymbol(1);
//	outputSignals[0]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	outputSignals[1]->setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	outputSignals[1]->setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
//	outputSignals[1]->setSamplesPerSymbol(inputSignals[0]->getSamplesPerSymbol());
//	outputSignals[1]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	for (unsigned int i = 0; i <= numPasses + 1; i++) {
		vector<t_binary> key;
		keys.push_back(key);
	}

	for (unsigned int i = 0; i <= numPasses; i++) {
		vector<t_binary> key;
		scrambledKeys.push_back(key);
	}

	for (unsigned int i = 0; i <= numPasses; i++) {
		vector<t_integer> key;
		scrambledKeysIdx.push_back(key);
	}
}

bool MessageProcessorBob::runBlock(void) {
	bool alive{ false };
	int isEmptyInputMsg = inputSignals[0]->getBufferEmpty();
	int isEmptyOutputMsg = outputSignals[0]->getBufferEmpty();

	do {
		alive = transmissionControl();
		alive = ProcessDataAndBasisToStore() || alive;
		alive = ProcessBasisReconcilliationToSend() || alive;
		alive = ProcessStoredMessage() || alive;
		alive = ProcessReceivedMessage() || alive;
		alive = ProcessStoredMessage() || alive;
		alive = ProcessBasisReconcilliationToSend() || alive;
		alive = outputKey() || alive ;
	} while (alive);
	
	if (!isEmptyInputMsg || !isEmptyOutputMsg) {
		alive = true;
	}

	return alive;
}

bool MessageProcessorBob::ProcessDataAndBasisToStore() {
	bool alive{ false };

	int ready1 = inputSignals[1]->ready(); // Basis In
	int ready2 = inputSignals[2]->ready(); // Basis In
	
	int ready = min(ready1, ready2);

	int space = maxOfStoredBasis - numberOfStoredBasis;

	int process = min(ready, space);

	if (process > 0) {
		for (auto k = 0; k < process; k++) {
			t_real dataIn;
			t_real basisIn;
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

bool MessageProcessorBob::ProcessBasisReconcilliationToSend() {
	bool alive{ false };

	int space = outputSignals[0]->space();  // Messages Out

	if (space <= 0) return alive;

//	int readyData = inputSignals[1]->ready();    // Data In
//	int readyBasis = inputSignals[2]->ready();   // Basis In
//	int ready = min(readyData, readyBasis);
	int ready = numberOfStoredBasis;
	int processMessage = min(ready, maxMessageDataLength);

	if (processMessage <= 0) return alive;
	
	string mData{ "" };
	vector<int> currData{};
	vector<int> currBasis{};
	for (auto l = 0; l < processMessage; l++) {
		t_real basisBob;
//		t_binary basisBob;
		t_real dataBob;
		int basisBobInt;

//		inputSignals[1]->bufferGet(&dataBob); // Data In
//		inputSignals[2]->bufferGet(&basisBob); // Basis In
		dataBob = storedData[l];
		basisBob = storedBasis[l];
		
		basisBobInt = (int)basisBob;
		if (dataBob >= 0) {
			mData.append(to_string(basisBobInt));
		} else {
			t_integer dataBobInt = (t_integer)dataBob;
			mData.append(to_string((int)dataBob));
		}
		//currData.push_back((int)dataBob);
		//currBasis.push_back((int)round(basisBob));
		storedReceivedData.push_back((int)dataBob);
		storedReceivedBasis.push_back((int)basisBob);
	}

	t_message messageToSend;
	messageToSend.messageData = mData;
	messageToSend.messageDataLength = to_string((t_message_data_length)mData.size());
	messageToSend.messageType = BasisReconciliation;

	outputSignals[0]->bufferPut((t_message)messageToSend); // Messages out
	//storedBasisFrames.push_back(currBasis);
	//storedDataFrames.push_back(currData);
	numberOfFrames++;

	storedData.erase(storedData.begin(), storedData.begin() + processMessage);
	storedBasis.erase(storedBasis.begin(), storedBasis.begin() + processMessage);
	numberOfStoredBasis = (int) storedBasis.size();

	alive = true;
	
	
	return alive;
}

bool MessageProcessorBob::ProcessReceivedMessage() {

	bool alive{ false };

	int ready = inputSignals[0]->ready(); // Messages in
	if (ready <= 0) return alive;
	else {
		if (numberOfStoredMessages < maxNumberOfStoredMessages) {
			t_message mIn;
			inputSignals[0]->bufferGet(&mIn); // Messages In
			storedMessages.push_back(mIn);
			numberOfStoredMessages++;
			alive = true;
		}
	}
	return alive;
}

bool MessageProcessorBob::ProcessStoredMessage() {

	bool alive{ false };
	int dLength;

	int space1 = outputSignals[1]->space(); // Key Out
	int space2 = outputSignals[2]->space(); // Sifted Key out
	int space = min(space1, space2);
	int msgSpace = outputSignals[0]->space(); // Msg out

	if (msgSpace <= 0) return alive;

	if (space > 0) {
		for (auto n = 0; n < numberOfStoredMessages; n++) {

			t_message_type mType = getMessageType(storedMessages[n]);
			t_message_data_length mDataLength = getMessageDataLength(storedMessages[n]);
			t_message_data mData;

			string mDataOut{ "" };
			int process{ 0 };
			switch (mType) {

			case BasisReconciliation:
				mData = getMessageData(storedMessages[n], mDataLength);
				process = min(space2, min((int)mData.size(), (int)storedReceivedData.size()));
				
				for (auto k = 0; k < process; k++) {

					alive = true;
					if (1 == mData[k]) {
						if ((t_binary)storedReceivedData[k] > 2) {
							t_integer data = storedReceivedData[k];
							t_integer basis = storedReceivedBasis[k];
							cout << "Error!";
						}
						outputSignals[2]->bufferPut((t_binary)storedReceivedData[k]); // Key out
						keys[0].push_back((t_binary)storedReceivedData[k]);
						//siftedKey.push_back((t_binary)storedReceivedData[k]);
						numBits++;
					}
				}

				dLength = mDataLength - process;
				mData.erase(mData.begin(), mData.begin() + process);
				storedReceivedData.erase(storedReceivedData.begin(), storedReceivedData.begin() + process);
				storedReceivedBasis.erase(storedReceivedBasis.begin(), storedReceivedBasis.begin() + process);
				//			storedDataFrames[0].erase(storedDataFrames[0].begin(), storedDataFrames[0].begin() + process);
				if (dLength == 0) {
					storedMessages.erase(storedMessages.begin() + n);
					numberOfStoredMessages = (int)storedMessages.size();
					//				storedDataFrames.erase(storedDataFrames.begin()+n);
					//				numberOfFrames = (int)storedDataFrames.size();
				}
				else {
					storedMessages[n].messageDataLength = to_string(dLength);
					string mDataUpdated{ "" };
					for (unsigned int m = 0; m < mData.size(); m++) {
						mDataUpdated.append(to_string(mData[m]));
					}
					storedMessages[n].messageData = mDataUpdated;
				}
				break;
			case Cascade:
				vector<t_integer> aliceParitiesResponse{};
				vector<t_integer> bobParities{};
				t_integer receivedCascadeWindowSet;
				t_integer passLevel;

				// Receive message
				alive = cascadeProcessMessage(storedMessages[n], mDataLength, aliceParitiesResponse, receivedCascadeWindowSet, passLevel);
				string mDataOut = "";
				mDataOut.append(to_string(passLevel));
				mDataOut.append("-");
				mDataOut.append(to_string(receivedCascadeWindowSet));
				mDataOut.append("-");

				if (receivedCascadeWindowSet > currentCascadeWindowSet) {
					// TODO
					// delete variables from last cascade run
					cascadeWindows.erase(cascadeWindows.begin(), cascadeWindows.end());
					cascadeStartPositions = {};
					cascadeEndPositions = {};
					currentCascadeWindowSet = receivedCascadeWindowSet;
					totalPossibleSearchSize = {};
					// And the rest
				}
				if (passLevel == 1){
					if (cascadeWindows.empty()) {
						// The first signal sent by Alice is different. It contains the initial parities, not if the
						// response to bob's parities

						// Initialize variables and calculate starting parities

						for (unsigned int w = 0; w < (int)aliceParitiesResponse.size(); w++) {
						
							vector<t_binary> cascadeWindow(keys[passLevel-1].begin(), keys[passLevel - 1].begin() + cascadeWindowSize);
							cascadeWindows.push_back(cascadeWindow);
							keys[passLevel - 1].erase(keys[passLevel - 1].begin(), keys[passLevel - 1].begin() + cascadeWindowSize);
							cascadeStartPositions.push_back(0);
							cascadeEndPositions.push_back(cascadeWindowSize);
						
							int parity = 0;
						
							parity = cascadeGetParity(cascadeWindows[w], cascadeStartPositions[w], cascadeEndPositions[w]);
							bobParities.push_back(parity);

							// Divide the windows
							if (aliceParitiesResponse[w] == bobParities[w]) {
								cascadeStartPositions[w] = 0;
								cascadeEndPositions[w] = 0;
								bobParities[w] = 2;
								totalPossibleSearchSize.push_back(1);
							}
							else {
								cascadeEndPositions[w] = ceil(cascadeWindowSize / 2);
								totalPossibleSearchSize.push_back(cascadeWindowSize);
								// Calculate new parities to send
								bobParities[w] = cascadeGetParity(cascadeWindows[w], cascadeStartPositions[w], cascadeEndPositions[w]);
							}
							mDataOut.append(to_string(bobParities[w]));
						}
					} else {
						for (unsigned int w = 0; w < (int)aliceParitiesResponse.size(); w++) {
							if (aliceParitiesResponse[w] == 2) {
								bobParities.push_back(2);
								totalPossibleSearchSize[w] = 1;
							} else {
								int oldSz = cascadeEndPositions[w] - cascadeStartPositions[w];
								if (oldSz == 1 && totalPossibleSearchSize[w] == 2) {
									if (aliceParitiesResponse[w] == 1) {
										cascadeCorrectBit(cascadeWindows[w][cascadeStartPositions[w] + (t_integer)1]);
										corrected1++;
									}
									else if (aliceParitiesResponse[w] == 0) {
										cascadeCorrectBit(cascadeWindows[w][cascadeStartPositions[w]]);
										corrected2++;
									}
									totalPossibleSearchSize[w] = 1;
									bobParities.push_back(2);
	//							} else if (cascadeEndPositions[w] - cascadeStartPositions[w] == 0) {
								} else {
									bool status = cascadeUpdateStartAndEndPositions(aliceParitiesResponse[w], cascadeStartPositions[w], cascadeEndPositions[w], cascadeWindowSize, totalPossibleSearchSize[w]);
									if (aliceParitiesResponse[w] == 1) {
										totalPossibleSearchSize[w] = totalPossibleSearchSize[w] - oldSz;
									}
									else {
										totalPossibleSearchSize[w] = oldSz;
									}
									if (totalPossibleSearchSize[w] == 1) {
										cascadeCorrectBit(cascadeWindows[w][cascadeStartPositions[w]]);
										corrected3++;
										bobParities.push_back(2);
									} else {
										bobParities.push_back(cascadeGetParity(cascadeWindows[w], cascadeStartPositions[w], cascadeEndPositions[w]));
									}
								}
							}
							mDataOut.append(to_string(bobParities[w]));
						}
					}
				} else {
					if (cascadeWindows.empty()) {
						// The first signal sent by Alice is different. It contains the initial parities, not if the
						// response to bob's parities
						t_integer m;
						t_integer n;
						if (scrambledKeys[passLevel].empty()) {
							//cascadeWindowSize = cascadeWindowSize * passLevel;
							m = cascadeWindowSize;
							n = m + 1;
							keyIdx = 0;
							alive = cascadeScrambleKeyWithMatrix(m, n, keyIdx, keys[passLevel - 1], scrambledKeys[passLevel], scrambledKeysIdx[passLevel]);
							keys[passLevel - 1] = {};
							if (passLevel % 1 == 0) {
								reverse(scrambledKeys[passLevel].begin(), scrambledKeys[passLevel].end());
								reverse(scrambledKeysIdx[passLevel].begin(), scrambledKeysIdx[passLevel].end());
							}
						}
						
						// Initialize variables and calculate starting parities

						for (unsigned int w = 0; w < (int)aliceParitiesResponse.size(); w++) {

							vector<t_binary> cascadeWindow(scrambledKeys[passLevel].begin(), scrambledKeys[passLevel].begin() + cascadeWindowSize );
							cascadeWindows.push_back(cascadeWindow);
							scrambledKeys[passLevel].erase(scrambledKeys[passLevel].begin(), scrambledKeys[passLevel].begin() + cascadeWindowSize );
							cascadeStartPositions.push_back(0);
							cascadeEndPositions.push_back(cascadeWindowSize);

							int parity = 0;

							parity = cascadeGetParity(cascadeWindows[w], cascadeStartPositions[w], cascadeEndPositions[w]);
							bobParities.push_back(parity);

							// Divide the windows
							if (aliceParitiesResponse[w] == bobParities[w]) {
								cascadeStartPositions[w] = 0;
								cascadeEndPositions[w] = 0;
								bobParities[w] = 2;
								totalPossibleSearchSize.push_back(1);
							}
							else {
								//							cascadeStartPositions[w] = 0;
								cascadeEndPositions[w] = ceil(cascadeWindowSize / 2);
								totalPossibleSearchSize.push_back(cascadeWindowSize);
								// Calculate new parities to send
								bobParities[w] = cascadeGetParity(cascadeWindows[w], cascadeStartPositions[w], cascadeEndPositions[w]);
							}
							mDataOut.append(to_string(bobParities[w]));
						}
					}
					else {
						for (unsigned int w = 0; w < (int)aliceParitiesResponse.size(); w++) {
							if (aliceParitiesResponse[w] == 2) {
								bobParities.push_back(2);
								totalPossibleSearchSize[w] = 1;
							}
							else {
								int oldSz = cascadeEndPositions[w] - cascadeStartPositions[w];
								if (oldSz == 1 && totalPossibleSearchSize[w] == 2) {
									if (aliceParitiesResponse[w] == 1) {
										cascadeCorrectBit(cascadeWindows[w][cascadeStartPositions[w] + (t_integer)1]);
										corrected1++;
									}
									else if (aliceParitiesResponse[w] == 0) {
										cascadeCorrectBit(cascadeWindows[w][cascadeStartPositions[w]]);
										corrected2++;
									}
									totalPossibleSearchSize[w] = 1;
									bobParities.push_back(2);
									//							} else if (cascadeEndPositions[w] - cascadeStartPositions[w] == 0) {
								}
								else {
									bool status = cascadeUpdateStartAndEndPositions(aliceParitiesResponse[w], cascadeStartPositions[w], cascadeEndPositions[w], cascadeWindowSize, totalPossibleSearchSize[w]);
									if (aliceParitiesResponse[w] == 1) {
										totalPossibleSearchSize[w] = totalPossibleSearchSize[w] - oldSz;
									}
									else {
										totalPossibleSearchSize[w] = oldSz;
									}
									if (totalPossibleSearchSize[w] == 1) {
										cascadeCorrectBit(cascadeWindows[w][cascadeStartPositions[w]]);
										corrected3++;
										bobParities.push_back(2);
									}
									else {
										bobParities.push_back(cascadeGetParity(cascadeWindows[w], cascadeStartPositions[w], cascadeEndPositions[w]));
									}
								}
							}
							mDataOut.append(to_string(bobParities[w]));
						}
					}
					// Implement the rest here
				}




				if (cascadeTestIfNoErrors(bobParities)) {
					for (unsigned int w = 0; w < (int)bobParities.size(); w++) {
						keys[passLevel].insert(keys[passLevel].end(), cascadeWindows[w].begin(), cascadeWindows[w].end());
					}
					cout << "Sent.";
				}

				t_message messageOut;
				messageOut.messageType = Cascade;
				messageOut.messageDataLength = to_string((int)mDataOut.size());
				messageOut.messageData = mDataOut;
				storedMessages.erase(storedMessages.begin() + n);
				numberOfStoredMessages = (int)storedMessages.size();
				if (mDataOut.size() != 0)
				{
					outputSignals[0]->bufferPut((t_message)messageOut); // Messages Out
				}
				break;
			}

		}
	}
	
	return alive;
}

t_message_data MessageProcessorBob::getMessageData(const t_message& msg, t_message_data_length dataLength) {

	string mDataString = msg.messageData;

	vector <int> mDataVector;

	for (int k = 0; k < dataLength; k++) {
		char data = mDataString.at(k);

		if (data == '1')
			mDataVector.push_back(1);
		else if (data == '0')
			mDataVector.push_back(0);
	}

	return mDataVector;
}

t_message_data_length MessageProcessorBob::getMessageDataLength(const t_message& msg) {

	if ((msg.messageDataLength).size() >= 0)
		return stoi(msg.messageDataLength);
	else
		return 0;
}

// Transmission control Functions
bool MessageProcessorBob::transmissionControl() {
	bool alive{ false };

	int ready = inputSignals[3]->ready();

	int process = ready;

	t_real currentStatus;
	if (process > 0) {
		for (auto k = 0; k < process; k++) {
			inputSignals[3]->bufferGet(&currentStatus);
			alive = true;
		}
		transmissionStatus = (int)currentStatus;
	}
	return alive;
}

/*
// Cascade functions

bool MessageProcessorBob::cascadeProcessMessageToBob(const t_message& msg, t_message_data_length dataLength, vector<t_integer> &parities, t_integer &currentCascadeWindowSet) {

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
			} else {
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

t_integer MessageProcessorBob::cascadeGetParity(const vector<t_binary> cascadeWindow, const t_integer startPosition, const t_integer endPosition){
	t_integer parity = 0;
	for (unsigned int i = startPosition; i < endPosition; i++) {
		parity += (cascadeWindow[i]);
	}
	parity = parity % 2;
	return parity;
}

bool MessageProcessorBob::cascadeUpdateStartAndEndPositions(const t_integer &aliceParityResponse, t_integer &startPosition, t_integer &endPosition) {
	if (aliceParityResponse == 2 ) {
		return true;
	} else if (aliceParityResponse == 1) {
		int winSz = endPosition - startPosition;
		startPosition = endPosition;
		endPosition = ceil((startPosition + winSz) / 2);
		return true;
	} else {
		endPosition = ceil((startPosition + endPosition) / 2);
		return true;
	}
	return false;
}

void MessageProcessorBob::cascadeCorrectBit(t_binary &bitValue) {
	switch (bitValue)
	{
	case 1:
		bitValue = 0;
	case 0:
		bitValue = 1;
	default:
		break;
	}
}

bool MessageProcessorBob::cascadeTestIfNoErrors(vector<t_integer> &bobParities) {
	bool test = true;

	for (unsigned int i = 0; i < (int)bobParities.size(); i++) {
		test = test && (bobParities[i]==2);
		if (!test) {
			return test;
		}
	}
	return test;
}
*/

bool MessageProcessorBob::outputKey() {
	bool alive{ false };
//	vector<t_binary> *key = &keys[0];
	vector<t_binary> *key = &keys[numPasses];

	int space = outputSignals[1]->space();
	int ready = key->size();
	int process = min(ready, space);
	
	if (process <= 0) return alive;

	for (unsigned int i = 0; i < process; i++) {
		outputSignals[1]->bufferPut((*key)[i]);
		cascadeNumbitsBob++;
	}
	key->erase(key->begin(), key->begin() + process);
	alive = true;

	return alive;
}
