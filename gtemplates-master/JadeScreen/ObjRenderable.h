#pragma once
#include <d3dcompiler.h>
#include "../Gateware/Gateware.h"
#include "../JadeScreen/Tex/include/DirectXTex.h"
#if _DEBUG
#pragma comment(lib, "../Tex/lib/DirectXTex.lib")
#else
#pragma comment(lib, "../Tex/lib/Release/DirectXTex.lib")
#endif
#pragma comment(lib, "d3dcompiler.lib")
#include "d3dx12.h" // official helper file provided by microsoft

#include <DirectXMath.h>
#include "DDSTextureLoader12.h"
#include <vector>
#include <thread>
#include <mutex>

using namespace DirectX;
using namespace GW;
using namespace GRAPHICS;

struct ConstantBuffer {
	XMMATRIX w, v, p;
	XMVECTOR lightPos[2];
	XMVECTOR lightColor[2];
	XMVECTOR lightDir[1];
	XMVECTOR worldCamera;
};

struct ThreadData {
	std::mutex mutex;
	std::condition_variable cv;
};
template<class Vertex>
class ObjRenderable {
protected:
	Microsoft::WRL::ComPtr<ID3D12Resource>			vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource>			indexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource>			textureBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource>			textureBufferUploadHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource>			constantBufferUploadHeap;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		pipeline;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		pipelineCCW;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		rootSignature;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>	mainDescriptorHeap;
	
	ID3D12Device* device;
	GW::GRAPHICS::GDirectX12Surface d3d;

	ConstantBuffer				cbVars;
	UINT8*						cbGPUAddress;
	UINT						cbSize;
	ID3D12CommandAllocator*		allocator;
	ID3D12GraphicsCommandList*	commandList;

	UINT64 GetConstantBufferSize(ConstantBuffer cb) {
		int alignment = 64 * 1024;
		int min = sizeof(cb) / alignment;

		return alignment * (1 + min);
	}

public:
	D3D12_VERTEX_BUFFER_VIEW						vertexView;
	D3D12_INDEX_BUFFER_VIEW							indexView;
	bool hasTexture = false;
	bool ccw = false;
	int indexCount = 0;
	int vertexCount = 0;

	ObjRenderable(ID3D12Device* _device, GW::GRAPHICS::GDirectX12Surface _d3d);
	~ObjRenderable();
	void InitResources(const Vertex data[], const unsigned int indeces[], const unsigned int vertexSize, const unsigned int indexSize);
	void CreatePipeline(Microsoft::WRL::ComPtr<ID3DBlob> vertexShader, Microsoft::WRL::ComPtr<ID3DBlob> pixelShader);
	ID3D12GraphicsCommandList* LoadTexture(const wchar_t* filename);
	ID3D12GraphicsCommandList* LoadTexture3D(const wchar_t* filename);
	void ThreadedLoadTexture(const wchar_t* filename, ThreadData* data);
	void Draw(ID3D12GraphicsCommandList* cmd);
	void ThreadedDraw(ThreadData* data);

	void											ControlObject(int left, int right, int up, int down, float distance);

	void											SetWorldMatrix(XMMATRIX world);
	void											SetViewMatrix(XMMATRIX view);
	void											SetProjectionMatrix(XMMATRIX projection);
	void											SetLightPosition(int index, XMVECTOR position);
	void											SetLightColor(int index, XMVECTOR color);
	void											SetLightDirection(int index, XMVECTOR direction);
	void											SetWorldCamera(XMVECTOR cameraPosition);
	XMMATRIX										GetWorldMatrix();
	XMMATRIX										GetViewMatrix();
	XMMATRIX										GetProjectionMatrix();
	ConstantBuffer									GetConstantBuffer();

	Microsoft::WRL::ComPtr<ID3D12Resource>			GetConstantBufferUpload();
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		GetPipeline();
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		GetRootSignature();
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>	GetDescriptorHeap();
	CD3DX12_GPU_DESCRIPTOR_HANDLE					GetGPUCBHandle();
	CD3DX12_GPU_DESCRIPTOR_HANDLE					GetGPUSRHandle();
	D3D12_PRIMITIVE_TOPOLOGY						GetTopology();
};

template<class Vertex>
ObjRenderable<Vertex>::ObjRenderable(ID3D12Device* _device, GW::GRAPHICS::GDirectX12Surface _d3d) {
	device = _device;
	d3d = _d3d;
	cbGPUAddress = nullptr;
	cbVars.w, cbVars.v, cbVars.p = XMMatrixIdentity();
	for (size_t i = 0; i < _countof(cbVars.lightColor); i++)
	{
		cbVars.lightColor[i] = XMVectorSet(0, 0, 0, 1);
	}
	for (size_t i = 0; i < _countof(cbVars.lightPos); i++)
	{
		cbVars.lightPos[i] = XMVectorSet(0, 0, 0, -1);
	}
	for (int i = 0; i < _countof(cbVars.lightDir); i++)
	{
		cbVars.lightDir[i] = XMVectorSet(0, 0, 0, -1);
	}
}

template<class Vertex>
ObjRenderable<Vertex>::~ObjRenderable() {
	allocator->Release();
	commandList->Release();
}

template<class Vertex>
void ObjRenderable<Vertex>::InitResources(const Vertex data[], const unsigned int indeces[], const unsigned int vertexSize, const unsigned int indexSize)
{
	HRESULT hr;

	//Vertex Buffer
	hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(data[0]) * vertexSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBuffer)
	);

	//Transfer vertices to the buffer
	UINT8* vertexMemLocation;
	vertexBuffer->Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&vertexMemLocation));
	memcpy(vertexMemLocation, data, sizeof(data[0]) * vertexSize);
	vertexBuffer->Unmap(0, nullptr);

	//Vertex View
	vertexView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexView.StrideInBytes = sizeof(Vertex);
	vertexView.SizeInBytes = sizeof(data[0]) * vertexSize;

	vertexCount = vertexSize;

	//Index Buffer
	hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(indeces[0]) * indexSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuffer)
	);

	//Transfer indeces to the buffer
	UINT8* indexMemLocation;
	indexBuffer->Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&indexMemLocation));
	memcpy(indexMemLocation, indeces, sizeof(indeces[0]) * indexSize);
	indexBuffer->Unmap(0, nullptr);

	//Index View
	indexView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexView.Format = DXGI_FORMAT_R32_UINT;
	indexView.SizeInBytes = sizeof(indeces[0]) * indexSize;

	indexCount = indexSize;

	//Constant Buffer
	hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constantBufferUploadHeap)
	);

	cbVars.w = XMMatrixIdentity();
	cbVars.v = XMMatrixIdentity();
	cbVars.p = XMMatrixIdentity();
	constantBufferUploadHeap->Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&cbGPUAddress));
	memcpy(cbGPUAddress, &cbVars, sizeof(cbVars));
}

template<class Vertex>
void ObjRenderable<Vertex>::CreatePipeline(Microsoft::WRL::ComPtr<ID3DBlob> vertexShader,
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShader)
{
	D3D12_INPUT_ELEMENT_DESC format[] = {
			{
				"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			},
			{
				"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			},
			{
				"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			}
	};

	if (!rootSignature)
	{
		D3D12_STATIC_SAMPLER_DESC sampler = {};
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.MipLODBias = 0;
		sampler.MaxAnisotropy = 0;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sampler.MinLOD = 0.0f;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;
		sampler.ShaderRegister = 0;
		sampler.RegisterSpace = 0;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		const int numRanges = 2;

		CD3DX12_DESCRIPTOR_RANGE1 descRanges[numRanges] = {};
		descRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
		descRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		CD3DX12_ROOT_PARAMETER1 rootParameter[numRanges] = {};
		rootParameter[0].InitAsDescriptorTable(1, &descRanges[0], D3D12_SHADER_VISIBILITY_ALL);
		rootParameter[1].InitAsDescriptorTable(1, &descRanges[1], D3D12_SHADER_VISIBILITY_ALL);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc;
		rootSigDesc.Init_1_1(_countof(rootParameter), rootParameter, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		Microsoft::WRL::ComPtr<ID3DBlob> signature, errors;
		D3DX12SerializeVersionedRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1_1, signature.GetAddressOf(), errors.GetAddressOf());
		device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	}

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 2;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&mainDescriptorHeap));

	cbSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);	

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc[1] = {};
	cbvDesc[0].BufferLocation = constantBufferUploadHeap->GetGPUVirtualAddress();
	cbvDesc[0].SizeInBytes = (sizeof(ConstantBuffer) + 255) & ~255;

	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(mainDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), 0, cbSize);
	device->CreateConstantBufferView(&cbvDesc[0], cpuHandle);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psDesc;
	ZeroMemory(&psDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psDesc.InputLayout = { format, ARRAYSIZE(format) };
	psDesc.pRootSignature = rootSignature.Get();
	psDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	psDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	psDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // a default depth stencil state
	psDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psDesc.SampleMask = UINT_MAX;
	psDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psDesc.NumRenderTargets = 1;
	psDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psDesc.SampleDesc.Count = 1;
	device->CreateGraphicsPipelineState(&psDesc, IID_PPV_ARGS(&pipeline));

	psDesc.InputLayout = { format, ARRAYSIZE(format) };
	psDesc.pRootSignature = rootSignature.Get();
	psDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	psDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());


	CD3DX12_RASTERIZER_DESC temp = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	temp.FrontCounterClockwise = true;

	psDesc.RasterizerState = temp;
	psDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // a default depth stencil state
	psDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psDesc.SampleMask = UINT_MAX;
	psDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psDesc.NumRenderTargets = 1;
	psDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psDesc.SampleDesc.Count = 1;
	device->CreateGraphicsPipelineState(&psDesc, IID_PPV_ARGS(&pipelineCCW));
}

template<class Vertex>
ID3D12GraphicsCommandList* ObjRenderable<Vertex>::LoadTexture(const wchar_t* filename)
{
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator));
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, nullptr, IID_PPV_ARGS(&commandList));
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	std::unique_ptr<uint8_t[]> dddata;

	HRESULT hr;

	hr = LoadDDSTextureFromFile(device, filename, textureBuffer.GetAddressOf(), dddata, subresources);

	UINT64 texUploadSize = GetRequiredIntermediateSize(textureBuffer.Get(), 0, subresources.size());

	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(texUploadSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(textureBufferUploadHeap.GetAddressOf())
	);

	UpdateSubresources(commandList, textureBuffer.Get(), textureBufferUploadHeap.Get(), 0, 0, subresources.size(), subresources.data());
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(textureBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	auto desc = textureBuffer->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC shaderResource = {};
	shaderResource.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	shaderResource.Format = desc.Format;

	if (subresources.size() == 1)
	{
		shaderResource.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		shaderResource.Texture2D.MipLevels = 1;
	}
	else
	{
		shaderResource.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		shaderResource.TextureCube.MipLevels = desc.MipLevels;
		shaderResource.TextureCube.MostDetailedMip = 0;
		shaderResource.TextureCube.ResourceMinLODClamp = 0;
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(mainDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), 1, cbSize);
	device->CreateShaderResourceView(textureBuffer.Get(), &shaderResource, srvHandle);

	commandList->Close();

	hasTexture = true;

	return commandList;

	//TexMetadata metadata;
	//ScratchImage image;
	//HRESULT hr = LoadFromDDSFile(filename, DDS_FLAGS::DDS_FLAGS_FORCE_RGB, &metadata, image);

	//std::vector<UINT8> texture;
	//for (size_t i = 0; i < image.GetPixelsSize(); i++)
	//{
	//	texture.push_back(image.GetPixels()[i]);
	//}

	//D3D12_RESOURCE_DIMENSION dimension;
	//if (metadata.dimension == 2)
	//{
	//	dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
	//} 
	//else if (metadata.dimension == 3)
	//{
	//	dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//}
	//else {
	//	dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
	//}

	//D3D12_RESOURCE_DESC textureResDesc = {};
	//textureResDesc.Dimension = dimension;
	//textureResDesc.Alignment = 0;
	//textureResDesc.Width = metadata.width;
	//textureResDesc.Height = metadata.height;
	//textureResDesc.DepthOrArraySize = metadata.arraySize;
	//textureResDesc.MipLevels = metadata.mipLevels;
	//textureResDesc.Format = metadata.format;
	//textureResDesc.SampleDesc.Count = 1;
	//textureResDesc.SampleDesc.Quality = 0;
	//textureResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	//textureResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	//device->CreateCommittedResource(
	//	&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
	//	D3D12_HEAP_FLAG_NONE,
	//	&textureResDesc,
	//	D3D12_RESOURCE_STATE_COPY_DEST,
	//	nullptr,
	//	IID_PPV_ARGS(&textureBuffer)
	//);

	//UINT64 texUploadSize = GetRequiredIntermediateSize(textureBuffer.Get(), 0, 1);

	//device->CreateCommittedResource(
	//	&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
	//	D3D12_HEAP_FLAG_NONE,
	//	&CD3DX12_RESOURCE_DESC::Buffer(texUploadSize),
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&textureBufferUploadHeap)
	//);

	//D3D12_SUBRESOURCE_DATA textureData = {};
	//textureData.pData = texture.data();
	//textureData.RowPitch = image.GetImage(0, 0, 0)->rowPitch;
	//textureData.SlicePitch = image.GetImage(0, 0, 0)->slicePitch;

	////commandList->Reset(allocator, pipeline.Get());
	//UpdateSubresources(commandList, textureBuffer.Get(), textureBufferUploadHeap.Get(), 0, 0, 1, &textureData);
	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(textureBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
}

template<class Vertex>
inline ID3D12GraphicsCommandList* ObjRenderable<Vertex>::LoadTexture3D(const wchar_t* filename)
{
	Microsoft::WRL::ComPtr<ID3D12Resource> tempResource;
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	std::unique_ptr<uint8_t[]> dddata;

	HRESULT hr;

	hr = LoadDDSTextureFromFile(device, filename, textureBuffer.GetAddressOf(), dddata, subresources);

	UINT64 texUploadSize = GetRequiredIntermediateSize(textureBuffer.Get(), 0, 6);

	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(texUploadSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(textureBufferUploadHeap.GetAddressOf())
	);

	UpdateSubresources(commandList, textureBuffer.Get(), textureBufferUploadHeap.Get(), 0, 0, 6, subresources.data());
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(textureBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	auto desc = textureBuffer->GetDesc();

	

	D3D12_SHADER_RESOURCE_VIEW_DESC shaderResource = {};
	shaderResource.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	shaderResource.Format = desc.Format;
	shaderResource.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	shaderResource.TextureCube.MipLevels = desc.MipLevels;
	shaderResource.TextureCube.MostDetailedMip = 0;
	shaderResource.TextureCube.ResourceMinLODClamp = 0;


	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(mainDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), 1, cbSize);
	device->CreateShaderResourceView(textureBuffer.Get(), &shaderResource, srvHandle);

	//TexMetadata metadata;
	//ScratchImage image;
	//hr = LoadFromDDSFile(filename, DDS_FLAGS::DDS_FLAGS_FORCE_RGB, &metadata, image);

	//std::vector<UINT8> texture;
	//size_t imagesize = image.GetPixelsSize();
	//for (size_t i = 0; i < image.GetPixelsSize(); i++)
	//{
	//	texture.push_back(image.GetPixels()[i]);
	//}

	//D3D12_RESOURCE_DESC textureResDesc = {};
	//textureResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//textureResDesc.Alignment = 0;
	//textureResDesc.Width = metadata.width;
	//textureResDesc.Height = metadata.height;
	//textureResDesc.DepthOrArraySize = metadata.arraySize;
	//textureResDesc.MipLevels = metadata.mipLevels;
	//textureResDesc.Format = metadata.format;
	//textureResDesc.SampleDesc.Count = 1;
	//textureResDesc.SampleDesc.Quality = 0;
	//textureResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	//textureResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;


	//device->CreateCommittedResource(
	//	&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
	//	D3D12_HEAP_FLAG_NONE,
	//	&textureResDesc,
	//	D3D12_RESOURCE_STATE_COPY_DEST,
	//	nullptr,
	//	IID_PPV_ARGS(&textureBuffer)
	//);

	//UINT64 texUploadSize = GetRequiredIntermediateSize(textureBuffer.Get(), 0, 1);

	//device->CreateCommittedResource(
	//	&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
	//	D3D12_HEAP_FLAG_NONE,
	//	&CD3DX12_RESOURCE_DESC::Buffer(texUploadSize),
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&textureBufferUploadHeap)
	//);

	//D3D12_SUBRESOURCE_DATA textureData = {};
	//textureData.pData = texture.data();
	//textureData.RowPitch = image.GetImage(0, 0, 0)->rowPitch;
	//textureData.SlicePitch = image.GetImage(0, 0, 0)->slicePitch;

	//UpdateSubresources(commandList, textureBuffer.Get(), textureBufferUploadHeap.Get(), 0, 0, 1, &textureData);
	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(textureBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	//D3D12_SHADER_RESOURCE_VIEW_DESC shaderResource = {};
	//shaderResource.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//shaderResource.Format = metadata.format;
	//shaderResource.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	//shaderResource.TextureCube.MipLevels = metadata.mipLevels;
	//shaderResource.TextureCube.MostDetailedMip = 0;
	//shaderResource.TextureCube.ResourceMinLODClamp = 0;

	//CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(mainDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), 1, cbSize);
	//device->CreateShaderResourceView(textureBuffer.Get(), &shaderResource, srvHandle);

	commandList->Close();

	hasTexture = true;

	return commandList;
}

template<class Vertex>
inline void ObjRenderable<Vertex>::ThreadedLoadTexture(const wchar_t* filename, ThreadData* data)
{
	Microsoft::WRL::ComPtr<ID3D12CommandList> command[] = { LoadTexture(filename) };
	ID3D12CommandQueue* commandQueue;
	data->mutex.lock();
	d3d.GetCommandQueue((void**)&commandQueue);
	commandQueue->ExecuteCommandLists(1, command->GetAddressOf());
	commandQueue->Release();
	data->mutex.unlock();
	//commandList->Reset(allocator, nullptr);
}

template<class Vertex>
void ObjRenderable<Vertex>::Draw(ID3D12GraphicsCommandList* cmd)
{
	cmd->SetGraphicsRootSignature(rootSignature.Get());
	
	ID3D12DescriptorHeap* descriptorHeaps[] = { mainDescriptorHeap.Get() };
	cmd->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(mainDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), 0, cbSize);
	CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(mainDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), 1, cbSize);
	
	cmd->SetGraphicsRootDescriptorTable(0, cbvHandle);
	if (hasTexture)
		cmd->SetGraphicsRootDescriptorTable(1, srvHandle);
	if (ccw)
		cmd->SetPipelineState(pipelineCCW.Get());
	else
		cmd->SetPipelineState(pipeline.Get());
	cmd->IASetVertexBuffers(0, 1, &vertexView);
	cmd->IASetIndexBuffer(&indexView);
	cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmd->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
	//return commandList;
}

template<class Vertex>
inline void ObjRenderable<Vertex>::ThreadedDraw(ThreadData* data)
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtv;
	D3D12_CPU_DESCRIPTOR_HANDLE dsv;
	ID3D12CommandQueue* commandQueue;
	d3d.GetCurrentRenderTargetView((void**)&rtv);
	d3d.GetDepthStencilView((void**)&dsv);
	commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
	Draw(commandList);
	commandList->Close();
	Microsoft::WRL::ComPtr<ID3D12CommandList> command[] = { commandList };
	data->mutex.lock();
	d3d.GetCommandQueue((void**)&commandQueue);
	commandQueue->ExecuteCommandLists(1, command->GetAddressOf());
	commandQueue->Release();
	data->mutex.unlock();
}

template<class Vertex>
inline void ObjRenderable<Vertex>::ControlObject(int left, int right, int up, int down, float distance)
{
	XMMATRIX movement = XMMatrixInverse(nullptr, cbVars.w);
	if (GetKeyState(left) & 0x8000)
		movement.r[3] += (movement.r[0] * distance);
	if (GetKeyState(right) & 0x8000)
		movement.r[3] += (movement.r[0] * -distance);
	if (GetKeyState(up) & 0x8000)
		movement.r[3] += (movement.r[2] * -distance);
	if (GetKeyState(down) & 0x8000)
		movement.r[3] += (movement.r[2] * distance);
	cbVars.w = XMMatrixInverse(nullptr, movement);
}



template<class Vertex>
void ObjRenderable<Vertex>::SetWorldMatrix(XMMATRIX world) {
	cbVars.w = world;
	memcpy(cbGPUAddress, &cbVars, sizeof(cbVars));
}

template<class Vertex>
void ObjRenderable<Vertex>::SetViewMatrix(XMMATRIX view) {
	cbVars.v = view;
	memcpy(cbGPUAddress, &cbVars, sizeof(cbVars));
}

template<class Vertex>
void ObjRenderable<Vertex>::SetProjectionMatrix(XMMATRIX projection) {
	cbVars.p = projection;
	memcpy(cbGPUAddress, &cbVars, sizeof(cbVars));
}

template<class Vertex>
inline void ObjRenderable<Vertex>::SetLightPosition(int index, XMVECTOR position)
{
	cbVars.lightPos[index] = position;
	memcpy(cbGPUAddress, &cbVars, sizeof(cbVars));
}

template<class Vertex>
inline void ObjRenderable<Vertex>::SetLightColor(int index, XMVECTOR color)
{
	cbVars.lightColor[index] = color;
	memcpy(cbGPUAddress, &cbVars, sizeof(cbVars));
}

template<class Vertex>
inline void ObjRenderable<Vertex>::SetLightDirection(int index, XMVECTOR direction)
{
	cbVars.lightDir[index] = direction;
	memcpy(cbGPUAddress, &cbVars, sizeof(cbVars));
}

template<class Vertex>
inline void ObjRenderable<Vertex>::SetWorldCamera(XMVECTOR cameraPosition)
{
	cbVars.worldCamera = cameraPosition;
	memcpy(cbGPUAddress, &cbVars, sizeof(cbVars));
}

template<class Vertex>
XMMATRIX ObjRenderable<Vertex>::GetWorldMatrix() { return cbVars.w; }
template<class Vertex>
XMMATRIX ObjRenderable<Vertex>::GetViewMatrix() { return cbVars.v; }
template<class Vertex>
XMMATRIX ObjRenderable<Vertex>::GetProjectionMatrix() { return cbVars.p; }

template<class Vertex>
inline ConstantBuffer ObjRenderable<Vertex>::GetConstantBuffer()
{
	return cbVars;
}

template<class Vertex>
Microsoft::WRL::ComPtr<ID3D12Resource> ObjRenderable<Vertex>::GetConstantBufferUpload() { return constantBufferUploadHeap; }
template<class Vertex>
Microsoft::WRL::ComPtr<ID3D12PipelineState> ObjRenderable<Vertex>::GetPipeline() { return pipeline; }

template<class Vertex>
Microsoft::WRL::ComPtr<ID3D12RootSignature> ObjRenderable<Vertex>::GetRootSignature() { return rootSignature; }

template<class Vertex>
inline Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> ObjRenderable<Vertex>::GetDescriptorHeap() { return mainDescriptorHeap; }

template<class Vertex>
inline CD3DX12_GPU_DESCRIPTOR_HANDLE ObjRenderable<Vertex>::GetGPUCBHandle()
{
	return CD3DX12_GPU_DESCRIPTOR_HANDLE(mainDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), 0, cbSize);
}

template<class Vertex>
inline CD3DX12_GPU_DESCRIPTOR_HANDLE ObjRenderable<Vertex>::GetGPUSRHandle()
{
	return CD3DX12_GPU_DESCRIPTOR_HANDLE(mainDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), 1, cbSize);
}

template<class Vertex>
inline D3D12_PRIMITIVE_TOPOLOGY ObjRenderable<Vertex>::GetTopology()
{
	return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}
