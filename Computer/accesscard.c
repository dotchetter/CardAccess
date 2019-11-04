#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "accesscard.h"


short GetIndexOfAccessCard(CARD_INVENTORY* card_inventory_state, char* card_number) {
	for (short i = 0; i < card_inventory_state->number_of_access_cards; i++) {
		if (strcmp(card_inventory_state->array_of_access_cards[i].number, card_number) == 0) {
			return i;
		}
	}
}


short GetIndexForNewAccessCard(CARD_INVENTORY* card_inventory_state) {
	short new_card_index;

	if (card_inventory_state->number_of_access_cards == 0) {
		card_inventory_state->array_of_access_cards = malloc(sizeof(ACCESSCARD));
		card_inventory_state->number_of_access_cards = 1;
		new_card_index = 0;
	} else {
		card_inventory_state->array_of_access_cards = realloc(
			card_inventory_state->array_of_access_cards,
			sizeof(ACCESSCARD) * (card_inventory_state->number_of_access_cards + 1)
		);

		new_card_index = card_inventory_state->number_of_access_cards;
		card_inventory_state->number_of_access_cards++;
	}
	return new_card_index;
}


bool AccessCardExists(CARD_INVENTORY* card_inventory_state, char* card_number) {
	for (short i = 0; i < card_inventory_state->number_of_access_cards; i++) {
		if (strcmp(card_inventory_state->array_of_access_cards[i].number, card_number) == 0) {
			return true;
		}
	}
	return false;
}

