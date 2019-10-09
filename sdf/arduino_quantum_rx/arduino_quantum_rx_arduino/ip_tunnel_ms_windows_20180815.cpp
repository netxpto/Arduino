# include "ip_tunnel_ms_windows_20180815.h" 
# include "message_processor_common_20190410.h"
//# include "..\..\ip_tunnel_ms_windows\include\ip_tunnel_ms_windows_20180815.h" 
//# include "..\..\ip_tunnel_ms_windows\include\message_processor_common_20190410.h"


SOCKET clientSocket;
int n = 0;

void IPTunnel::initialize(void)
{

	if (inputSignals.empty()) {
		printf("server%d\n", n++);
		if (!server()) {
			printf("Error opening server\n");
			::exit(1);
		}
	}
	else {
		printf("client%d\n", n++);
		if (!client()) {
			printf("Error opening client\n");
			::exit(1);
		}
	}
}


bool IPTunnel::runBlock(void)
{
	int ready, process;

	if (inputSignals.empty()) { //server

		long space = outputSignals[0]->space();
		for (auto k : outputSignals) {
			long int aux = k->space();
			space = min(space, aux);
		}

		ipTunnelSendInt(space);
		//----------------------------------------RECEIVING THE SIGNAL----------------------------------------

		process = ipTunnelRecvInt();

		if (process == 0) {
			//alive = false;
			if (displayNumberOfSamples) {
				cout << "Samples received through IP Tunnel: " << process << "\n";
				
			}
			return false;
		}

		t_binary valueBinary;
		t_real valueReal;
		t_complex valueComplex;
		t_complex_xy valueComplexXy;
		t_photon_mp_xy valueComplexMp;

		t_message_type valueMType;
		t_message_data_length valueMDataLength;
		t_message_data valueMData;



		int result = 0;
		int remaining, remainingBufferType, remainingBufferDataLength;
		char* recv_buffer = 0;
		char* recv_bufferType = 0;
		char* recv_bufferDataLength = 0;
		signal_value_type sType = outputSignals[0]->getValueType();
		for (int k = 0; k < process; k++) {
			switch (sType) {
			case signal_value_type::t_binary:
				recv_buffer = (char*)& valueBinary;
				remaining = sizeof(t_binary);
				break;
			case signal_value_type::t_real:
				recv_buffer = (char*)& valueReal;
				remaining = sizeof(t_real);
				break;
			case signal_value_type::t_complex:
				recv_buffer = (char*)& valueComplex;
				remaining = sizeof(t_complex);
				break;
			case signal_value_type::t_complex_xy:
				recv_buffer = (char*)& valueComplexXy;
				remaining = sizeof(t_complex_xy);
				break;
			case signal_value_type::t_photon_mp_xy:
				recv_buffer = (char*)& valueComplexMp;
				remaining = sizeof(t_photon_mp_xy);
				break;
			case signal_value_type::t_message:
				recv_bufferType = (char*)& valueMType;
				//recv_bufferDataLength = (char*)& valueMDataLength;
				//recv_buffer = (char*)& valueMData;

				remainingBufferType = sizeof(t_message_type);
				remainingBufferDataLength = sizeof(t_message_data_length);
				//remaining = sizeof(t_message_data);//(t_message_data);


				break;
			default:
				printf("Error getting signal type\n");
				::exit(1);
			}

			string receivedString;

			if (sType == signal_value_type::t_message) {
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
				double numberOfMessages = ceil(lengthOfStringToReceive / 511);

				string tmpMsg{ "" };
				int msgReceived = 0;
				int msgToReceive = min(lengthOfStringToReceive, 511);


				for (int it = 0; it < numberOfMessages; ++it) {
					char msg[512];

					remaining = msgToReceive;//sizeof(msg);
					msgReceived = msgToReceive;
					msgToReceive = min(lengthOfStringToReceive - msgReceived, 511);

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
					if (result < 512)
						msg[result] = 0;

					string tmpMsg{ msg };
					receivedString = receivedString + tmpMsg;
				}
				//				receivedString = tmpMsg;
			}
			else {

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

			}
			//reconstructing message again

			t_message messageToBuffer;

			sType = outputSignals[0]->getValueType();
			switch (sType) {
			case signal_value_type::t_binary:
				outputSignals[0]->bufferPut(valueBinary);
				break;
			case signal_value_type::t_real:
				outputSignals[0]->bufferPut(valueReal);
				break;
			case signal_value_type::t_complex:
				outputSignals[0]->bufferPut(valueComplex);
				break;
			case signal_value_type::t_complex_xy:
				outputSignals[0]->bufferPut(valueComplexXy);
				break;
			case signal_value_type::t_photon_mp_xy:
				outputSignals[0]->bufferPut(valueComplexMp);
				break;
			case signal_value_type::t_message:

				messageToBuffer.messageData = receivedString;
				messageToBuffer.messageDataLength = to_string((t_message_data_length)valueMDataLength);
				messageToBuffer.messageType = valueMType;

				outputSignals[0]->bufferPut(messageToBuffer);
				break;
			default:
				printf("Error putting signal in buffer due to signal type unknown\n");
				::exit(1);
			}
		}
		if (displayNumberOfSamples) {
			cout << "Samples received through IP Tunnel: " << process << "\n";
		}
	}
	else { //client
	ready = inputSignals[0]->ready();

	int space = ipTunnelRecvInt();

	process = min((long int)ready, space);

	ipTunnelSendInt(process);
	// #############################
	for (int i=0; i < process; i++)
	{
		t_binary ipOut;
		inputSignals[0]->bufferGet(&ipOut);
		cout << ipOut << endl;
	}
	
	//##############################
	if (process == 0) {
		//alive = false;
		if (displayNumberOfSamples) {
			cout << "Samples sent through IP Tunnel: " << process << "\n";
		}
		return false;
	}
	signal_value_type sType = inputSignals[0]->getValueType();
	switch (sType) {
	case signal_value_type::t_binary:
		for (int k = 0; k < process; k++) {
			t_binary signalValue;
			inputSignals[0]->bufferGet(&signalValue);
			ipTunnelPut(signalValue);
		}
		break;
	case signal_value_type::t_real:
		for (int k = 0; k < process; k++) {
			t_real signalValue;
			inputSignals[0]->bufferGet(&signalValue);
			ipTunnelPut(signalValue);
		}
		break;
	case signal_value_type::t_complex:
		for (int k = 0; k < process; k++) {
			t_complex signalValue;
			inputSignals[0]->bufferGet(&signalValue);
			ipTunnelPut(signalValue);
		}
		break;
	case signal_value_type::t_complex_xy:
		for (int k = 0; k < process; k++) {
			t_complex_xy signalValue;
			inputSignals[0]->bufferGet(&signalValue);
			ipTunnelPut(signalValue);
		}
		break;

	case signal_value_type::t_photon_mp_xy:
		for (int k = 0; k < process; k++) {
			t_photon_mp_xy signalValue;
			inputSignals[0]->bufferGet(&signalValue);
			ipTunnelPut(signalValue);
		}
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
			double numberOfMessages = ceil(data.size() / 511.0);
			string tmpMsg;
			int msgSent = 0;
			int msgToSend = min((int)data.size(), 511);

			for (int it = 0; it < numberOfMessages; ++it) {
				char msg[512];


				tmpMsg.assign(data.begin() + msgSent, data.begin() + msgSent + msgToSend);
				msgSent = msgSent + msgToSend;
				msgToSend = min((int)data.size() - msgSent, 511);
				//acrescentar if se o size for maior que msg
				if (valueMDataLength > 512)
					printf("TAMANHO É MAIOR QUE 512");
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
		::exit(1);
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

	int connResult = -2;
	while (connResult != 0 || numberOfTrials == 0) {
		connResult = connect(clientSocket, (sockaddr*)& hint, sizeof(hint));
		if (connResult == SOCKET_ERROR)
		{
			cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
			cerr << "Waiting " << timeIntervalSeconds << " seconds." << endl;
		}

		Sleep(timeIntervalSeconds * 1000);
		;
		if (--numberOfTrials == 0) {
			cerr << "Reached maximum number of attempts." << endl;
			::exit(1);
		}
	}
	cout << "Connected!\n";
	return true;
}