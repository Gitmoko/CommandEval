
# include <DxLib.h>
# include "CommandEval.h"
# include "CmdParser.h"
# include "Cfg_Dxlib.h"
# include "Button.h"
# include<iostream>
# include<fstream>
# include<atlstr.h>

namespace Command {
	extern Command::Parser::Cmd cmd_parse_fromfile(const std::string& s);
}

struct Hadou {
	std::pair<double, double> dir;
	double x, y;
	int time = 0;
	bool exist = true;
	int col = GetColor(255, 255, 255);
	int pow = 1;

	Hadou(std::pair<double, double> _dir, int _col) : dir(_dir), col(_col) {}

	void Update() {
		if (!exist) {
			return;
		}

		time++;
		if (time > 60) {
			exist = false;
		}
		x += dir.first;
		y += dir.second;
	}

	void Draw() {
		if (!exist) {
			return;
		}
		DrawCircle(x, y,10, col, TRUE);
	}

};

struct CChara {
	bool facing = true;
	int x = 0;
	int y = 0;
	Command::CEval *cmgr;
	int col = GetColor(255, 255, 255);

	std::shared_ptr<Hadou> h;

	bool getFacing() { 
		return facing;
	}
	void Update() {

		if (cmgr->Eval_Command("F")) {
			x += 3 * (facing ? 1 : -1);
		}
		if (cmgr->Eval_Command("B")) {
			x -= 3 * (facing ? 1 : -1);;
			facing = !facing;
		}
		if (cmgr->Eval_Command("D")) {
			y += 3;
		}
		if (cmgr->Eval_Command("U")) {
			y -= 3;
		}

		if (cmgr->Eval_Command("Sinku")) {
			if (!h) {
				h = std::make_shared<Hadou>(std::pair<double, double>(facing ? 12 : -12, 0.0), col);
				h->x = x + (facing ? 20 : -20);
				h->y = y;
			}
		}

		if (cmgr->Eval_Command("Hadou")) {
			if (!h) {
				h = std::make_shared<Hadou>(std::pair<double, double>(facing ? 3.5 : -3.5, 0.0), col);
				h->x = x + (facing ? 20 : -20);
				h->y = y;
			}
		}
		if (cmgr->Eval_Command("ab")) {
			if (!h) {
				h = std::make_shared<Hadou>(std::pair<double, double>(0.,0.), col);
				h->x = x + (facing ? 50 : -50);
				h->y = y;
			}
		}

		if (h) {
			h->Update();
			if (!h->exist) {
				h.reset();
			}
		}
		
	}
	void Draw() {
		DrawCircle(x, y, 30, col);
		DrawString(x, y, CString(std::string(facing ? "右" : "左").c_str()).GetBuffer(),GetColor(0,0,0));

		if (h) {
			h->Draw();
		}
	}

};

std::function<bool(unsigned int)> GEN_Pressed_Func(Cfg::CButtonMgr& _mgr, std::function<bool()>_facing, int _player) {
	return [&,_facing,_player](unsigned int arg) {
		unsigned int button_number = arg;
		if ((arg == LEFT_BUTTON || arg == RIGHT_BUTTON) && !_facing()) {

			button_number = arg == RIGHT_BUTTON ? LEFT_BUTTON : RIGHT_BUTTON;
		}
		return _mgr.GetFrame(_player, button_number) > 0;
	};
}


CChara c1;
CChara c2;

void SceneUpdate(){
	c1.Update();
	c2.Update();
}

void SceneDraw() {

	c1.Draw();
	c2.Draw();
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE), DxLib_Init(), SetDrawScreen(DX_SCREEN_BACK); ///ウィンドウモード変更と初期化と裏画面設定

	auto&mgr = Cfg::CButtonMgr::getCButtonMgr();
	//mgr.SetPlayerCfg_pad(0, "pad_dxlib.cfg", DX_INPUT_PAD1);
	mgr.SetPlayerCfg_key(0, "key_dxlib.cfg");
	mgr.SetPlayerCfg_pad(1, "pad_dxlib.cfg", DX_INPUT_PAD2);

	auto pressed_func_p1 = GEN_Pressed_Func(mgr, [&]() {return c1.getFacing(); }, 0);
	auto cmdmgr1 = Command::CEval{ pressed_func_p1 };
	cmdmgr1.Load(Command::cmd_parse_fromfile("Text.txt"));
	auto cmddata1 = cmdmgr1.getCmddata();

	auto pressed_func_p2 = GEN_Pressed_Func(mgr, [&]() {return c2.getFacing(); }, 1);
	auto cmdmgr2 = Command::CEval{ pressed_func_p2 };
	cmdmgr2.Load(Command::cmd_parse_fromfile("Text.txt"));
	auto cmddata2 = cmdmgr2.getCmddata();

	c1.cmgr = &cmdmgr1;
	c1.x = 100;
	c1.y = 100;
	c1.col = GetColor(255, 100, 100);

	c2.cmgr = &cmdmgr2;
	c2.x = 300;
	c2.y = 100;
	c2.col = GetColor(100, 255, 100);

	while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0) {
		mgr.Update();
		cmdmgr1.Update_Buffer();
		cmdmgr2.Update_Buffer();

		SceneUpdate();
		SceneDraw();

		DrawString(0, 0, _T("test"), GetColor(255, 255, 255));
	}
	return 0;
}
