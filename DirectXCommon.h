#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<wrl.h>
#include"WinApp.h"
#include<vector>
class DirectXCommon
{
public:
	//������
	void Initialize(WinApp* winApp);
private:
	//�f�o�C�X�̐���
	void InitializeDevice();
	//�R�}���h�֘A�̏�����
	void InitializeCommand();
	//�X���b�v�`�F�[���̏�����
	void InitializeSwapchain();
	//�����_�[�^�[�Q�b�g�r���[�̏�����
	void InitializeRendarTagetView();
	// �[�x�o�b�t�@�̏�����
	void InitializeDepthBuffer();
	//�t�F���X�̏�����
	void InitializeFence();

private:

	HRESULT result;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	//DIrectX12�f�o�C�X
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	//DXGI�t�@�N�g��
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> comdAllocator = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	Microsoft::WRL::ComPtr <IDXGISwapChain4> swapChain;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap = nullptr;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers;

	//�t�F���X�̐���
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	UINT64 fenceVal = 0;

	//WindowAPI
	WinApp* winApp_ = nullptr;
};

