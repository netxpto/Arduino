#include "../include/bb84_st_message_processors_common.h"


bool cascadeProcessMessage(const t_message& msg, t_message_data_length dataLength, vector<t_integer> &parities, t_integer &currentCascadeWindowSet, t_integer &passLevel) {

	bool gettingCurrentCascadeWindowSet = true;
	t_integer dividers{ 0 };
	string mDataString = msg.messageData;
	string currentWindowSet_{ "" };
	string currentPassLevel_{ "" };
	
	vector <int> mDataVector;

	for (int k = 0; k < dataLength; k++) {
		string data;
		switch (dividers) {
		case 0:
			data = mDataString.at(k);
			if (data == "-") {
				passLevel = stoi(currentPassLevel_);
				dividers++;
			}
			else {
				currentPassLevel_.append(data);
			}
			break;
		case 1:
			data = mDataString.at(k);
			if (data == "-") {
				currentCascadeWindowSet = stoi(currentWindowSet_);
				dividers++;;
			}
			else {
				currentWindowSet_.append(data);
			}
			break;
		case 2:
			data = mDataString.at(k);

			if (data == "1")
				parities.push_back((t_integer)1);
			else if (data == "0")
				parities.push_back((t_integer)0);
			else if (data == "2")
				parities.push_back((t_integer)2);
			break;
		}
	}
	return true;
}

t_integer cascadeGetParity(const vector<t_binary> cascadeWindow, const t_integer startPosition, const t_integer endPosition) {
	t_integer parity = 0;
	for (unsigned int i = startPosition; i < endPosition; i++) {
		parity += (cascadeWindow[i]);
	}
	parity = parity % 2;
	return parity;
}

bool cascadeUpdateStartAndEndPositions(const t_integer &aliceParityResponse, t_integer &startPosition, t_integer &endPosition, const t_integer windowSize, const t_integer totalSearchSize) {
	int oldStart = startPosition;
	int oldEnd = endPosition;
	int winSz = endPosition - startPosition;

	if (startPosition == 0 && endPosition == windowSize) {
		endPosition = ceil(endPosition / 2.0);
		return true;
	} else if (startPosition == endPosition) {
		return true;
	}

	if (aliceParityResponse == 2) {
		return true;
	}
	else if (aliceParityResponse == 1) {
		startPosition = endPosition;
		endPosition = ceil((startPosition + ceil((totalSearchSize - winSz)/2.0)));
		if ((endPosition < startPosition) || (endPosition>windowSize)) {
			cout << "Error!";
		}
		return true;
	}
	}
	else {
		endPosition = ceil((startPosition + endPosition) / 2.0);
		if ((endPosition < startPosition)||(endPosition > windowSize)) {
			cout << "Error!";
		}
		return true;
	}
	return false;
}

void cascadeCorrectBit(t_binary &bitValue) {
	switch (bitValue)
	{
	case 1:
		bitValue = 0;
		break;
	case 0:
		bitValue = 1;
		break;
	default:
		cout << "Stuff...";
		break;
	}
}

bool cascadeTestIfNoErrors(vector<t_integer> &bobParities) {
	bool test = true;

	for (unsigned int i = 0; i < (int)bobParities.size(); i++) {
		test = test && (bobParities[i] == 2);
		if (!test) {
			return test;
		}
	}
	return test;
}

t_message_type getMessageType(const t_message& msg) {
	if (msg.messageType[0] == BasisReconciliation) return BasisReconciliation;
	if (msg.messageType[0] == Cascade) return Cascade;
	return (t_message_type)0;
}


bool cascadeScrambleKeyWithMatrix(const t_integer M, const t_integer N, t_integer &keyIdx,  const vector<t_binary> &ORIGINAL_KEY, vector<t_binary> &scrambledKey, vector<t_integer> &bitsIdx) {
	bool alive = false;
	//bool keyAvailable = true;
	//t_integer keyIdx = 0;

	t_integer readIdx = 0;
	vector<vector <t_binary>> scrambleMatrix(M);
	for (unsigned int i = 0; i < M; i++) {
		scrambleMatrix[i].resize(N);
	}

	vector<vector <t_integer>> bitsMatrix(M);
	for (unsigned int i = 0; i < M; i++) {
		bitsMatrix[i].resize(N);
	}

	// Put key in matrix
	do {
		t_integer statusCount = 0;
		int process = ((int)ORIGINAL_KEY.size()-keyIdx);

		for (unsigned int m_idx = 0; m_idx < M; m_idx++) {
			for (unsigned int n_idx = 0; n_idx < N; n_idx++) {
				if (statusCount < process) {
					scrambleMatrix[m_idx][n_idx] = ORIGINAL_KEY[keyIdx];
					bitsMatrix[m_idx][n_idx] = keyIdx;
						statusCount++;
						keyIdx++;
				}
				else {
					break;
				}
			}
			if (statusCount >= process) {
				break;
			}
		}

		statusCount = 0;
		// Retrieve scrambled key
		for (unsigned int n_idx2 = 0; n_idx2 < N; n_idx2++) {
			for (unsigned int m_idx2 = 0; m_idx2 < M; m_idx2++) {
				if (statusCount < process) {
					scrambledKey.push_back(scrambleMatrix[m_idx2][n_idx2]);
					bitsIdx.push_back(bitsMatrix[m_idx2][n_idx2]);
					statusCount++;
					readIdx++;
				}
				else {
					break;
				}
			}
			if (statusCount >= process) {
				break;
			}
		}

	} while (keyIdx < (int)ORIGINAL_KEY.size());


	return alive;
}


/*
bool cascadeScrambleKeyWithMatrix(const t_integer M, const t_integer N, t_integer& keyIdx, const vector<t_binary>& ORIGINAL_KEY, vector<t_binary>& scrambledKey, vector<t_integer>& bitsIdx) {
	bool alive = false;
	//bool keyAvailable = true;
	//t_integer keyIdx = 0;

	while (keyIdx < ORIGINAL_KEY.size()) {
		scrambledKey.push_back(ORIGINAL_KEY[keyIdx]);
		keyIdx++;
	}

	return true;

}*/