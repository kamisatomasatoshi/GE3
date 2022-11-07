#pragma once
#include<d3d12.h>
#include<DirectXMath.h>
#include<wrl.h>

using namespace DirectX;

//定数バッファ用データ構造体（3D変換行列）
struct ConstBufferDataTransform
{
	XMMATRIX mat; //3D変換行列
};

class Object3D
{
public:
	void Initialize(ID3D12Device* device);

	void Update(XMMATRIX matView, XMMATRIX matProjection);

	void Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW vbView, D3D12_INDEX_BUFFER_VIEW ibView, UINT numIndices);

	//定数バッファ（行列用）
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffTransform;
	//定数バッファマップ（行列用）
	ConstBufferDataTransform* constMapTransform;
	//アフィン変換情報
	XMFLOAT3 scale = { 1,1,1 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 position = { 0,0,0 };
	//ワールド変換行列
	XMMATRIX matWorld;
	//親オブジェクトへのポインタ
	Object3D* parent = nullptr;

};