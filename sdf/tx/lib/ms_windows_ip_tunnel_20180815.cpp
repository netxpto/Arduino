# include "../include/ms_windows_ip_tunnel_20180815.h" 
#include "../include/message_processor_common_20190410.h"

constexpr int msgSegmentSize = 20000;
constexpr int sizeOfValArrays = 5000;

t_binary valArrayBin[sizeOfValArrays];
t_real valArrayReal[sizeOfValArrays];
t_complex valArrayComplex[sizeOfValArrays];
t_complex_xy valArrayComplexXY[sizeOfValArrays];
t_photon_mp_xy valArrayPhotonMPXY[sizeOfValArrays];

void IPTunnel::initialize(void)
{

	if (inputSignals.empty()) {
		printf("server%d\n", n++);
		if (!server()) {
			printf("Error opening server\n");
			//			::exit(1);
		}
	}
	else {
		printf("client%d\n", n++);
		if (!client()) {
			printf("Error opening client\n");
			//			::exit(1);
		}
	}
}


bool IPTunnel::runBlock(void)
{
	int ready, process;

	if (inputSignals.empty()) { //server

		signal_value_type sType = outputSignals[0]->getValueType();
		ipTunnelOutputData(outputSignals, sType);

		long space = outputSignals[0]->space();
		for (auto k : outputSignals) {
			long int aux = k->space();


			space = min(space, aux);
		}


		//----------------------------------------RECEIVING THE SIGNAL----------------------------------------

		process = ipTunnelRecvInt();

		if (process == 0) {
			//alive = false;
			if (displayNumberOfSamples) {
				cout << "Samples received through IP Tunnel: " << process << "\n";
			}
			return false;
		}


		if (sType == signal_value_type::t_message) {
			ipTunnelRecvMessages(outputSignals, process);
		}
		else {
			ipTunnelRecvValues(outputSignals, process, sType);
		}

		if (displayNumberOfSamples) {
			cout << "Samples received through IP Tunnel: " << process << "\n";
		}

		ipTunnelOutputData(outputSignals, sType);
	}
	else { //client
		signal_value_type sType = inputSignals[0]->getValueType();



		ready = inputSignals[0]->ready();



		process = (long int)ready;

		ipTunnelSendInt(process);

		if (process == 0) {
			//alive = false;
			if (displayNumberOfSamples) {
				cout << "Samples sent through IP Tunnel: " << process << "\n";
			}
			return false;
		}

		switch (sType) {
		case signal_value_type::t_binary:
			for (int k = 0; k < process; k++) {
				t_binary signalValue;
				inputSignals[0]->bufferGet(&signalValue);
				valArrayBin[k] = signalValue;
			}
			ipTunnelPut(valArrayBin, sizeof(t_binary), process);
			break;
		case signal_value_type::t_real:
			for (int k = 0; k < process; k++) {
				t_real signalValue;
				inputSignals[0]->bufferGet(&signalValue);
				valArrayReal[k] = signalValue;
			}
			ipTunnelPut(valArrayReal, sizeof(t_real), process);
			break;
		case signal_value_type::t_complex:
			for (int k = 0; k < process; k++) {
				t_complex signalValue;
				inputSignals[0]->bufferGet(&signalValue);
				valArrayComplex[k] = signalValue;
			}
			ipTunnelPut(valArrayComplex, sizeof(t_complex), process);
			break;
		case signal_value_type::t_complex_xy:
			for (int k = 0; k < process; k++) {
				t_complex_xy signalValue;
				inputSignals[0]->bufferGet(&signalValue);
				valArrayComplexXY[k] = signalValue;
			}
			ipTunnelPut(valArrayComplexXY, sizeof(t_complex_xy), process);
			break;

		case signal_value_type::t_photon_mp_xy:
			for (int k = 0; k < process; k++) {
				t_photon_mp_xy signalValue;
				inputSignals[0]->bufferGet(&signalValue);
				valArrayPhotonMPXY[k] = signalValue;
			}
			ipTunnelPut(valArrayPhotonMPXY, sizeof(t_photon_mp_xy), process);
			break;
		case signal_value_type::t_message:
			for (int k = 0; k < process; k++) {
				t_message message;
				inputSignals[0]->bufferGet(&message);

				t_message_type valueMType = MessageProcessors::getMessageType(message);
				t_message_data_length valueMDataLength = MessageProcessors::getMessageDataLength(message);

				ipTunnelPut(valueMType);
				ipTunnelSendInt(valueMDataLength);
				//////////////////////////////////////////////
				// Point of sending msg length

				string data = message.messageData;

				ipTunnelSendInt((int)data.size());
				int numberOfMessages = (int)ceil((int)data.size() / (double)(msgSegmentSize - 1));
				string tmpMsg;
				int msgSent = 0;
				int msgToSend = min((int)data.size(), msgSegmentSize - 1);

				for (int it = 0; it < numberOfMessages; ++it) {
					char msg[msgSegmentSize];


					tmpMsg.assign(data.begin() + msgSent, data.begin() + msgSent + msgToSend);
					msgSent = msgSent + msgToSend;
					msgToSend = min((int)data.size() - msgSent, msgSegmentSize - 1);
					//acrescentar if se o size for maior que msg
	//				if (valueMDataLength > 512)
	//					printf("TAMANHO É MAIOR QUE 512");
					strncpy_s(msg, tmpMsg.c_str(), sizeof(msg));

					//					msg[sizeof(msg) - 1] = 0;

					int strSz = (int)strlen(msg);


					char* tosend = (char*)& msg;
					int remaining = strSz;
					int result = 0;
					int sent = 0;
					while (remaining > 0) {
						result = send(clientSocket, tosend + sent, remaining, 0);
						if (result > 0) {
							remaining -= result;
							sent += remaining;
						}
						else if (result < 0) {
							printf("ERROR!\n");
							// probably a good idea to close socket
							break;
						}
					}
				}
			}
			break;
		default:
			printf("Error sending signal due to signal type unknown\n");
			//::exit(1);
		}
		if (displayNumberOfSamples) {
			cout << "Samples sent through IP Tunnel: " << process << "\n";
		}
	}
	return true;
}

void IPTunnel::terminate(void) {
	closesocket(clientSocket);
	WSACleanup();
}

template <class T>
int IPTunnel::ipTunnelPut(T object) {
	char* tosend = (char*)& object;
	int remaining = sizeof(object);
	int result = 0;
	int sent = 0;
	while (remaining > 0) {
		result = send(clientSocket, tosend + sent, remaining, 0);
		if (result > 0) {
			remaining -= result;
			sent += remaining;
		}
		else if (result < 0) {
			printf("ERROR sending object!\n");
			// probably a good idea to close socket
			break;
		}
	}
	return 0;
}

template <class T, size_t N>
int IPTunnel::ipTunnelPut(T(&object)[N], int elementSize, int processf) {
	char* tosend = (char*)& object;
	int remaining = elementSize * processf;
	int result = 0;
	int sent = 0;
	while (remaining > 0) {
		result = send(clientSocket, tosend + sent, remaining, 0);
		if (result > 0) {
			remaining -= result;
			sent += remaining;
		}
		else if (result < 0) {
			printf("ERROR sending object!\n");
			// probably a good idea to close socket
			break;
		}
	}
	return 0;
}

void IPTunnel::ipTunnelSendInt(int space) {
	int data = space;
	char* tosend = (char*)& data;
	int remaining = sizeof(data);
	int result = 0;
	int sent = 0;
	while (remaining > 0) {
		result = send(clientSocket, tosend + sent, remaining, 0);
		if (result > 0) {
			remaining -= result;
			sent += remaining;
		}
		else if (result < 0) {
			//printf("ERROR sending int!\n");
			// probably a good idea to close socket
			break;
		}
	}
}

int IPTunnel::ipTunnelRecvInt() {
	int value = 0;
	char* recv_buffer = (char*)& value;
	int remaining = sizeof(int);
	int received = 0;
	int result = 0;
	while (remaining > 0) {
		result = recv(clientSocket, recv_buffer + received, remaining, 0);
		if (result > 0) {
			remaining -= result;
			received += result;
		}
		else if (result == 0) {
			printf("Remote side closed his end of the connection\n");
			// probably a good idea to close socket
			break;
		}
		else if (result < 0) {
			//printf("ERROR receiving int!\n");
			// probably a good idea to close socket
			break;
		}
	}
	return value;
}

bool IPTunnel::server() {
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		cerr << "Can't Initialize winsock! Quitting" << endl;
		return false;
	}

	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		return false;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = ntohs(tcpPort);
	//inet_pton(AF_INET, (PCSTR)remoteMachineIpAddress.c_str(), &hint.sin_addr.s_addr); // hint.sin_addr.S_un.S_addr = inet_addr(ipAddressServer.c_str());
	hint.sin_addr.S_un.S_addr = INADDR_ANY;


	if (::bind(listening, (sockaddr*)& hint, sizeof(hint)) < 0) {
		printf("\n ERROR on binding");
		return false;
	}

	if (listen(listening, SOMAXCONN) == -1) {
		printf("\n ERROR on binding");
		return false;
	}

	bool setTcpNoDelay = true;
	int socketTcpNoDelayResult = setsockopt(listening, IPPROTO_TCP, TCP_NODELAY, (char*)& setTcpNoDelay, sizeof(setTcpNoDelay));

	sockaddr_in client;
	int clientSize = sizeof(client);

	clientSocket = accept(listening, (sockaddr*)& client, &clientSize);
	
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)& client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected on port " << service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " <<
			ntohs(client.sin_port) << endl;
	}
	return true;
}

bool IPTunnel::client() {

	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		cerr << "Can't start Winsock, Err #" << wsResult << endl;
		return false;
	}

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		cerr << "Can't create socket, Err #" << WSAGetLastError() << endl;
		WSACleanup();
		return false;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(tcpPort);
	inet_pton(AF_INET, remoteMachineIpAddress.c_str(), &hint.sin_addr);

	bool setTcpNoDelay = true;
	int socketTcpNoDelayResult = setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, (char*)& setTcpNoDelay, sizeof(setTcpNoDelay));

	int connResult = -2;
	while (connResult != 0 || numberOfTrials == 0) {
		connResult = connect(clientSocket, (sockaddr*)& hint, sizeof(hint));
		if (connResult == SOCKET_ERROR)
		{
			cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
			cerr << "Waiting " << timeIntervalSeconds << " seconds." << endl;
			Sleep(timeIntervalSeconds * 1000);
		}

		if (--numberOfTrials == 0) {
			cerr << "Reached maximum number of attempts." << endl;
			//::exit(1);
		}
	}
	cout << "Connected!\n";
	return true;
}


bool IPTunnel::ipTunnelRecvValues(vector <Signal*> outputS, int processf, signal_value_type stypef) {
	int result = 0;
	int remaining;
	char* recv_buffer = 0;

	switch (stypef) {
	case signal_value_type::t_binary:
		recv_buffer = (char*)& valArrayBin;
		remaining = sizeof(t_binary) * processf;
		break;
	case signal_value_type::t_real:
		recv_buffer = (char*)& valArrayReal;
		remaining = sizeof(t_real) * processf;
		break;
	case signal_value_type::t_complex:
		recv_buffer = (char*)& valArrayComplex;
		remaining = sizeof(t_complex) * processf;
		break;
	case signal_value_type::t_complex_xy:
		recv_buffer = (char*)& valArrayComplexXY;
		remaining = sizeof(t_complex_xy) * processf;
		break;
	case signal_value_type::t_photon_mp_xy:
		recv_buffer = (char*)& valArrayPhotonMPXY;
		remaining = sizeof(t_photon_mp_xy) * processf;
		break;
	default:
		printf("Error getting signal type\n");
		//::exit(1);
	}

	int received = 0;
	result = 0;
	while (remaining > 0) {
		result = recv(clientSocket, recv_buffer + received, remaining, 0);
		if (result > 0) {
			remaining -= result;
			received += result;
		}
		else if (result == 0) {
			printf("Remote side closed his end of the connection before all data was received\n");
			// probably a good idea to close socket
			break;
		}
		else if (result < 0) {
			printf("ERROR!\n");
			// probably a good idea to close socket
			break;
		}
	}

	switch (stypef) {
	case signal_value_type::t_binary:
		vBinary.insert(vBinary.end(), valArrayBin, valArrayBin + processf);
		break;
	case signal_value_type::t_real:
		vReal.insert(vReal.end(), valArrayReal, valArrayReal + processf);
		break;
	case signal_value_type::t_complex:
		vComplex.insert(vComplex.end(), valArrayComplex, valArrayComplex + processf);
		break;
	case signal_value_type::t_complex_xy:
		vComplex_xy.insert(vComplex_xy.end(), valArrayComplexXY, valArrayComplexXY + processf);
		break;
	case signal_value_type::t_photon_mp_xy:
		vPhoton_mp_xy.insert(vPhoton_mp_xy.end(), valArrayPhotonMPXY, valArrayPhotonMPXY + processf);
		break;
	default:
		printf("Error putting signal in buffer due to signal type unknown\n");
		//::exit(1);
	}

	return true;

}

bool IPTunnel::ipTunnelRecvMessages(vector <Signal*> outputS, int processf) {
	for (int k = 0; k < processf; ++k) {

		t_message_type valueMType;
		t_message_data_length valueMDataLength;
		t_message_data valueMData;

		int result = 0;
		int remaining, remainingBufferType, remainingBufferDataLength;
		char* recv_buffer = 0;
		char* recv_bufferDataLength = 0;
		char* recv_bufferType = (char*)& valueMType;

		remainingBufferType = sizeof(t_message_type);
		remainingBufferDataLength = sizeof(t_message_data_length);

		string receivedString;

		int received = 0;//receiving message type
		result = 0;
		while (remainingBufferType > 0) {
			result = recv(clientSocket, recv_bufferType + received, remainingBufferType, 0);
			if (result > 0) {
				remainingBufferType -= result;
				received += result;
			}
			else if (result == 0) {
				printf("Remote side closed his end of the connection before all data was received\n");
				// probably a good idea to close socket
				break;
			}
			else if (result < 0) {
				printf("ERROR!\n");
				// probably a good idea to close socket
				break;
			}
		}

		// Probably receive the number N of strings first, and iterate to receive N strings
		valueMDataLength = ipTunnelRecvInt();

		//////////////////////////////////////////////
		// Point of receiving msg length
		int lengthOfStringToReceive = ipTunnelRecvInt();
		int numberOfMessages = (int)ceil(lengthOfStringToReceive / (double)(msgSegmentSize - 1));

		string tmpMsgStr{ "" };
		int msgReceived = 0;
		int msgToReceive = min(lengthOfStringToReceive, msgSegmentSize - 1);

		if (valueMType == t_message_type::ParameterEstimationSeedAndBitsMsg) {
			cout << "RX Parameter Estimation: " << lengthOfStringToReceive << " - ";
		}
		else if (valueMType == t_message_type::BasisReconciliationMsg) {
			//			cout << "RX Basis Reconciliation Message: " << lengthOfStringToReceive << " - ";
		}

		for (int it = 0; it < numberOfMessages; ++it) {
			char msg[msgSegmentSize];

			remaining = msgToReceive;//sizeof(msg);
			msgReceived = msgToReceive;
			msgToReceive = min(lengthOfStringToReceive - msgReceived, msgSegmentSize - 1);

			recv_buffer = (char*)& msg;

			received = 0;//receiving message data
			result = 0;
			while (remaining > 0) {
				result = recv(clientSocket, recv_buffer + received, remaining, 0);
				if (result > 0) {
					remaining -= result;
					received += result;
				}
				else if (result == 0) {
					printf("Remote side closed his end of the connection before all data was received\n");
					// probably a good idea to close socket
					break;
				}
				else if (result < 0) {
					printf("ERROR!\n");
					// probably a good idea to close socket
					break;
				}
			}

			//msg[512 - (512 - valueMDataLength)] = 0;
			if (result < msgSegmentSize) {
				if (lengthOfStringToReceive >= msgSegmentSize) {
					cout << "Length to receive bigger than recv buffer?";
				}
				else {
					msg[lengthOfStringToReceive] = 0;
				}
			}
			string tmpMsgStr{ msg };
			receivedString = receivedString + tmpMsgStr;
		}

		t_message tmpMsg;
		tmpMsg.messageData = receivedString;
		tmpMsg.messageDataLength = to_string((t_message_data_length)valueMDataLength);
		tmpMsg.messageType = valueMType;

		if (valueMDataLength != receivedString.length()) {
			cout << "";
		}

		vMsg.push_back(tmpMsg);

	}
	return true;
}

bool IPTunnel::ipTunnelOutputData(vector <Signal*> outputS, signal_value_type sType) {
	bool alive = false;

	int space = outputS[0]->getBufferLength();

	for (auto k : outputS) space = (space < k->space()) ? space : k->space();
	int processf;
	switch (sType) {
	case signal_value_type::t_binary:
		processf = min(space, (int)vBinary.size());
		for (int k = 0; k < processf; ++k) {
			for (unsigned int s = 0; s < outputS.size(); ++s) {
				outputS[s]->bufferPut(vBinary[k]);
			}
		}
		vBinary.erase(vBinary.begin(), vBinary.begin() + processf);
		break;
	case signal_value_type::t_real:
		processf = min(space, (int)vReal.size());
		for (int k = 0; k < processf; ++k) {
			for (unsigned int s = 0; s < outputS.size(); ++s) {
				outputS[s]->bufferPut(vReal[k]);
			}
		}
		vReal.erase(vReal.begin(), vReal.begin() + processf);
		break;
	case signal_value_type::t_complex:
		processf = min(space, (int)vComplex.size());
		for (int k = 0; k < processf; ++k) {
			for (unsigned int s = 0; s < outputS.size(); ++s) {
				outputS[s]->bufferPut(vComplex[k]);
			}
		}
		vComplex.erase(vComplex.begin(), vComplex.begin() + processf);
		break;
	case signal_value_type::t_complex_xy:
		processf = min(space, (int)vComplex_xy.size());
		for (int k = 0; k < processf; ++k) {
			for (unsigned int s = 0; s < outputS.size(); ++s) {
				outputS[s]->bufferPut(vComplex_xy[k]);
			}
		}
		vComplex_xy.erase(vComplex_xy.begin(), vComplex_xy.begin() + processf);
		break;
	case signal_value_type::t_photon_mp_xy:
		processf = min(space, (int)vPhoton_mp_xy.size());
		for (int k = 0; k < processf; ++k) {
			for (unsigned int s = 0; s < outputS.size(); ++s) {
				outputS[s]->bufferPut(vPhoton_mp_xy[k]);
			}
		}
		vPhoton_mp_xy.erase(vPhoton_mp_xy.begin(), vPhoton_mp_xy.begin() + processf);
		break;
	case signal_value_type::t_message:
		processf = min(space, (int)vMsg.size());
		for (int k = 0; k < processf; ++k) {
			for (unsigned int s = 0; s < outputS.size(); ++s) {
				outputS[s]->bufferPut(vMsg[k]);
			}
		}
		vMsg.erase(vMsg.begin(), vMsg.begin() + processf);
		break;
	default:
		auto outputVec = { 0 };
		printf("Error putting signal in buffer due to signal type unknown\n");
		//::exit(1);
	}

	alive = true;
	return alive;
}