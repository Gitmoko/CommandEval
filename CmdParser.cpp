
#pragma once
#include "MyParser_Boost_Cfg.h"
#include "CmdParser_impl.h"
namespace Command {
	Parser::Cmd cmd_parse(const std::string& s) {
		using namespace Command;
		using namespace Parser;
		auto ret = parse_impl(s);
		return ret;
	}
}
