# ifndef MS_WINDOWS_IP_TUNNEL_H_
# define MS_WINDOWS_IP_TUNNEL_H_

# include "netxpto_20180815.h"
<<<<<<< HEAD
//#include "WS2tcpip.h" //official windows header with some functions needed
=======
#include "WS2tcpip.h" //official windows header with some functions needed
>>>>>>> 37855f1d0b3eba88c8f5b3ee3e0c9155928dbe15

class IPTunnel : public Block {

public:

	// ##########################################################################

	IPTunnel(vector<Signal*> InputSig, vector<Signal*> OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);
	void terminate(void);

	void ipTunnelSendInt(int space);
	int ipTunnelRecvInt();

	template <class T>
	int ipTunnelPut(T object);

	template <class T>
	int ipTunnelPut(T object, int objectSize);
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

};

#endif
