
#pragma once
#include "CmdParser_impl.h"
#include <fstream>
namespace Command {
	Parser::Cmd cmd_parse(const std::string& s) {
		using namespace Command;
		using namespace Parser;
		auto ret = parse_impl(s);
		return ret;
	}


	Parser::Cmd cmd_parse_fromfile(const std::string& file) {
		std::ifstream ifs(file);
		if (ifs.fail())
		{
			return Parser::Cmd{};
		}
		std::string str((std::istreambuf_iterator<char>(ifs)),
			std::istreambuf_iterator<char>());
		auto ret = cmd_parse(str);
		return ret;
	}
}
