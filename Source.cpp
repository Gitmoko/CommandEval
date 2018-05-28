
# include <DxLib.h>
# include "Cfg_Dxlib.h"
# include "Command_ForDxLib.h"
# include<iostream>
# include<atlstr.h>

struct Chara {
	int x = 100;
	int y = 100;
	bool facing = true;


};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{

	ChangeWindowMode(TRUE), DxLib_Init(), SetDrawScreen(DX_SCREEN_BACK); ///ウィンドウモード変更と初期化と裏画面設定

	Chara chara;

	//mgr.SetPlayerCfg_pad(0, "pad_dxlib.cfg", DX_INPUT_PAD1);
	Cfg::CButtonMgr::getCButtonMgr().SetPlayerCfg_key(0, "key_dxlib.cfg");
	Cfg::CButtonMgr::getCButtonMgr().SetPlayerCfg_pad(1, "pad_dxlib.cfg", DX_INPUT_PAD2);

	Command::Command_ForDxLib cmdmgr{
		Cfg::GEN_Pressed_Func(0,&chara.facing) 
	};
	cmdmgr.Load_file("Text.txt");

	while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0) {
		Cfg::CButtonMgr::getCButtonMgr().Update();
		cmdmgr.Update_Buffer();


	}
	return 0;
}
