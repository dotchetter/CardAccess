#include "accesscard.h"
#include "accesscardstorage.h"
#include "accesslevel.h"


AccessCard::AccessCard() {
	time(&registered_timestamp);
}

AccessCard::AccessCard(std::string id) { 
	this->id = id;
}

AccessCard::AccessCard(std::string id, ACCESS_LEVEL access)	{
	this->id = id;
	this->access = access;
}

ACCESS_LEVEL AccessCard::get_access_level() {
	return access;
}

void AccessCard::set_access_level(ACCESS_LEVEL access) {
	this->access = access;
}

std::string AccessCard::get_id() const {
	return id;
}

std::string AccessCard::get_registered_timestamp() const {
	std::stringstream ss;
	ss << registered_timestamp;
	return ss.str();
}

std::string AccessCard::get_access_level_as_string() const {
	if (access == ACCESS_LEVEL::ACCESS) {
		return "Approved";
	}
	return "Denied";
}