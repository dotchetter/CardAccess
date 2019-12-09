#pragma once
#include <sstream>
#include <iostream>
#include <ctime>
#include <stdexcept>
#include <string>
#include <time.h>
#include <type_traits>
#include <map>
#include "accesslevel.h"

class AccessCard {
private:
	ACCESS_LEVEL access = ACCESS_LEVEL::NO_ACCESS;
	std::string id;
	time_t registered_timestamp;

public:
	AccessCard();
	AccessCard(std::string id);
	AccessCard(std::string id, ACCESS_LEVEL access)	;

	ACCESS_LEVEL get_access_level();
	void set_access_level(ACCESS_LEVEL access);
	std::string get_id() const;
	std::string get_registered_timestamp() const;
	std::string get_access_level_as_string() const;
};
