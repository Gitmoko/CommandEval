
# include <Siv3D.hpp>
# include "CommandEval.h"
# include "CmdParser.h"
# include "Button.h"
# include<iostream>
# include<fstream>


std::map<CButton::ButtonNumber, std::function<int()>> MakeButton_Key(std::vector<std::pair<CButton::ButtonNumber, s3d::Key>> arg) {
	std::map<CButton::ButtonNumber, std::function<int()>> output;
	for (auto & elem : arg) {
		auto code = elem.second.code;
		output[elem.first] = [=]()->int {return s3d::Key{ code }.pressed ? 1 : -1;};
	}
	return output;
}

std::vector<std::pair<int, std::function<int()>> >  Input_from_cfg_pad(int pad, std::string file) {


	s3d::CSVReader cfg(s3d::CharacterSet::Widen(file));
	if (!cfg) {
		return{};
	}

	std::vector<std::pair<int, std::function<int()>> > output;
	std::map<std::string, std::function<int()>> functions;

	functions.insert(std::pair < std::string, std::function<int()> > {"r+", [=]()->int {return s3d::Gamepad(pad).r >= 1; } });
	functions.insert(std::pair < std::string, std::function<int()> > {"r-", [=]()->int {return s3d::Gamepad(pad).r <= -1; } });
	functions.insert(std::pair < std::string, std::function<int()> > {"z+", [=]()->int {return s3d::Gamepad(pad).z >= 1; } });
	functions.insert(std::pair < std::string, std::function<int()> > {"z-", [=]()->int {return s3d::Gamepad(pad).z <= -1; } });
	functions.insert(std::pair < std::string, std::function<int()> > {"x+", [=]()->int {return s3d::Gamepad(pad).x >= 1; } });
	functions.insert(std::pair < std::string, std::function<int()> > {"x-", [=]()->int {return s3d::Gamepad(pad).x <= -1; } });
	functions.insert(std::pair < std::string, std::function<int()> > {"y+", [=]()->int {return s3d::Gamepad(pad).y >= 1; } });
	functions.insert(std::pair < std::string, std::function<int()> > {"y-", [=]()->int {return s3d::Gamepad(pad).y <= -1; } });
	functions.insert(std::pair < std::string, std::function<int()> > {"left", [=]()->int {return s3d::Gamepad(pad).povLeft.pressed; } });
	functions.insert(std::pair < std::string, std::function<int()> > {"right", [=]()->int {return s3d::Gamepad(pad).povRight.pressed; } });
	functions.insert(std::pair < std::string, std::function<int()> > {"up", [=]()->int {return s3d::Gamepad(pad).povForward.pressed; } });
	functions.insert(std::pair < std::string, std::function<int()> > {"down", [=]()->int {return s3d::Gamepad(pad).povBackward.pressed; } });
	for (int i = 0; i < s3d::Gamepad(pad)._get_num_buttons(); i++) {
		functions.insert(std::pair < std::string, std::function<int()> > { std::to_string(i), [=]()->int {return s3d::Gamepad(pad).button(i).pressed; }});
	}

	for (auto i = 0; i < cfg.rows; i++) {
		auto index = std::stoi(s3d::CharacterSet::Narrow(cfg.get<s3d::String>(i, 1)));
		auto name = s3d::CharacterSet::Narrow(cfg.get<s3d::String>(i, 2));
		using tmp = decltype(output);
		output.push_back(tmp::value_type{ index, functions[name] });
	}
	return output;
}

void Main()
{

	bool facing = true;


	CButton buttons;

	if (s3d::Gamepad(0).isConnected()) {
		buttons.Set_button_map(Input_from_cfg_pad(0, "pad.cfg"));
	}
	else {
		auto key_map = MakeButton_Key(
		{
			{ VDOWN_BUTTON, s3d::Input::KeyDown }
			,{ VLEFT_BUTTON, s3d::Input::KeyLeft }
			,{ VRIGHT_BUTTON, s3d::Input::KeyRight }
			,{ VUP_BUTTON, s3d::Input::KeyUp }
			,{ DOWN_BUTTON, s3d::Input::KeyS }
			,{ LEFT_BUTTON, s3d::Input::KeyA }
			,{ RIGHT_BUTTON, s3d::Input::KeyD }
			,{ UP_BUTTON, s3d::Input::KeyW }
			,{ SQUARE_BUTTON, s3d::Input::KeyV }
			,{ TRIANGLE_BUTTON, s3d::Input::KeyZ }
			,{ CIRCLE_BUTTON, s3d::Input::KeyX }
			,{ CROSS_BUTTON, s3d::Input::KeyC }
			,{ L1_BUTTON, s3d::Input::KeyY }
			,{ L2_BUTTON, s3d::Input::KeyR }
			,{ R1_BUTTON, s3d::Input::KeyT }
			,{ R2_BUTTON, s3d::Input::KeyU }
			,{ L3_BUTTON, s3d::Input::KeyF }
			,{ R3_BUTTON, s3d::Input::KeyH }
			,{ SELECT_BUTTON, s3d::Input::KeySpace }
			,{ PAUSE_BUTTON, s3d::Input::KeyShift }
		});
		buttons.Set_button_map(key_map);
	}
	auto pressed_func = [&](unsigned int arg) {
		if ((arg == VLEFT_BUTTON || arg == VRIGHT_BUTTON) && !facing) {
			//arg = arg == VRIGHT_BUTTON ? VLEFT_BUTTON : VRIGHT_BUTTON;
		}
		return buttons.Counter(arg) > 0; 
		};
	const Font font(30);
	auto cmdmgr = Command::CEval{ pressed_func };
	cmdmgr.Load_file("Text.txt");
	auto cmddata = cmdmgr.getCmddata();
	std::vector<std::pair<std::string,int>> history;
	int a = 0;
	int b = 0;
	while (System::Update())
	{
		buttons.Update();
		facing = s3d::Key{ s3d::Input::KeyLeft }.pressed ? false : s3d::Key{ s3d::Input::KeyRight }.pressed ? true : facing;
		cmdmgr.Update_Buffer();
		for (auto&elem : cmddata) {
			if (cmdmgr.Eval_Command(elem.first)) {
				history.push_back({ elem.first ,60});
			}
		}

		for (auto it = history.begin(), end = history.end(); it != end; it++) {
			font(s3d::CharacterSet::Widen(it->first)).draw(100+100.0/(-it->second*0.1 + 6.1), 100+std::distance(history.begin(),it)*20);
		}

		std::for_each(history.begin(), history.end(), [](decltype(history)::value_type& elem) {elem.second--; });
		auto newend = std::remove_if(history.begin(), history.end(), [](decltype(history)::value_type& elem) {return elem.second == 0; });
		history.erase(newend, history.end());

		font(L"ようこそ、Siv3D の世界へ！").draw();
		Circle(Mouse::Pos(), 50).draw({ 255, 0, 0, 127 });
	}
}
