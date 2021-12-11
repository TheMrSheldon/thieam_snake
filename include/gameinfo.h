#pragma once

#include <inttypes.h>
#include <string>
#include <map>

struct GameInfo {
	std::string id;
	std::map<std::string, std::string> rules;
	uint32_t timeout;
	std::string source;
};