#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include <conio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>
#include "safeinput.h"
#include "accesscard.h"
#include "serial.h"
#define KEY_DOWN(vk_code)((GetAsyncKeyState((vk_code))&0x8000 ? 1:0))



bool RegisterNewAccessCardPanel(SERIALPORT port, CARD_INVENTORY* card_inventory_state) {
	time_t now;
	time(&now);
	short new_card_index;
	char new_card_number[50];
	char access_is_granted_input[5];
	char fullname_input[100];
	INPUT_RESULT valid_entry = INPUT_RESULT_NO_INPUT;
	
	new_card_number[0] = 0;

	printf("\n---> You are here: [Access Card Control Panel / Add a new access card]\n");
	printf("\n--- [INFO]: Place the card on the reader.\n");

	while (new_card_number[0] == 0)	{
		SerialReadToNewLine(port, new_card_number, sizeof(new_card_number));
	}
		
	printf("--- [SERIAL]: Card number: %s\n", new_card_number);


	if (AccessCardExists(card_inventory_state, new_card_number)) {
		printf("--- [WARNING]: This card number is occupied\n");
		return false;
	} else {
		new_card_index = GetIndexForNewAccessCard(card_inventory_state);
	}

	while (valid_entry != INPUT_RESULT_OK) {
		valid_entry = GetInput("--- Enter the full name for this user -> ", 
			fullname_input, sizeof(fullname_input)
		);
	}
	
	GetInput("--- Does this card have access to unlock? (y/n) -> ",
		access_is_granted_input, sizeof(access_is_granted_input));

	while (access_is_granted_input[0] != 'n' && access_is_granted_input[0] != 'y') {
		printf("\n--- [ERROR]: Invalid entry, try again\n");
		GetInput("--- Does this card have access to unlock? (y/n) -> ",
			access_is_granted_input, sizeof(access_is_granted_input));
	}
	
	if (access_is_granted_input[0] == 'y') {
		card_inventory_state->array_of_access_cards[new_card_index].access_level = ACCESS_GRANTED;
	} else if (access_is_granted_input[0] == 'n') {
		card_inventory_state->array_of_access_cards[new_card_index].access_level = ACCESS_DENIED;
	}

	card_inventory_state->array_of_access_cards[new_card_index].session_status = IS_SIGNED_OUT;
	card_inventory_state->array_of_access_cards[new_card_index].registration_timestamp = now;
	strcpy(card_inventory_state->array_of_access_cards[new_card_index].number, new_card_number);
	strcpy(card_inventory_state->array_of_access_cards[new_card_index].fullname, fullname_input);

	if (AccessCardExists(card_inventory_state, new_card_number)) {
		return true;
	}
	return false;
}


void ListAllAccessCards(CARD_INVENTORY* card_inventory_state) {
	char temp_datetime_buffer[80];
	struct tm temp_timestruct;

	if (card_inventory_state->number_of_access_cards < 1) {
		printf("\n-- [WARNING]: There are no stored access cards.\n\n");
		return;
	}

	printf("\n-- Stored Access Cards Below:\n\n");

	for (int i = 0; i < card_inventory_state->number_of_access_cards; i++) {
	
		time_t rawtime = card_inventory_state->array_of_access_cards[i].registration_timestamp;
		temp_timestruct = *localtime(&rawtime);
		strftime(temp_datetime_buffer, sizeof(temp_datetime_buffer), "%a %Y-%m-%d %H:%M:%S", &temp_timestruct);

		printf(" * Card number:\t\t\t\t%s\n",card_inventory_state->array_of_access_cards[i].number);
		printf(" * Registered to:\t\t\t%s\n",card_inventory_state->array_of_access_cards[i].fullname);
		printf(" * Card has access to unlock:\t\t");
		printf(card_inventory_state->array_of_access_cards[i].access_level == ACCESS_GRANTED ? "true\n" : "false\n");

		printf(" * Card is signed in:\t\t\t");
		printf(card_inventory_state->array_of_access_cards[i].session_status == IS_SIGNED_IN ? "true\n" : "false\n");

		printf(" * Card added to system:\t\t%s", temp_datetime_buffer);
		printf("\n\n");
	}
}


void RemoteOpenDoor(SERIALPORT port) {
	char buf[512];

	SerialWritePort(port, "<REMOTEOPEN>", strlen("<REMOTEOPEN>"));

	for (char i = 0; i < 4; i++) {
		SerialReadToNewLine(port, buf, sizeof(buf));
		printf("%s\n", buf);
		Sleep(1000);
	}
}


bool EditAccessCardPanel(SERIALPORT port, CARD_INVENTORY* card_inventory_state) {
	char access_is_granted[5];
	char card_number_to_search_for[50];
	short access_card_index;
	char fullname_input[100];
	INPUT_RESULT valid_entry = INPUT_RESULT_NO_INPUT;
	card_number_to_search_for[0] = 0;

	printf("\n---> You are here: [Access Card Control Panel / Edit Access Card]\n");
	printf("\n--- [INFO]: Place the card on the reader.\n");
	while (card_number_to_search_for[0] == 0) {
		SerialReadToNewLine(port, card_number_to_search_for, sizeof(card_number_to_search_for));
	}

	if (AccessCardExists(card_inventory_state, card_number_to_search_for)) {
		access_card_index = GetIndexOfAccessCard(card_inventory_state, card_number_to_search_for);
	} else {
		printf("\n--- [WARNING]: The card you searched for was not found");
		return false;
	}

	printf("--- [INFO]: Editing card %s\n", card_number_to_search_for);

	while (access_is_granted[0] != 'y' && access_is_granted[0] != 'n') {
		GetInput(
			"\n--- Does this card have access to unlock? (y/n) -> ",
			access_is_granted, sizeof(access_is_granted)
		);
	}

	while (valid_entry != INPUT_RESULT_OK) {
		valid_entry = GetInput("--- Enter the full name for this user -> ", 
			fullname_input, sizeof(fullname_input)
		);
	}

	strcpy(card_inventory_state->array_of_access_cards[access_card_index].fullname, fullname_input);

	if (access_is_granted[0] == 'y') {
		card_inventory_state->array_of_access_cards[access_card_index].access_level = ACCESS_GRANTED;
	} else if (access_is_granted[0] == 'n') {
		card_inventory_state->array_of_access_cards[access_card_index].access_level = ACCESS_DENIED;
	}
	return true;
}


void AccessCardControlPanel(SERIALPORT port, CARD_INVENTORY* card_inventory_state) {
	
	while (true) {
		int selection;

		printf("\n---> You are here: [Access Card Control Panel]\n");
		printf("\n1. Register card\n2. Edit access for existing card\n3. Back to Main Panel\n");

		if (!GetInputInt("\n-> ", &selection)) {
			continue;
		}

		switch (selection) {
		case 1:
			if (!RegisterNewAccessCardPanel(port, card_inventory_state)) {
				printf("--- [ERROR] - Unable to save new access card.\n\n");
			} else {
				printf("--- [INFO]: Data saved.\n\n");
			}
			break;
		case 2:
			if (!EditAccessCardPanel(port, card_inventory_state)) {
				printf("\n--- [ERROR] - Unable to edit access card.\n\n");
			} else {
				printf("\n--- [INFO]: Data saved, returning to control panel.\n\n");
			}
			break;
		case 3:
			return;
		}
	}
}


void DeployApprovedCards(SERIALPORT port, CARD_INVENTORY* card_inventory_state) {

	char protocol_tag[] = "<ADDCARD>";
	char feedback_buf[1024];
	char card_number_buff[200];
	char complete_command[200];
	short number_of_granted_access_cards = 0;

	for (int i = 0; i < card_inventory_state->number_of_access_cards; i++) {
		if (card_inventory_state->array_of_access_cards[i].access_level == ACCESS_GRANTED) {
			number_of_granted_access_cards++;
		}
	}

	if (card_inventory_state->number_of_access_cards < 1) {
		printf("--- [WARNING]: There are no cards in the system.");
		return;
	} else if (number_of_granted_access_cards < 1) {
		printf("--- [WARNING]: There are no cards with sufficient priviliges in the system to deploy.");
		return;
	}
	
	printf("\n--- [INFO]: Sending command to purge stored cards and deploying current inventory.\n");
	printf("\t    This will take a few seconds, please be patient.\r\n");
	SerialWritePort(port, "<CLEARALL>", sizeof("<CLEARALL>"));
	Sleep(ARDUINO_WAIT_TIME);

	for (int i = 0; i < card_inventory_state->number_of_access_cards; i++) {

		if (card_inventory_state->array_of_access_cards[i].access_level == ACCESS_GRANTED) {
			strncpy(card_number_buff, card_inventory_state->array_of_access_cards[i].number, sizeof(complete_command));
			strncpy(complete_command, protocol_tag, sizeof(complete_command));
			strncat(complete_command, card_number_buff, strlen(card_number_buff));
			SerialWritePort(port, complete_command, sizeof(complete_command));

			printf("--- [INFO]: Deploying %s...\n", card_inventory_state->array_of_access_cards[i].number);
			SerialReadToNewLine(port, feedback_buf, sizeof(feedback_buf));
			Sleep(ARDUINO_WAIT_TIME);
			printf("--- [SERIAL DEVICE]: %s\n\n", feedback_buf);
			Sleep(ARDUINO_WAIT_TIME);
		}
	}
}


void ListAllRemoteAccessCards(SERIALPORT port) {
	char feedback_buf[4096];
	SerialWritePort(port, "<LISTALL>", sizeof("<LISTALL>"));
	Sleep(250);
	SerialReadToEndOfTag(port, feedback_buf, sizeof(feedback_buf));
	printf("--- [SERIAL]: \n%s\n ", feedback_buf);
}


void MonitorMode(SERIALPORT port, CARD_INVENTORY* card_inventory_state) {

	char card_number_to_search_for[50];
	short index_of_accesscard;
	card_number_to_search_for[0] = 0;

	printf("\n---> You are here: [Access Card Control Panel / Monitor Mode]\n\n");
	printf("--- [INFO]: This mode is continuous and will monitor the RFID scanner in real time.\n");
	printf("            Unknown cards will not produce any output.\n");
	printf("            To quit: Press any key on the keyboard, then scan any card.\n\n");
	printf("\n--- [INFO]: Monitor mode initiated; Scan a tag or card to stream feedback.\n\n");
	
	while (true) {

		while (card_number_to_search_for[0] == 0) {
			Sleep(250);
			SerialReadToNewLine(port, card_number_to_search_for, sizeof(card_number_to_search_for));
			if (_kbhit() > 0) {
				return;
			}
		} 
		
		if (AccessCardExists(card_inventory_state, card_number_to_search_for)) {
			Sleep(250);
			index_of_accesscard = GetIndexOfAccessCard(card_inventory_state, card_number_to_search_for);
			printf("--- [SERIAL]: Card scanned: %s\n", card_number_to_search_for);
		
			if (card_inventory_state->array_of_access_cards[index_of_accesscard].access_level == ACCESS_GRANTED) {
				printf("--- [INFO]: Issued unlock command, door is unlocked for 3 seconds.\n");
				SerialWritePort(port, "<REMOTEOPEN>", sizeof("<REMOTEOPEN>"));
				Sleep(ARDUINO_WAIT_TIME);
				
				if (card_inventory_state->array_of_access_cards[index_of_accesscard].session_status == IS_SIGNED_IN) {
					card_inventory_state->array_of_access_cards[index_of_accesscard].session_status = IS_SIGNED_OUT;
					printf("--- [INFO]: %s just signed out, card number: %s\n", 
						card_inventory_state->array_of_access_cards[index_of_accesscard].fullname, 
						card_number_to_search_for
					);
				} else {
					card_inventory_state->array_of_access_cards[index_of_accesscard].session_status = IS_SIGNED_IN;
					printf("--- [INFO]: %s just signed in, card number: %s\n", 
						card_inventory_state->array_of_access_cards[index_of_accesscard].fullname, 
						card_number_to_search_for
					);
				}
			} else {
				printf("--- [INFO]: Access denied.\n");
			}
			printf("\n\n");
		}
		memset(card_number_to_search_for, 0, sizeof(card_number_to_search_for));
	}
}


short MainPanel(SERIALPORT port, CARD_INVENTORY* card_inventory_state) {
	int selection;

	while (true) {
		printf("\n---> You are here: [Main Panel]\n");
		printf("\n1. Access Card Control Panel\n2. Remote open door (3 seconds)\n");
		printf("3. List all access cards\n4. List all cards cached on RFID scanner\n5. Start monitor mode\n6. Quit");

		if (GetInputInt("\n-> ", &selection) == false) {
			continue;
		}
		switch (selection)	{
		case 1:
			AccessCardControlPanel(port, card_inventory_state);
			break;
		case 2:
			RemoteOpenDoor(port);
			break;
		case 3:
			ListAllAccessCards(card_inventory_state);			
			break;
		case 4:
			ListAllRemoteAccessCards(port);
			break;
		case 5:
			DeployApprovedCards(port, card_inventory_state);
			MonitorMode(port, card_inventory_state);
			break;
		case 6:
			return 0;
		}
	}
}


short main() {

	CARD_INVENTORY card_inventory_state;
	SERIALPORT port = SerialInit("\\\\.\\COM4");
	
	if (!SerialIsConnected(port)) {
		return;
	}

	card_inventory_state.array_of_access_cards = NULL;
	card_inventory_state.number_of_access_cards = 0;

	MainPanel(port, &card_inventory_state);
	return 0;
}
