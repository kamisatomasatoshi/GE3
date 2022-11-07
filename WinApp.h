#pragma once
#include<Windows.h>
class WinApp
{
public://静的メンバ関数
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wapram, LPARAM lparam);
public://メンバ関数
	//初期化
	void Initialize();

	void Finalize();

	//メッセージの処理
	bool ProcessMessage();

	//getter
	HWND GetHwnd() const { return hwnd; }

	HINSTANCE GetInstance() const { return w.hInstance; }
public://定数
//ウィンドウ横幅
	static const int window_width = 1280;
	//ウィンドウ縦幅
	static const int window_heigit = 720;
private:
	//ウィンドウハンドル
	HWND hwnd = nullptr;
	//ウィンドウクラスの設定
	WNDCLASSEX w{};

};

