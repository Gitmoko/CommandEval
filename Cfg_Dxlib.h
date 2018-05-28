#pragma once

#include<DxLib.h>
# include "Button.h"
#include"CSVReader.h"
#include<vector>
#include<iostream>
#include<fstream>
#include<assert.h>

#define update(x,y,f) f(x,PAD_INPUT_##y,y)
#define ins_key(x,f) functions.insert(std::pair < std::string, std::function<int()> > { #x, [=]()->int {return f(KEY_INPUT_##x); }});
#define ins_pad(x,f) functions.insert(std::pair < std::string, std::function<int()> > { #x, [=]()->int {return f(PAD_INPUT_##x); }});

namespace Cfg {
	inline std::vector<std::pair<int, std::function<int()>> >  Input_from_cfg_pad_DxLib(int pad, std::string file) {


		CSVReader::CFileRead cfg;
		if (cfg.FileRead(file.c_str(), ",") < 0) {
			return {};
		}
		if (GetJoypadNum() == 0) {
			return {};
		}

		std::vector<std::pair<int, std::function<int()>> > output;
		auto f = [pad]()->DINPUT_JOYSTATE {
			DINPUT_JOYSTATE ret;
			GetJoypadDirectInputState(pad, &ret);
			return ret;
		};
		auto g = [pad](int x)->bool {
			auto ret = GetJoypadInputState(pad);
			return (ret & x) != 0;
		};
		std::map<std::string, std::function<int()>> functions;
		functions.insert(std::pair < std::string, std::function<int()> > {"x+", [=]()->int {return f().X >= 600; } });
		functions.insert(std::pair < std::string, std::function<int()> > {"x-", [=]()->int {return f().X <= -600; } });
		functions.insert(std::pair < std::string, std::function<int()> > {"y+", [=]()->int {return f().Y >= 600; } });
		functions.insert(std::pair < std::string, std::function<int()> > {"y-", [=]()->int {return f().Y <= -600; } });
		functions.insert(std::pair < std::string, std::function<int()> > {"left", [=]()->int {return f().POV[0] == 27000; } });
		functions.insert(std::pair < std::string, std::function<int()> > {"right", [=]()->int {return f().POV[0] == 9000; } });
		functions.insert(std::pair < std::string, std::function<int()> > {"up", [=]()->int {return f().POV[0] == 0; } });
		functions.insert(std::pair < std::string, std::function<int()> > {"down", [=]()->int {return f().POV[0] == 18000; }});
		ins_pad(1, g);
		ins_pad(2, g);
		ins_pad(3, g);
		ins_pad(4, g);
		ins_pad(5, g);
		ins_pad(6, g);
		ins_pad(7, g);
		ins_pad(8, g);
		ins_pad(9, g);
		ins_pad(10, g);
		ins_pad(11, g);
		ins_pad(12, g);
		ins_pad(13, g);
		ins_pad(14, g);
		ins_pad(15, g);


		for (auto i = 0; i < cfg.GetSizeRow(); i++) {
			auto index = cfg.GetDatat<std::string>(i, 1);
			auto name = cfg.GetDatat<std::string>(i, 2);
			using tmp = decltype(output);
			output.push_back(tmp::value_type{ std::stoi(index), functions[name] });
		}
		return output;
	}

	/// <param name="ck">DxLibのDX_INPUT_*(*は任意のキーボードボタン)を入力したときにそのボタンが押されているかどうか返してください</param>
	inline std::vector<std::pair<int, std::function<int()>> >  Input_from_cfg_key_DxLib(std::string file, std::function<int(int)> ck) {


		CSVReader::CFileRead cfg;
		if (cfg.FileRead(file.c_str(), ",") < 0) {
			return {};
		}

		std::vector<std::pair<int, std::function<int()>> > output;


		for (auto i = 0; i < cfg.GetSizeRow(); i++) {
			auto index = cfg.GetDatat<std::string>(i, 1);
			auto keycode = std::stoi(cfg.GetDatat<std::string>(i, 2));
			using tmp = decltype(output);
			output.push_back(tmp::value_type{ std::stoi(index), [ck,keycode]() {
				return ck(keycode); }
			});
		}
		return output;
	}

	enum class  MODE { PAD, KEY };

	class CButtonMgr {
	public:
		using Player = int;

	public:
		char keybuf[256];
		std::map<Player, std::pair<MODE, CButton>> button;

	private:
		CButtonMgr(CButtonMgr&) {}
		CButtonMgr() {}

		void KeyUpdate() {
			GetHitKeyStateAll(keybuf);
		}
		int checkkey(int _x) {
			return keybuf[_x];
		}

	public:
		static CButtonMgr& getCButtonMgr() {
			static CButtonMgr s;
			return s;
		}

		int SetPlayerCfg_key(Player player, std::string file) {
			auto keys = Input_from_cfg_key_DxLib(file, [=](int _x) {return this->checkkey(_x); });
			if (keys.empty()) {
				return -1;
			}
			button.erase(player);
			button[player].first = MODE::KEY;
			button[player].second.Set_button_map(keys);
			return 0;
		}
		int SetPlayerCfg_pad(Player player, std::string file, int pad) {
			auto keys = Input_from_cfg_pad_DxLib(pad, file);
			if (keys.empty()) {
				return -1;
			}
			button.erase(player);
			button[player].first = MODE::PAD;
			button[player].second.Set_button_map(keys);
			return 0;
		}

		int Update() {
			KeyUpdate();
			for (auto& elem : button) {
				elem.second.second.Update();
			}
			return 0;
		}

		int GetFrame(int player, CButton::ButtonNumber _ButtonNum) {
			return button[player].second.Counter(_ButtonNum);
		}


	};


	std::function<bool(unsigned int)> GEN_Pressed_Func(int _player, bool* _facing) {
		return [&, _facing, _player](unsigned int arg) {
			unsigned int button_number = arg;
			if ((arg == LEFT_BUTTON || arg == RIGHT_BUTTON) && *_facing) {

				button_number = arg == RIGHT_BUTTON ? LEFT_BUTTON : RIGHT_BUTTON;
			}
			return CButtonMgr::getCButtonMgr().GetFrame(_player, button_number) > 0;
		};
	}

	std::function<bool(unsigned int)> GEN_Pressed_Func( int _player, std::function<bool()>_facing=nullptr) {
		return [&, _facing, _player](unsigned int arg) {
			unsigned int button_number = arg;
			if ((arg == LEFT_BUTTON || arg == RIGHT_BUTTON) && !_facing()) {

				button_number = arg == RIGHT_BUTTON ? LEFT_BUTTON : RIGHT_BUTTON;
			}
			return CButtonMgr::getCButtonMgr().GetFrame(_player, button_number) > 0;
		};
	}


}