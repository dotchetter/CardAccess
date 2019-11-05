#ifdef __cplusplus
extern "C" {
#endif
#include <time.h>
#include <stdbool.h>

typedef struct {
	char number[100];

} ACCESSCARD;

typedef struct {
	ACCESSCARD* array_of_access_cards;
	int number_of_access_cards;
}CARD_INVENTORY;

short GetIndexOfAccessCard(CARD_INVENTORY* card_inventory_state, char* card_number);
short GetIndexForNewAccessCard(CARD_INVENTORY* card_inventory_state);
bool AccessCardExists(CARD_INVENTORY* card_inventory_state, char* card_number);

#ifdef __cplusplus
}
#endif
