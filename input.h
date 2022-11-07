#pragma once
#define DIRECTINPUT_VERSION 0x0800 //DirectInputのバージョン指定
#include<dinput.h>
#include<windows.h>
#include<wrl.h>
#include"WinApp.h"

//入力
class Input
{
public:

	template <class T> using ComPtr= Microsoft::WRL::ComPtr<T>;

public://メンバ関数

	//初期化
	void Initialize(WinApp* winApp);
	//更新
	void Update();

	/// <summary>
	/// キーの押下をチェック
	/// </summary>
	///<param name="keynumber">キー番号(DIK_0等)</param>
	/// <returns>押されているか</returns>
	bool PushKey(BYTE keyNumber);
	/// <summary>
	/// キーのトリガーをチェック
	/// </summary>
	///<param name="keynumber">キー番号(DIK_0等)</param>
	/// <returns>トリガーか</returns>
	bool TriggerKey(BYTE keyNumber);
private://メイン変数
	//キーボードデバイス
	ComPtr<IDirectInputDevice8> keyboard = nullptr;

	ComPtr<IDirectInput8> directInput = nullptr;

	BYTE key[256] = {};

	BYTE keyPre[256] = {};

	//WindowAPI
	WinApp* winApp_ = nullptr;
};
