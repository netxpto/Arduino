//#include <windows.h>
#include <WS2tcpip.h> //official windows header with some functions needed
#include <winsock2.h>


#include <iostream>
#include "./include/SerialPort.h"
#include <stdio.h>
#include <string.h>
#include <chrono>


using namespace std;

char* portName = (char*)"\\\\.\\COM9";

#define MAX_DATA_LENGTH 4000

char incomingData[MAX_DATA_LENGTH];
string remoteMachineIpAddress{ "127.0.0.1" };

int repetitionFrequency = 8000;
int bufferTime = (double)MAX_DATA_LENGTH / (double)repetitionFrequency * 1000;
int tcpPort{ 54000 };
int numberOfTrials{ 10 };
int timeIntervalSeconds{ 3 };
int burnInt = 10;
int burnCounter = 0;
auto start = chrono::steady_clock::now();


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
			printf("ERROR sending object!\n");
			// probably a good idea to close socket
			break;
		}
	}
	return 0;
}



//Arduino SerialPort object
SerialPort* arduino;

//If you want to send data then define "SEND" else comment it out
//#define SEND

void receiveData(void)
{
	int readResult = arduino->readSerialPort(incomingData, MAX_DATA_LENGTH);
	//printf(incomingData);
}

void exampleWriteData(unsigned int delayTime)
{
}

void autoConnect(void)
{
	//wait connection
	while (!arduino->isConnected()) {
		Sleep(100);
		arduino = new SerialPort(portName);
	}

	//Checking if arduino is connected or not
	if (arduino->isConnected()) {
		std::cout << "Connection established at port " << portName << endl;

		for (int i = 0; i < burnInt; i++)
		{
			auto end = chrono::steady_clock::now();
			while (bufferTime - chrono::duration_cast<chrono::milliseconds>(end - start).count() > 0) end = chrono::steady_clock::now();

			receiveData();
			printf("Burning data! -> %d\n", i);
			start = chrono::steady_clock::now();
		}

	}

	while (arduino->isConnected()) {
		auto end = chrono::steady_clock::now();
		while (bufferTime - chrono::duration_cast<chrono::milliseconds>(end - start).count() > 0) end = chrono::steady_clock::now();

		receiveData();
		start = chrono::steady_clock::now();


		int ready = MAX_DATA_LENGTH;
		ipTunnelSendInt(ready);

		for (size_t i = 0; i < ready; i++)
		{
			int data = incomingData[i] - '0';
			ipTunnelPut(data);
		}

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


	arduino = new SerialPort(portName);

	autoConnect();
}
