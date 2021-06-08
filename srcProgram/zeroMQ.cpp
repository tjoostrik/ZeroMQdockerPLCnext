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
	string myText2;
	string myText3;
	list<string> config;
	// Lists for network configuration
	list<string> bipsub;
	list<string> cipsub;
	list<string> topicssub;
	list<string> bippub;
	list<string> cippub;
	list<string> topicspub;
	int pubSub = 0;
	
	// Makes a IO_thread for zeroMQ
	context_t context(1);
	
	// Reads every line of file
	while (getline (MyReadFile, myText)) {
		if (myText.find("#") != 0 && myText.compare("") != 0) {
			stringstream ss(myText);
			config.clear();
			while (ss >> myText) config.push_back(myText);

			myText = config.front();
			config.pop_front();	
	
			// Sets the program as subscriber
			if (myText.compare("-sub") == 0 && pubSub == 0) pubSub = 1;
			// Sets the program as publisher
			if (myText.compare("-pub") == 0 && pubSub == 0) pubSub = 2;
			// Sets the program as publisher
			if (myText.compare("-pub") == 0 && pubSub == 1) pubSub = 3;
			// Adds a port to bind
			if (myText.compare("-b") == 0 && (pubSub == 2 || pubSub == 3)) bippub.push_back(config.front());
			// Adds a port to bind
			if (myText.compare("-b") == 0 && pubSub == 1) bipsub.push_back(config.front());
			// Adds a ipaddress:port to connect
			if (myText.compare("-c") == 0 && (pubSub == 2 || pubSub == 3)) cippub.push_back(config.front());
			// Adds a ipaddress:port to connect
			if (myText.compare("-c") == 0 && pubSub == 1) cipsub.push_back(config.front());
			// Adds a topic to subscriber/publisher
			if (myText.compare("-t") == 0 && (pubSub == 2 || pubSub == 3)) topicspub.push_back(config.front());
			// Adds a topic to subscriber/publisher
			if (myText.compare("-t") == 0 && pubSub == 1) topicssub.push_back(config.front());
		}
	}
	 
	// Closes the file
	MyReadFile.close();
	
	// Stops if there isn't a subscriber or publisher defined
	if (pubSub == 0 || (cipsub.size() == 0 && bipsub.size() == 0 && cippub.size() == 0 && bippub.size() == 0)) return 0;

	// Makes a subscriber when defined
	socket_t subscriber(context, ZMQ_SUB);
	
	socket_t publisher(context, ZMQ_PUB);
		
	// Loops through bip
	for (string i : bipsub) {
		// Adds port to lokal ipaddress through tcp
		i = "tcp://*:" + i;
		// Sets up subscriber on ipaddress:port
		subscriber.bind(i);
		printf("zeroMQ subscriber binded at: " + i);
	}
		
	// Loops through cip
	for (string i : cipsub) {
		// Adds ipaddress:port to lokal ipaddress through tcp
		i = "tcp://" + i;
		// Connects subscriber to ipaddress:port
		subscriber.connect(i);
		printf("zeroMQ subscriber connected to: " + i);
	}
		
	// Loops through topics
	for (string i : topicssub) {
		// Adds topic to subscriber
		subscriber.setsockopt(ZMQ_SUBSCRIBE, i.c_str(), i.length());
		printf("Topic: " + i);
	}
	
	if (topicssub.size() == 0 && (pubSub == 1 || pubSub == 3)) {
		subscriber.setsockopt(ZMQ_SUBSCRIBE, "*", 1);
		printf("Topic: *");
	}
		
	// Loops through bip
	for (string i : bippub){
		// Adds port to lokal ipaddress through tcp
		i = "tcp://*:" + i;
		publisher.bind(i);
		printf("zeroMQ publisher binded at: " + i);
	}
		
	// Loops through cip
	for (string i : cippub) {
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
	
	for (string i : topicspub) {
		printf("Topic: " + i);
	}
	
	if (topicspub.size() == 0) {
		topicspub.push_back("*");
		printf("Topic: *");
	}
	
	// Runs until message END
	while(pubSub == 1) {
		// Recieves topic
		string address = s_recv(subscriber);
		// Recieves message
		string message = s_recv(subscriber);	
		printf("[" + address + "]: " + message);
		// Stops the program when message is END
		if (message == "END") return 1;
	}
		
	// Runs until stop is given
	while(pubSub == 2) {
		cout << "Type your word: ";
		cin >> message;
		// Stops when message is stop
		if (message == "stop") return 2;
		// Loops until message is send to every given topic
		for (string i : topicspub) {
			// Sends the given topic
			s_sendmore(publisher, i);
			// Sends the given message
			s_send(publisher, message);
		}	
	}
	
	// Runs until stop is given
	while(pubSub == 3) {
		// Recieves topic
		string address = s_recv(subscriber);
		// Recieves message
		string message = s_recv(subscriber);	
		printf("[" + address + "]: " + message);
		// Stops the program when message is END
		if (message == "FINISH") return 1;	
		// Loops until message is send to every given topic
		for (string i : topicspub) {
			// Sends the given topic
			s_sendmore(publisher, i);
			// Sends the given message
			s_send(publisher, message);
		}	
	}
}

// A function to let print look nicer
void printf(string print) {
	cout << print << endl;
}
