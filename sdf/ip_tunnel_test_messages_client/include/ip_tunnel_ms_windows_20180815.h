# ifndef MS_WINDOWS_IP_TUNNEL_H_
# define MS_WINDOWS_IP_TUNNEL_H_

# include "../../../include/netxpto_20180815.h"
#include <WS2tcpip.h> //official windows header with some functions needed

class IPTunnel : public Block {

public:

	// ##########################################################################

	IPTunnel(initializer_list<Signal*> InputSig, initializer_list<Signal*> OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);
	void terminate(void);

	void ipTunnelSendInt(int space);
	int ipTunnelRecvInt();

	template <class T>
	int ipTunnelPut(T object);

	template <class T>
	int ipTunnelPut(T object, int objectSize);

	
	template <class T, size_t N>
	int ipTunnelPut(T(&object)[N], int elementSize, int processf);

	
	bool ipTunnelRecvValues(vector <Signal*> outputS, int processf, signal_value_type stypef);

	bool ipTunnelRecvMessages(vector <Signal*> outputS, int processf);
	// ##########################################################################

	void setDisplayNumberOfSamples(bool opt) { displayNumberOfSamples = opt; };
	bool getDisplayNumberOfSamples() const { return displayNumberOfSamples; };

	void setRemoteMachineIpAddress(string rMachineIpAddress) { remoteMachineIpAddress = rMachineIpAddress; };
	string getRemoteMachineIpAddress() const { return remoteMachineIpAddress; };

	void setTcpPort(int tcpP) { tcpPort = tcpP; };
	int getTcpPort() const { return tcpPort; };

	void setNumberOfTrials(int nOfTrials) { numberOfTrials = nOfTrials; };
	int getNumberOfTrials() const { return numberOfTrials; };

	void setTimeIntervalSeconds(int tIntervalSeconds) { timeIntervalSeconds = tIntervalSeconds; };
	int getTimeIntervalSeconds() const { return timeIntervalSeconds; };

	bool server();
	bool client();


	// ###########################################################################
	
private:

	// # Input Parameters ########################################################
	bool displayNumberOfSamples{ true };
	string remoteMachineIpAddress{ "127.0.0.1" };
	int tcpPort{ 54000 };
	int numberOfTrials{ 10 };
	int timeIntervalSeconds{ 3 };

	SOCKET clientSocket;
	int n = 0;

};

#endif
