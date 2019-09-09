#ifndef BB84_ST_MESSAGE_PROCESSORS_COMMON_H
#define BB84_ST_MESSAGE_PROCESSORS_COMMON_H

#include <vector>
#include "netxpto_20180815.h"

typedef enum { BasisReconciliation, Cascade } t_message_type;
typedef int t_message_data_length;
typedef vector<int> t_message_data;

bool cascadeProcessMessage(const t_message& msg, t_message_data_length dataLength, vector<t_integer> &parities, t_integer &currentCascadeWindowSet, t_integer &passLevel);
t_integer cascadeGetParity(const vector<t_binary> cascadeWindow, const t_integer startPosition, const t_integer endPosition);
bool cascadeUpdateStartAndEndPositions(const t_integer &aliceParityResponse, t_integer &startPosition, t_integer &endPosition, const t_integer windowSize, const t_integer totalPossibleSearchSize);
void cascadeCorrectBit(t_binary &bitValue);
bool cascadeTestIfNoErrors(vector<t_integer> &bobParities);
t_message_type getMessageType(const t_message& msg);
bool cascadeScrambleKeyWithMatrix(const t_integer M, const t_integer N, t_integer &keyIdx, const vector<t_binary> &ORIGINAL_KEY, vector<t_binary> &scrambledKey, vector<t_integer>& bitsIdx);

#endif