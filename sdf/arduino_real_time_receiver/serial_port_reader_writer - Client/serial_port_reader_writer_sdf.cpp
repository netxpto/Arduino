//#include <windows.h>
#include <WS2tcpip.h> //official windows header with some functions needed
#include <winsock2.h>


#include <iostream>
#include "./include/SerialPort.h"
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <vector>


using namespace std;

char* portNameRec = (char*)"\\\\.\\COM9";
char* portNameDac = (char*)"\\\\.\\COM3";

SerialPort* arduinoRec;
SerialPort* arduinoDac;


#define MAX_DATA_LENGTH 4000

char incomingData[MAX_DATA_LENGTH];
string remoteMachineIpAddress{ "127.0.0.1" };

int repetitionFrequency = 8000;
int bufferTime = (double)MAX_DATA_LENGTH / (double)repetitionFrequency * 1000;
int tcpPort{ 54000 };
int numberOfTrials{ 100 };
int timeIntervalSeconds{ 3 };
int burnInt = 10;
int burnCounter = 0;
auto start = chrono::steady_clock::now();

char buffer[MAX_DATA_LENGTH];


SOCKET clientSocket;
int n = 0;
bool client() {

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
		connResult = connect(clientSocket, (sockaddr*)&hint, sizeof(hint));
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

void ipTunnelSendInt(int space) {
	int data = space;
	char* tosend = (char*)&data;
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

int ipTunnelRecvInt() {
	int value = 0;
	char* recv_buffer = (char*)&value;
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

void terminate(void) {
	closesocket(clientSocket);
	WSACleanup();
}

template <class T>
int ipTunnelPut(T object) {
	char* tosend = (char*)&object;
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
			printf("Connection lost, reconnecting\n");
			closesocket(clientSocket);
			client();
			return 0;
			// probably a good idea to close socket
			break;
		}
	}
	return 1;
}




bool hasWritten;
bool hasRead;
int updateCounter = 0;


void autoConnect(void)
{
	//wait connection
	while (!arduinoRec->isConnected() || !arduinoDac->isConnected()) {
		Sleep(100);

		arduinoRec = new SerialPort(portNameRec);
		arduinoDac = new SerialPort(portNameDac);
	}


	//Checking if arduino is connected or not
	if (arduinoRec->isConnected() && arduinoDac->isConnected()) {
		std::cout << "Connection established at ports " << portNameRec << " and " << portNameDac << endl;

		auto start = chrono::steady_clock::now();
		auto end = chrono::steady_clock::now();
		std::cout << "Burning through data now, allow for around 1 second." << endl;


		while (1 * 1000 - chrono::duration_cast<chrono::milliseconds>(end - start).count() > 0) { // burns through 10 seconds worth of data
			end = chrono::steady_clock::now();
			while (*incomingData != '0' & *incomingData != '1' & *incomingData != '2' & *incomingData != '3') hasRead = arduinoRec->readSerialPort(incomingData, 1);
		}
	}

	char selected;
	char* sendString;
	int bufferCounter = 0;

	while (arduinoRec->isConnected() && arduinoDac->isConnected()) {
		updateCounter++;
		while (*incomingData != '0' & *incomingData != '1' & *incomingData != '2' & *incomingData != '3') hasRead = arduinoRec->readSerialPort(incomingData, 1);
		if (hasRead) {
			buffer[bufferCounter] = *incomingData;
			bufferCounter++;
		}
		memset(incomingData, 0, sizeof(incomingData));

		if (bufferCounter == MAX_DATA_LENGTH)
		{
			bufferCounter = 0;
			int trash = ipTunnelRecvInt();
			ipTunnelSendInt(MAX_DATA_LENGTH);
			std::cout << "Sending " << MAX_DATA_LENGTH << " counts to netxpto!" << endl;

			for (size_t i = 0; i < MAX_DATA_LENGTH; i++) {
				
				int result = ipTunnelPut(buffer[i] - '0');
				if (!result) break;
			}
		}
		
		
		if (updateCounter == 500)
		{
			updateCounter = 0;
			selected = 'U';
			sendString = &selected;
			hasWritten = arduinoDac->writeSerialPort(sendString, 1);




			int aux1_0 = rand() % 256;
			int aux2_0 = rand() % 256;
			int aux3_0 = rand() % 256;
			int aux4_0 = rand() % 256;
			int aux5_0 = rand() % 256;
			int aux6_0 = rand() % 256;
			int aux7_0 = rand() % 256;
			int aux8_0 = rand() % 256;

			int aux1_1 = rand() % 256;
			int aux2_1 = rand() % 256;
			int aux3_1 = rand() % 256;
			int aux4_1 = rand() % 256;
			int aux5_1 = rand() % 256;
			int aux6_1 = rand() % 256;
			int aux7_1 = rand() % 256;
			int aux8_1 = rand() % 256;

			char sendArray[] = {(char)aux1_0, (char)aux2_0, (char)aux3_0, (char)aux4_0, (char)aux5_0, (char)aux6_0, (char)aux7_0, (char)aux8_0, (char)aux1_1, (char)aux2_1, (char)aux3_1, (char)aux4_1, (char)aux5_1, (char)aux6_1, (char)aux7_1, (char)aux8_1 };

			arduinoDac->writeSerialPort(sendArray, 16);



		}

		int aux = rand() % 2;
		char selected = (char)aux;

		char* sendString = &selected;
		bool hasWritten = arduinoDac->writeSerialPort(sendString, 1);


	}





	//if the serial connection is lost
	autoConnect();
}

int main()
{
	printf("client%d\n", n++);
	if (!client()) {
		printf("Error opening client\n");
		::exit(1);
	}


	arduinoRec = new SerialPort(portNameRec);
	arduinoDac = new SerialPort(portNameDac);


	autoConnect();
}
