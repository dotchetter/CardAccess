#include "accesscardstorage.h"


AccessCardStorage::AccessCardStorage() {}

AccessCardStorage::~AccessCardStorage() {
	card_table.clear();
}

int AccessCardStorage::size () const {
	return card_table.size();
}


void AccessCardStorage::add_access_card(std::string card_id, ACCESS_LEVEL access_level) {
	card_table[card_id] = AccessCard(card_id, access_level);
}

bool AccessCardStorage::remove_access_card(std::string card_id) {
	if (card_table.find(card_id) == end(card_table)) {
		return false;
	}
	card_table.erase(card_id);
	return true;
}

void AccessCardStorage::assign_access(std::string card_id, ACCESS_LEVEL access_level) {
	AccessCard &card = card_table.at(card_id);
	card.set_access_level(access_level);
}

bool AccessCardStorage::access_card_exists(std::string card_id) {
	if (card_table.count(card_id) > 0) {
		return true;
	}
	return false;
}

AccessCard AccessCardStorage::get_access_card(std::string card_id) {
	return card_table[card_id];
}

ACCESS_LEVEL AccessCardStorage::validate_card_access(std::string card_id) {
	if (card_table.find(card_id) == end(card_table)) {
		return ACCESS_LEVEL::NO_ACCESS;
	}
	return card_table.at(card_id).get_access_level();
}
