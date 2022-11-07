#include"Triangle.h"
#define DIRECTINPUT_VERSION 0x0800 //DirectInputのバージョン指定
#include<dinput.h>
#include<d3dcompiler.h>
#include<Windows.h>
#include <tchar.h>
#include<iostream>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cassert>
#include<vector>
#include<string>
#include<DirectXMath.h>
#include<DirectXTex.h>
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

using namespace DirectX;
using namespace std;
Triangle::Triangle()
{
	vertex1 = { {0.0f,0.0f,0.0f},{1.0f,1.0f} };
	vertex2 = { {0.0f,0.0f,0.0f},{1.0f,1.0f} };
	vertex3 = { {0.0f,0.0f,0.0f},{1.0f,1.0f} };
}
Triangle::Triangle(Vertex vertex1, Vertex vertex2, Vertex vertex3)
{
	this->vertex1=vertex1;
	this->vertex2=vertex2;
	this->vertex3=vertex3;
}
Triangle::~Triangle()
{

}
 void Triangle::Initialize(ID3D12Device* device)
{
	 Vertex vertices[] = {
		{vertex1},//左下
		{vertex2 },//左上
		{vertex3 },//右下
	 };
	 //頂点データ全体のサイズ=頂点データ一つ分のサイズ*頂点データの要素数
	 UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));
	 //頂点バッファの設定
	 D3D12_HEAP_PROPERTIES heapProp{};  //ヒープ設定
	 heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用
	 //リソース設定
	 D3D12_RESOURCE_DESC resDesc{};
	 resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	 resDesc.Width = sizeVB; //頂点データ全体のサイズ
	 resDesc.Height = 1;
	 resDesc.DepthOrArraySize = 1;
	 resDesc.MipLevels = 1;
	 resDesc.SampleDesc.Count = 1;
	 resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	 //頂点バッファの生成
	 ID3D12Resource* vertBuff = nullptr;
	 result = device->CreateCommittedResource(
		 &heapProp, //ヒープ設定
		 D3D12_HEAP_FLAG_NONE,
		 &resDesc, //リソース設定
		 D3D12_RESOURCE_STATE_GENERIC_READ,
		 nullptr,
		 IID_PPV_ARGS(&vertBuff));
	 assert(SUCCEEDED(result));
	 //インデックスデータ全体のサイズ
	 UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

	 //リソース設定
	 resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	 resDesc.Width = sizeIB;//インデックス情報が入る分のサイズ
	 resDesc.Height = 1;
	 resDesc.DepthOrArraySize = 1;
	 resDesc.MipLevels = 1;
	 resDesc.SampleDesc.Count = 1;
	 resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	 //インデックスバッファの生成
	 ID3D12Resource* indexBuff = nullptr;
	 result = device->CreateCommittedResource(
		 &heapProp,//ヒープ設定
		 D3D12_HEAP_FLAG_NONE,
		 &resDesc,
		 D3D12_RESOURCE_STATE_GENERIC_READ,
		 nullptr,
		 IID_PPV_ARGS(&indexBuff));
	 //インデックスバッファをマッピング
	 uint16_t* indexMap = nullptr;
	 result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	 //全インデックスに対して
	 for (int i = 0; i < _countof(indices); i++)
	 {
		 indexMap[i] = indices[i];//インデックスをコピー
	 }
	 //マッピング解除
	 indexBuff->Unmap(0, nullptr);
	 //GPU上のバッファに対応した仮想メモリ（メインメモリ上）を取得
	 Vertex* vertMap = nullptr;
	 result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	 assert(SUCCEEDED(result));
	 //前頂点に対して
	 for (int i = 0; i < _countof(vertices); i++) {
		 vertMap[i] = vertices[i]; //座標コピー
	 }
	 //繋がり解除
	 vertBuff->Unmap(0, nullptr);

	 //頂点バッファビューの作成
	 vbView;
	 //GPU仮想アドレス
	 vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	 //頂点バッファのサイズ
	 vbView.SizeInBytes = sizeVB;
	 //頂点一つ分のデータサイズ
	 vbView.StrideInBytes = sizeof(vertices[0]);

	 //インデックスバッファビューの作成
	 ibView;
	 ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	 ibView.Format = DXGI_FORMAT_R16_UINT;
	 ibView.SizeInBytes = sizeIB;

	 ID3DBlob* vsBlob = nullptr;//頂点シェーダーオブジェクト
	 ID3DBlob* psBlob = nullptr;//ピクセルシェーダーオブジェクト
	 ID3DBlob* errorBlob = nullptr;//エラーオブジェクト

	 //頂点シェーダーの読み込みとコンパイル
	 result = D3DCompileFromFile(
		 L"BasicVS.hlsl", //シェーダーファイル名
		 nullptr,
		 D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
		 "main", "vs_5_0",//エントリーポイント名、シェーダーモデル指定
		 D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //デバッグ用設定
		 0,
		 &vsBlob, &errorBlob);

	 //エラーなら
	 if (FAILED(result)) {
		 //errorBlobからエラー内容をstring型にコピー
		 std::string error;
		 error.resize(errorBlob->GetBufferSize());

		 std::copy_n((char*)errorBlob->GetBufferPointer(),
			 errorBlob->GetBufferSize(),
			 error.begin());
		 error += "\n";
		 //エラー内容を出力ウインドウに表示
		 OutputDebugStringA(error.c_str());
		 assert(0);
	 }
	 //ピクセルシェーダーの読み込みとコンパイル
	 result = D3DCompileFromFile(
		 L"BasicPS.hlsl",
		 nullptr,
		 D3D_COMPILE_STANDARD_FILE_INCLUDE,
		 "main", "ps_5_0",
		 D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		 0,
		 &psBlob, &errorBlob);
	 //エラーなら
	 if (FAILED(result)) {
		 //errorBlobから内容をstring型にコピー
		 std::string error;
		 error.resize(errorBlob->GetBufferSize());

		 std::copy_n((char*)errorBlob->GetBufferPointer(),
			 errorBlob->GetBufferSize(),
			 error.begin());
		 error += "\n";
		 //エラー内容を出力ウインドウに表示
		 OutputDebugStringA(error.c_str());
		 assert(0);
	 }
	 //頂点レイアウト
	 D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		 {
			 "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			 D3D12_APPEND_ALIGNED_ELEMENT,
			 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0,
			 },//一行で書いたほうが見やすい
			 {
			 "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
			 D3D12_APPEND_ALIGNED_ELEMENT,
			 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0,
			 },//一行で書いたほうが見やすい
	 };

	 //設定
	 D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	 //シェーダーの設定
	 pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	 pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	 pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	 pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	 //サンプルマスクの設定
	 pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	 //ラスタライザの設定
	 pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//カリングしない
	 pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//ポリゴン内塗りつぶし
	 pipelineDesc.RasterizerState.DepthClipEnable = true;//深度グリッピングを有効に
	 //ブレンドステート
	 //pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
	 //	= D3D12_COLOR_WRITE_ENABLE_ALL;//RBGAすべてのチャンネルを描画
	 //レンダーターゲットのブレンド設定
	 D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	 blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//RGBA全てのチャンネルを描画
	 blenddesc.BlendEnable = false;//ブレンドを有効にする
	 blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	 blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	 blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	 //加算合成
	 blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//加算
	 blenddesc.SrcBlend = D3D12_BLEND_ONE;//ソースの値を100%使う
	 blenddesc.DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う
	 ////減算合成
	 //blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;//デストからソースを減算
	 //blenddesc.SrcBlend = D3D12_BLEND_ONE;//ソースの値を100%使う
	 //blenddesc.DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う
	 ////色反転
	 //blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//加算
	 //blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1.0f-デストカラーの値
	 //blenddesc.DestBlend = D3D12_BLEND_ZERO;//使わない
	 ////半透明合成
	 //blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//加算
	 //blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースのアルファ値
	 //blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値
	 //頂点レイアウトの設定
	 pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	 pipelineDesc.InputLayout.NumElements = _countof(inputLayout);
	 //図形の形状設定
	 pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	 //その他の設定
	 pipelineDesc.NumRenderTargets = 1;//描画対象は一つ
	 pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0～255指定のRGBA
	 pipelineDesc.SampleDesc.Count = 1; //1ピクセルにつき一回サンプリング

	 //デスクリプタレンジの設定
	 D3D12_DESCRIPTOR_RANGE descriptorRange{};
	 descriptorRange.NumDescriptors = 1;//一度の描画に使うテクスチャが一枚なので１
	 descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	 descriptorRange.BaseShaderRegister = 0;//テクスチャレジスタ0番
	 descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	 //ルートパラメーターの設定
	 D3D12_ROOT_PARAMETER rootParams[2] = {};
	 rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファ
	 rootParams[0].Descriptor.ShaderRegister = 0;//定数バッファ番号
	 rootParams[0].Descriptor.RegisterSpace = 0;//デフォルト値
	 rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダーから見える
	 //テクスチャレジスタ0番
	 rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//種類
	 rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;//デスクリプタレンジ
	 rootParams[1].DescriptorTable.NumDescriptorRanges = 1;//デスクリプタレンジ数
	 rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダーから見える
	 //テクスチャサンプラー設定
	 D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	 samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//横繰り返し（タイリング）
	 samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//縦繰り返し（タイリング）
	 samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//奥行繰り返し（タイリング）
	 samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//ボーダーの時は黒
	 samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//全てリニア補完
	 samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;//ミップマップ最大値
	 samplerDesc.MinLOD = 0.0f;//ミップマップ最小値
	 samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	 samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダーからのみ使用可能
	 //ルートシグネチャ
	 rootSignature;
	 //ルートシグネチャの設定
	 D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	 rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	 rootSignatureDesc.pParameters = rootParams;//ルートパラメーターの先頭アドレス
	 rootSignatureDesc.NumParameters = _countof(rootParams);//ルートパラメータ数
	 rootSignatureDesc.pStaticSamplers = &samplerDesc;
	 rootSignatureDesc.NumStaticSamplers = 1;
	 //ルートシグネチャのシリアライズ
	 ID3DBlob* rootSigBlob = nullptr;
	 result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		 &rootSigBlob, &errorBlob);
	 assert(SUCCEEDED(result));
	 result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		 IID_PPV_ARGS(&rootSignature));
	 assert(SUCCEEDED(result));
	 rootSigBlob->Release();
	 //パイプラインにルートシグネチャをセット
	 pipelineDesc.pRootSignature = rootSignature.Get();

	 //パイプランスステートの生成
	 result = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	 assert(SUCCEEDED(result));
}
void Triangle::Draw(ID3D12GraphicsCommandList* commandList)
{
	//頂点バッファビューの設定コマンド
	commandList->IASetVertexBuffers(0, 1, &vbView);
	//インデックスバッファビューの設定コマンド
	commandList->IASetIndexBuffer(&ibView);
	//描画コマンド
	commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);//すべての頂点を使って描画
}
