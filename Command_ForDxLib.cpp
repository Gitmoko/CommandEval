#include"Command_ForDxLib.h"
#include"CommandEval.h"

#ifndef USELIB

extern Command::Parser::Cmd cmd_parse_fromfile(const std::string& s);
namespace Command {
	Command_ForDxLib::Command_ForDxLib(isPressedFunc _isPressed) {
		pimpl.reset(new Command::CEval(_isPressed));
	}
	Command_ForDxLib::~Command_ForDxLib() {}

	bool Command_ForDxLib::Eval_Command(std::string arg) {
		auto ret = pimpl->Eval_Command(arg);
		return ret;

	}

	std::vector<Command_ForDxLib::CommandData> Command_ForDxLib::getCommandList() {
		std::vector<Command_ForDxLib::CommandData> ret;
		auto data = pimpl->getCmddata();
		for (auto& e : data) {
			visitors::printer p{};
			boost::apply_visitor(p,e.second.data);
			ret.push_back({p.out,e.second.margine});
		}
		return ret;
	}
	void Command_ForDxLib::Update_Buffer() {
		pimpl->Update_Buffer();		
	}

	bool Command_ForDxLib::Load_file(std::string arg) {
		pimpl->Load(cmd_parse_fromfile(arg));
		return true;
	}
}
#endif