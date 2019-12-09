#define _CRT_SECURE_NO_WARNINGS

#include <cctype>
#include <memory>
#include <exception>
#include <Windows.h>
#include <iostream>
#include <string>
#include "serial.h"
#include "accesscard.h"
#include "accesslevel.h"
#include "accesscardstorage.h"
#include "cardqueue.h"

static char HEARTBEAT = '?';
static char NOT_READY = '!';

void GetSerialPortName(char* buf, size_t bufsize) {

	char port_no[25];
	std::string output;
	std::cout << "Enter port number for serial device (1..9): ";
	std::cin >> port_no;

	char portname[20] = "\\\\.\\COM";
	strcat(portname, port_no);
	strcpy(buf, portname);
}

void StartRuntime(std::unique_ptr<AccessCardStorage> storage, SERIALPORT* port) {
	
	bool suspend;
	char serial_read_buff[50];
	AccessCard card;
	CardAccess::queue<AccessCard> scan_queue;

	std::cout << "To exit, hit escape." << std::endl << std::endl;

	while (true) {
		
		if(GetKeyState(0x1B) & 0x8000) {
			return;
		}

		serial_read_buff[0] = NOT_READY;

		SerialReadToNewLine(*port, serial_read_buff, sizeof serial_read_buff);
		
		if (serial_read_buff[0] == NOT_READY) {
			continue;
		} else {
			suspend = false;
		}

		if (serial_read_buff[0] != HEARTBEAT) {

			if (!storage->access_card_exists(serial_read_buff)) {
				std::cout << "Unrecognized card." << std::endl;
				continue;
			}
			
			auto card = storage->get_access_card(serial_read_buff);

			if (card.get_access_level() != ACCESS_LEVEL::ACCESS) {
				std::cout << "Access denied." << std::endl;
				continue;
			}

			scan_queue.enqueue(card);
			std::cout << "Enqueued: " << card.get_id() << std::endl;
			std::cout << "There are " << scan_queue.size() << " scans in queue.";
			std::cout << std::endl << std::endl;
			continue;
		}

		if (scan_queue.size() > 0 && suspend == false) {
			auto card = scan_queue.dequeue();

			std::cout << "Dequeued: " << card.get_id() << std::endl;
			std::cout << "There are " << scan_queue.size() << " scans in queue." << std::endl;
			SerialWritePort(*port, R"(<REMOTEOPEN>)", sizeof(R"(<REMOTEOPEN>)"));
			std::cout << "Remote open door command sent to device." << std::endl << std::endl;
			suspend = true;
		}
	}
} 

void test() {
	AccessCard c = AccessCard();
	AccessCardStorage store = AccessCardStorage();
	CardAccess::queue<AccessCard> testQueue;

	testQueue.enqueue(AccessCard("27103", ACCESS_LEVEL::ACCESS));
	testQueue.enqueue(AccessCard("27103", ACCESS_LEVEL::ACCESS));
	testQueue.enqueue(AccessCard("27103", ACCESS_LEVEL::ACCESS));

	auto card = testQueue.dequeue();
	std::cout << card.get_id() << std::endl;
}

int main(){

	//test();
	bool removed;
	int selection;
	char portname[50];
	char serial_read_buff[50];
	std::string access_choice;
	auto storage = std::make_unique<AccessCardStorage>();
	
	GetSerialPortName(portname, sizeof(portname));
	std::cout << "Trying to establish connection..." << std::endl;
	SERIALPORT port = SerialInit(portname);
	
	if (!SerialIsConnected(port)) {
		return -1;
	}

	std::cout << "Successfully connected to " << portname;
	std::cout << std::endl << std::endl;

	while (true) {
		std::cout << std::endl;

		std::cout << "1. Register card" 	<< std::endl
				  << "2. Delete card"   	<< std::endl
				  << "3. Edit access level"	<< std::endl
				  << "4. Start runtime"		<< std::endl
				  << "-> "; 

		std::cin >> selection;
		std::cout << std::endl;
		serial_read_buff[0] = HEARTBEAT;
		
		switch(selection) {
			case 1:
				std::cout << "Place the card on the device." << std::endl;
				SerialReadToNewLine(port, serial_read_buff, sizeof serial_read_buff);

				while (serial_read_buff[0] == HEARTBEAT || strlen(serial_read_buff) < 3) {
					memset(serial_read_buff, 0, sizeof(serial_read_buff));
					SerialReadToNewLine(port, serial_read_buff, sizeof serial_read_buff);
				}

				if (storage->access_card_exists(serial_read_buff)) {
					std::cout << "Card already registered." << std::endl;
					break;
				}
				std::cout << std::endl;
				storage->add_access_card(serial_read_buff, ACCESS_LEVEL::NO_ACCESS);
				std::cout << "Registered card " << serial_read_buff << std::endl;
				std::cout << "Default for new cards is no access." << std::endl;
				break;

			case 2:
				std::cout << "Place the card on the device." << std::endl;
				
				SerialReadToNewLine(port, serial_read_buff, sizeof serial_read_buff);

				while (serial_read_buff[0] == HEARTBEAT) {
					memset(serial_read_buff, 0, sizeof(serial_read_buff));
					SerialReadToNewLine(port, serial_read_buff, sizeof serial_read_buff);
				}

				removed = storage->remove_access_card(serial_read_buff);
				std::cout << "Card" << serial_read_buff << " removal was ";
				std::cout << (removed ? "successful" : "unsuccessful");
				std::cout << std::endl;
				break;
			
			case 3:
				std::cout << "Place the card on the device." << std::endl;
				SerialReadToNewLine(port, serial_read_buff, sizeof serial_read_buff);

				while (serial_read_buff[0] == HEARTBEAT) {
					memset(serial_read_buff, 0, sizeof(serial_read_buff));
					SerialReadToNewLine(port, serial_read_buff, sizeof serial_read_buff);
				}

				std::cout << "Does this card have access to unlock? y/n -> ";
				std::cin >> access_choice;

				if (access_choice == "y" || access_choice == "Y") {
					storage->assign_access(serial_read_buff, ACCESS_LEVEL::ACCESS);
					std::cout << "Set access to Granted." << std::endl;
				} else {
					storage->assign_access(serial_read_buff, ACCESS_LEVEL::NO_ACCESS);
					std::cout << "Set access to Denied." << std::endl;
				}

				break;
			case 4:
				std::cout << "Entering runtime mode." << std::endl;
				StartRuntime(move(storage), &port);
				break;
		}
	}
}