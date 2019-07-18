#ifndef BASIS_RECONCILIATION_H_
#define BASIS_RECONCILIATION_H_

#include "../../../include/netxpto_20180815.h"


enum class basis_reconciliation_role{ Alice, Bob, Undefined};

class BasisReconciliation : public Block {

public:

	BasisReconciliation(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig) : Block(InputSig, OutputSig) {};

	void initializer(void);
	bool runBlock(void);

	void setBasisReconciliationRole(basis_reconciliation_role role) { basisReconciliationRole = role; }
	void setDoubleClickNumber(t_integer dcn) { doubleClickNumber = dcn; }
	void setNoClickNumber(t_integer ncn) { noClickNumber = ncn; }

	long int getReceivedQubits(void) { return receivedQubits; }
	long int getReconciledBits(void) { return reconciledBits; }

private:
	// Input Variables
	basis_reconciliation_role basisReconciliationRole{ basis_reconciliation_role::Undefined };
	t_integer doubleClickNumber{ 2 };
	t_integer noClickNumber{ 3 };

	// State Variables
	bool bobBasisReconStatus{ false };
	vector<t_integer> storedData = {};
	long int receivedQubits{ 0 };
	long int reconciledBits{ 0 };
	vector<t_integer> msgOutputData{};
	vector<t_integer> msgInputData{};
	bool startedSending{ false };
	bool finishedSending{ false };


	bool startedReceiving{ false };
	bool finishedReceiving{ false };
	t_real numberOfBasisToReceive{ 0 };

	bool sendToMsg(vector <t_integer> &data, Signal &signalToMsg, bool &started);


};

#endif // !BASIS_RECONCILIATION_H_
