#pragma once

#include "CommandTree.h"
#include <unordered_map>

namespace Command {
	namespace Parser {
		struct CmdData {
			std::string name;
			Actions::Sequence action;
			unsigned time;
			~CmdData() {

			}
		};

		struct Cmd {
			std::unordered_map <std::string, unsigned> map;
			std::vector<CmdData> data;
		};

		struct parse_error {
			std::string pos;
			std::string what;
		};
	}
}