#include <time.h>
#include <stdbool.h>
#pragma once

typedef enum {
	IS_SIGNED_IN,
	IS_SIGNED_OUT
}SESSION_STATUS;


typedef enum {
	ACCESS_GRANTED,
	ACCESS_DENIED
}CARD_ACCESS_LEVEL;


typedef struct {
	time_t registration_timestamp;
	CARD_ACCESS_LEVEL access_level;
	SESSION_STATUS session_status;
	char number[100];
	char fullname[100];

} ACCESSCARD;


typedef struct {
	ACCESSCARD* array_of_access_cards;
	short number_of_access_cards;
}CARD_INVENTORY;


short GetIndexOfAccessCard(CARD_INVENTORY* card_inventory_state, char* card_number);
short GetIndexForNewAccessCard(CARD_INVENTORY* card_inventory_state);
bool AccessCardExists(CARD_INVENTORY* card_inventory_state, char* card_number);