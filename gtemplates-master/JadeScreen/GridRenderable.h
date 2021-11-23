#pragma once
#include "ObjRenderable.h"

template<typename Vertex>
class GridRenderable : public ObjRenderable<Vertex>
{
public:
	GridRenderable(ID3D12Device* _device, GW::GRAPHICS::GDirectX12Surface _d3d) : ObjRenderable(_device, _d3d) {};
	void InitResources(const Vertex data[], const unsigned int vertexSize);
	void CreatePipeline(Microsoft::WRL::ComPtr<ID3DBlob> vertexShader, Microsoft::WRL::ComPtr<ID3DBlob> pixelShader, Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature = nullptr);
	void Draw(ID3D12GraphicsCommandList* cmd);
	
	D3D12_PRIMITIVE_TOPOLOGY					GetTopology();
};

template<typename Vertex>
void GridRenderable<Vertex>::InitResources(const Vertex data[], const unsigned int vertexSize)
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

template<typename Vertex>
inline void GridRenderable<Vertex>::CreatePipeline(Microsoft::WRL::ComPtr<ID3DBlob> vertexShader,
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShader,
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature)
{
	D3D12_INPUT_ELEMENT_DESC format[] = {
			{
				"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			}
	};

	rootSignature = _rootSignature;

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

		CD3DX12_DESCRIPTOR_RANGE1 descRanges[1] = {};
		descRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

		CD3DX12_ROOT_PARAMETER1 rootParameter[1] = {};
		rootParameter[0].InitAsDescriptorTable(1, &descRanges[0], D3D12_SHADER_VISIBILITY_VERTEX);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc;
		rootSigDesc.Init_1_1(_countof(rootParameter), rootParameter, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		Microsoft::WRL::ComPtr<ID3DBlob> signature, errors;
		D3DX12SerializeVersionedRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1_1, signature.GetAddressOf(), errors.GetAddressOf());
		device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	}

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1;
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
	psDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	psDesc.NumRenderTargets = 1;
	psDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psDesc.SampleDesc.Count = 1;
	device->CreateGraphicsPipelineState(&psDesc, IID_PPV_ARGS(&pipeline));
}

template<typename Vertex>
inline void GridRenderable<Vertex>::Draw(ID3D12GraphicsCommandList* cmd)
{
	cmd->SetGraphicsRootSignature(rootSignature.Get());

	ID3D12DescriptorHeap* descriptorHeaps[] = { mainDescriptorHeap.Get() };
	cmd->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(mainDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), 0, cbSize);

	cmd->SetGraphicsRootDescriptorTable(0, cbvHandle);
	cmd->SetPipelineState(pipeline.Get());
	cmd->IASetVertexBuffers(0, 1, &vertexView);
	cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	cmd->DrawInstanced(vertexCount, 1, 0, 0);
}

template<typename Vertex>
inline D3D12_PRIMITIVE_TOPOLOGY GridRenderable<Vertex>::GetTopology()
{
	return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
}
