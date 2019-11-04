#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "safeinput.h"
#include <stdbool.h>

typedef enum
{
	IS_SIGNED_IN,
	IS_SIGNED_OUT
}SESSION_STATUS;


typedef enum
{
	IS_UNLOCKED,
	IS_LOCKED
}DOORLOCK;


typedef struct
{
	DOORLOCK state
} MOCKUP_DOOR;


typedef struct
{
	time_t registration_timestamp;
	char number[100];
	bool has_access;
	SESSION_STATUS is_signed_in;

} ACCESSCARD;


typedef struct
{
	ACCESSCARD* array_of_access_cards;
	short number_of_access_cards;
}CARD_INVENTORY;


short GetIndexOfAccessCard(CARD_INVENTORY* card_inventory_state, char* card_number)
{
	for (short i = 0; i < card_inventory_state->number_of_access_cards; i++)
	{
		if (strcmp(card_inventory_state->array_of_access_cards[i].number, card_number) == 0)
		{
			return i;
		}
	}
}


short GetIndexForNewAccessCard(CARD_INVENTORY* card_inventory_state)
{
	short new_card_index;

	if (card_inventory_state->number_of_access_cards == 0)
	{
		card_inventory_state->array_of_access_cards = malloc(sizeof(ACCESSCARD));
		card_inventory_state->number_of_access_cards = 1;
		new_card_index = 0;
	}
	else
	{
		card_inventory_state->array_of_access_cards = realloc(
			card_inventory_state->array_of_access_cards,
			sizeof(ACCESSCARD) * (card_inventory_state->number_of_access_cards + 1)
		);

		new_card_index = card_inventory_state->number_of_access_cards;
		card_inventory_state->number_of_access_cards++;
	}
	return new_card_index;
}


bool AccessCardExists(CARD_INVENTORY* card_inventory_state, char* card_number)
{
	for (short i = 0; i < card_inventory_state->number_of_access_cards; i++)
	{
		if (strcmp(card_inventory_state->array_of_access_cards[i].number, card_number) == 0)
		{
			return true;
		}
	}
	return false;
}


bool RegisterNewAccessCardPanel(CARD_INVENTORY* card_inventory_state)
{
	time_t now;
	time(&now);

	short new_card_index;
	char new_card_number[50];
	char access_is_granted_input[5];

	printf("\n---> You are here: [Access Card Control Panel / Add a new access card]\n");

	GetInput("\n--- Enter Card number -> ", new_card_number, sizeof(new_card_number));

	if (AccessCardExists(card_inventory_state, new_card_number))
	{
		printf("--- [WARNING]: This card number is occupied.");
		return false;
	}
	else
	{
		new_card_index = GetIndexForNewAccessCard(card_inventory_state);
	}

	GetInput("--- Does this card have access to unlock? (Y or N) -> ",
		access_is_granted_input, sizeof(access_is_granted_input));

	if (access_is_granted_input[0] != 'N' && access_is_granted_input[0] != 'Y')
	{
		printf("\n--- [ERROR]: Invalid entry, try again\n");
		return false;
	}
	else if (access_is_granted_input[0] == 'Y')
	{
		card_inventory_state->array_of_access_cards[new_card_index].has_access = true;
	}
	else if (access_is_granted_input[0] == 'N')
	{
		card_inventory_state->array_of_access_cards[new_card_index].has_access = false;
	}

	card_inventory_state->array_of_access_cards[new_card_index].is_signed_in = IS_SIGNED_OUT;
	card_inventory_state->array_of_access_cards[new_card_index].registration_timestamp = now;
	strcpy(card_inventory_state->array_of_access_cards[new_card_index].number, new_card_number);

	if (AccessCardExists(card_inventory_state, new_card_number))
	{
		return true;
	}
	return false;
}


void ListAllAccessCards(CARD_INVENTORY* card_inventory_state)
{
	char temp_datetime_buffer[80];
	struct tm temp_timestruct;

	if (card_inventory_state->number_of_access_cards < 1)
	{
		printf("\n-- [WARNING]: There are no stored access cards.\n\n");
		return;
	}

	printf("\n-- Stored Access Cards Below:\n\n");
	for (int i = 0; i < card_inventory_state->number_of_access_cards; i++)
	{
		time_t rawtime = card_inventory_state->array_of_access_cards[i].registration_timestamp;
		temp_timestruct = *localtime(&rawtime);
		strftime(temp_datetime_buffer, sizeof(temp_datetime_buffer), "%a %Y-%m-%d %H:%M:%S", &temp_timestruct);


		printf(
			" * Card number: %s\n",
			card_inventory_state->array_of_access_cards[i].number
		);
		printf(" * Card has access to unlock: ");
		printf(card_inventory_state->array_of_access_cards[i].has_access ? "true\n" : "false\n");

		printf(" * Card is signed in: ");
		printf(card_inventory_state->array_of_access_cards[i].is_signed_in == IS_SIGNED_IN ? "true\n" : "false\n");

		printf("\n * Card added to system: %s\n", temp_datetime_buffer);
		printf("\n\n");
	}
}


void RemoteOpenDoor()
{
	unsigned long begin = time(NULL);
	unsigned long end;

	printf("--- [MANUAL OVERRIDE]: Door is unlocked\n");
	while (true)
	{
		end = time(NULL);
		if (end - begin >= 3)
		{
			printf("--- [MANUAL OVERRIDE]: Door is locked");
			break;
		}
	}
}


bool EditAccessCardPanel(CARD_INVENTORY* card_inventory_state)
{
	char access_is_granted_input[5];
	char card_number_to_search_for[100];
	short access_card_index;
	INPUT_RESULT valid_input;

	printf("\n---> You are here: [Access Card Control Panel / Edit Access Card]\n");

	valid_input = GetInput(
		"\n--- Enter Card number for the card to modify -> ",
		card_number_to_search_for,
		sizeof(card_number_to_search_for)
	);

	if (valid_input != INPUT_RESULT_OK)
	{
		printf("--- [WARNING]: Invalid entry, try again");
		return false;
	}

	if (AccessCardExists(card_inventory_state, card_number_to_search_for))
	{
		access_card_index = GetIndexOfAccessCard(card_inventory_state, card_number_to_search_for);
	}
	else
	{
		printf("--- [WARNING]: The card you searched for was not found\n");
		return false;
	}

	printf(
		"\n--- [INFO]: Card %s selected. ",
		card_inventory_state->array_of_access_cards[access_card_index].number
	);

	GetInput(
		"\n--- Does this card have access to unlock? (Y or N) -> ",
		access_is_granted_input, sizeof(access_is_granted_input)
	);

	if (access_is_granted_input[0] != 'N' && access_is_granted_input[0] != 'Y')
	{
		printf("\n---[ERROR]: Invalid entry\n");
		return false;
	}

	else if (access_is_granted_input[0] == 'Y')
	{
		card_inventory_state->array_of_access_cards[access_card_index].has_access = true;
	}
	else if (access_is_granted_input[0] == 'N')
	{
		card_inventory_state->array_of_access_cards[access_card_index].has_access = false;
	}
	return true;
}


void AccessCardControlPanel(CARD_INVENTORY* card_inventory_state)
{
	while (true)
	{
		int selection;

		printf("\n---> You are here: [Access Card Control Panel]\n");
		printf("\n1. Create new card\n2. Edit access for existing card\n3. Back to Main Panel\n");

		if (!GetInputInt("\n-> ", &selection))
			continue;

		switch (selection)
		{
		case 1:
			if (!RegisterNewAccessCardPanel(card_inventory_state))
			{
				printf("----[ERROR] - Unable to save new access card.\n\n");
			}
			else
			{
				printf("----[INFO]: Data saved.\n\n");
			}
			break;
		case 2:
			if (!EditAccessCardPanel(card_inventory_state))
			{
				printf("\n----[ERROR] - Unable to edit access card.\n\n");
			}
			else
			{
				printf("\n----[INFO]: Data saved, returning to control panel.\n\n");
			}
			break;
		case 3:
			return;
		}
	}
}


short MainPanel(CARD_INVENTORY* card_inventory_state)
{
	int selection;

	while (true)
	{
		printf("\n---> You are here: [Main Panel]\n");
		printf("\n1. Remote open door (3 seconds)\n2. Access Card Control Panel\n");
		printf("3. List all access cards\n4. Simulate card scan\n5. Quit\n");

		if (GetInputInt("\n-> ", &selection) == false)
			continue;
		switch (selection)
		{
		case 1:
			RemoteOpenDoor();
		case 2:
			AccessCardControlPanel(card_inventory_state);
			break;
		case 3:
			ListAllAccessCards(card_inventory_state);
			break;
		case 4:
			return 0;
		}
	}
}


short main()
{
	CARD_INVENTORY card_inventory_state;

	card_inventory_state.array_of_access_cards = NULL;
	card_inventory_state.number_of_access_cards = 0;
	MainPanel(&card_inventory_state);

	return 0;
}
