#include "WinApp.h"
#pragma comment(lib,"winmm.lib")

//ウィンドウプロージャー
LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wapram, LPARAM lparam) {
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{
		//ウインドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wapram, lparam);
}

void WinApp::Initialize()
{

	//ウインドウクラスの設定
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;//ウインドウプロ―ジャーの設定
	w.lpszClassName = L"DirectXGame";//ウインドウクラス名
	w.hInstance = GetModuleHandle(nullptr);//ウインドウハンドル
	w.hCursor = LoadCursor(NULL, IDC_ARROW);//カーソル指定

	//ウインドウクラスをOSに登録する
	RegisterClassEx(&w);
	//ウインドウサイズ｛X座標　Y座標　横幅　縦幅｝
	RECT wrc = { 0,0,window_width,window_heigit };
	//自動サイズを補正する
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウインドウオブジェクトの生成
	hwnd = CreateWindow(w.lpszClassName,//クラス名
		L"DirectXGame",//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,//標準的なウインドウスタイル
		CW_USEDEFAULT,//表示X座標（OSに任せる）
		CW_USEDEFAULT,//表示Y座標（OSに任せる）
		wrc.right - wrc.left,//ウインドウ横幅
		wrc.bottom - wrc.top,//ウインドウ縦幅
		nullptr,//親ウインドウハンドル
		nullptr,//メニューハンドル
		w.hInstance,//呼び出しアプリケーションハンドル
		nullptr);//オプション

	//ウインドウを表示状態にする
	ShowWindow(hwnd, SW_SHOW);

	//システムタイマーの分解能を上げる
	timeBeginPeriod(1);
}

void WinApp::Finalize()
{
	//ウインドウクラスを登録解除
	UnregisterClass(w.lpszClassName, w.hInstance);
}

bool WinApp::ProcessMessage()
{
	MSG msg{};

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT)
	{
		return true;
	}

	return false;
}
