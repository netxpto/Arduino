#ifndef PRIVACY_AMPLIFICATION_H_
#define PRIVACY_AMPLIFICATION_H_

#include "../../../include/netxpto_20180815.h"
enum class privacy_amplification_role{SeedTransmitter, SeedReceiver, Undefined};

class PrivacyAmplification : public Block {
public:
	PrivacyAmplification(initializer_list<Signal*> InputSig, initializer_list<Signal*> OutputSig) : Block(InputSig, OutputSig) {};
	void initialize(void);
	bool runBlock(void);

	void setRole(privacy_amplification_role role) { paRole = role; }
	void setBER(t_real berValue) { ber = berValue; }
	void setSystematicSecurityParameter(t_integer sp) { securityParameter = sp; }
	void setBypassHashing(bool mrb) { bypassHashing= mrb; }

	long int getInputBits(void) { return inputBits; }
	long int getOutputBits(void) { return outputBits; }

private:
	// State variables
	vector<t_binary> vectorHashedBits{};
	vector<t_binary> vectorBitsToHash{};
	vector<t_binary> bitsOnHold{};
	long int inputBits{ 0 };
	long int outputBits{ 0 };

	// Input Variables
	t_real ber{ 0.03 };
	privacy_amplification_role paRole{ privacy_amplification_role::Undefined };
	t_integer securityParameter{ 1 };
	bool bypassHashing{ true };

	// Private Methods 
	template<std::size_t N>
	bitset<N> hashFn(bitset<N> bitsToHash);
	vector<t_binary> hashTheBits(const vector<t_binary>& vectorBitsToHash);

};
#endif // !PRIVACY_AMPLIFICATION_H_
