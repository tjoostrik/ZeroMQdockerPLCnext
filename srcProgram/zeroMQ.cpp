#include "zhelpers.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <list>

using namespace std;
using namespace zmq;

void printf(string print);

int main (int argc, char *argv[]) {

	// Gets config file
	ifstream MyReadFile(argv[1]);
	// String
	string myText;
	// Lists for network configuration
	list<string> bip;
	list<string> cip;
	list<string> topics;
	char pubSub = 0;
	// variables helping read the file
	bool nextIsBind = false;
	bool nextIsConnect = false;
	bool nextIsTopic = false;
	
	// Reads every line of file
	while (getline (MyReadFile, myText, ' ')) {
		// Sets the program as subscriber
		if (myText.compare("-sub") == 0 && pubSub == 0) pubSub = 1;
		// Sets the program as publisher
		if (myText.compare("-pub") == 0 && pubSub == 0) pubSub = 2;
		// Adds a port to bind
		if (nextIsBind) {
			bip.push_back(myText);
			nextIsBind = false;
		}
		// Says next word is a port to bind
		if (myText.compare("-b") == 0) nextIsBind = true;
		// Adds a ipaddress:port to connect
		if (nextIsConnect) {
			cip.push_back(myText);
			nextIsConnect = false;
		}
		// Says next word is a ipaddress:port to connect
		if (myText.compare("-c") == 0) nextIsConnect = true;
		// Adds a topic to subscriber/publisher
		if (nextIsTopic) {
			topics.push_back(myText);
			nextIsTopic = false;
		}
		// Says next word is a topic
		if (myText.compare("-t") == 0) nextIsTopic = true;
	}
	
	// Closes the file
	MyReadFile.close();
	// Makes a IO_thread
	context_t context(1);
	
	// Stops if there isn't a subscriber or publisher defined
	if (pubSub == 0 || (cip.size() == 0 && bip.size() == 0)) return 0;
	
	// Makes a subscriber when defined
	if (pubSub == 1) {
		// Makes a subscriber
		socket_t subscriber(context, ZMQ_SUB);
		
		// Loops through bip
		for (string i : bip) {
			// Adds port to lokal ipaddress through tcp
			i = "tcp://*:" + i;
			// Sets up subscriber on ipaddress:port
			subscriber.bind(i);
			printf("zeroMQ subscriber binded at: " + i);
		}
		
		// Loops through cip
		for (string i : cip) {
			// Adds ipaddress:port to lokal ipaddress through tcp
			i = "tcp://" + i;
			// Connects subscriber to ipaddress:port
			subscriber.connect(i);
			printf("zeroMQ subscriber connected to: " + i);
		}
		
		// Loops through topics
		for (string i : topics) {
			// Adds topic to subscriber
			subscriber.setsockopt(ZMQ_SUBSCRIBE, i.c_str(), i.length());
			printf("Topic: " + i);
		}
		
		// Runs until message END
		while(1) {
			// Recieves topic
			string address = s_recv(subscriber);
			// Recieves message
			string message = s_recv(subscriber);	
			printf("[" + address + "]: " + message);
			// Steps out of loop when message is END
			if (message == "END") break;
		}
		// Stops the program
		return 1;
	}
	
	// Makes publisher when defined
	socket_t publisher(context, ZMQ_PUB);
		
	// Loops through bip
	for (string i : bip){
		// Adds port to lokal ipaddress through tcp
		i = "tcp://*:" + i;
		// Sets up publisher on ipaddress:port
		publisher.bind(i);
		printf("zeroMQ publisher binded at: " + i);
	}
		
	// Loops through cip
	for (string i : cip) {
		// Adds ipaddress:port to lokal ipaddress through tcp
		i = "tcp://" + i;
		// Connects publisher to ipaddress:port
		publisher.connect(i);
		printf("zeroMQ publisher connected to: " + i);
	}
		
	// Sleeps 20 ms so the publisher started 
	usleep(20000);
	
	// Makes variable voor messages to send
	string message;
		
	// Runs until stop is given
	while(1) {
		cout << "Type your word: ";
		cin >> message;
		// Stops when message is stop
		if (message == "stop") break;
		// Loops until message is send to every given topic
		for (string i : topics) {
			// Sends the given topic
			s_sendmore(publisher, i);
			// Sends the given message
			s_send(publisher, message);
		}	
	}
	
	// Stops the program
	return 2;
}

// A function to let print look nicer
void printf(string print) {
	cout << print << endl;
}
