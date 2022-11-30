#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<wrl.h>
#include<vector>
#include"WinApp.h"
#include<chrono>

class DirectXCommon
{
public://メンバ関数
	//初期化
	void Initialize(WinApp* WinApp);

	void InitializeDevice();

	void InitializeCommand();

	void InitializeSwapchain();

	void InitializeRenderTargetView();

	void InitializeDepthBuffer();

	void IntializeFence();

	void PreDraw();

	void PostDrow();

	//デバイス取得
	ID3D12Device* GetDevice() const { return device.Get(); }

	//コマンドリスト取得
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }
private://メンバ関数
	//FPS固定初期化
	void InitializeFixFPS();

	//FPS固定更新
	void UpdateFixFPS();

private:

	HRESULT result;
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> comdAllocator = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap = nullptr;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuff = nullptr;

	UINT64 fenceVal = 0;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	D3D12_RESOURCE_BARRIER barrierDesc{};

	//記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_;

	//WindowsAPI
	WinApp* winApp_ = nullptr;
};

