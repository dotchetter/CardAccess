#pragma once
#include <iostream>
#include <map>
#include "accesscard.h"

class AccessCardStorage {
private:
	std::map<std::string, AccessCard> card_table;
	AccessCard lookup;

public:
	AccessCardStorage();

	~AccessCardStorage();

	int size () const;
	
	void add_access_card(std::string card_id, ACCESS_LEVEL access_level);
	
	bool remove_access_card(std::string card_id);

	void assign_access(std::string card_id, ACCESS_LEVEL access_level);

	bool access_card_exists(std::string card_id);

	AccessCard get_access_card(std::string card_id);

	ACCESS_LEVEL validate_card_access(std::string card_id);
};