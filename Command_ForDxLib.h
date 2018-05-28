#pragma once
#include<memory>
#include<unordered_map>
#include<functional>
#include"CmdParser.h"
//#define USELIB

#ifdef USELIB
#ifdef _DEBUG
#pragma comment(lib,"CmdForDxLib_d.lib")
#else
#pragma comment(lib,"CmdForDxLib.lib")
#endif
#endif



namespace  Command {


	class CEval;

	struct Command_ForDxLib {
	private:
		std::unique_ptr<CEval> pimpl;
	public:
		struct CommandData {
			std::string data = "";
			unsigned int margine = 0;
		};
	public:

		~Command_ForDxLib();
		using isPressedFunc = std::function<bool(unsigned int)>;
		Command_ForDxLib(isPressedFunc _isPressed);
		Command_ForDxLib(Command_ForDxLib &) = default;
		Command_ForDxLib(Command_ForDxLib &&) = default;
		void Update_Buffer();

		bool Eval_Command(std::string name);

		bool Load_file(std::string filename);

		std::vector<CommandData> getCommandList();

	};
}