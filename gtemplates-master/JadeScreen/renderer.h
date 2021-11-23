#pragma once
// minimalistic code to draw a single triangle, this is not part of the API.
// required for compiling shaders on the fly, consider pre-compiling instead
#include <d3dcompiler.h>
#include "../JadeScreen/Tex/include/DirectXTex.h"
#if _DEBUG
#pragma comment(lib, "../Tex/lib/DirectXTex.lib")
#else
#pragma comment(lib, "../Tex/lib/Release/DirectXTex.lib")
#endif
#pragma comment(lib, "d3dcompiler.lib")
#include "d3dx12.h" // official helper file provided by microsoft
#include <DirectXMath.h>
#include "test_pyramid.h"
#include <vector>
#include <chrono>
#include "ObjRenderable.h"
#include "GridRenderable.h"
#include "talon.h"
#include "Cube.h"

#define FAR_PLANE	1000.0f
#define NEAR_PLANE	0.1f

using namespace DirectX;

void test() {
	int a = 12;
	//does nothing
}

// Creation, Rendering & Cleanup
class Renderer
{
	// proxy handles
	GW::SYSTEM::GWindow win;
	GW::GRAPHICS::GDirectX12Surface d3d;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>	mainDescriptorHeap[1];

	struct ConstantBuffer
	{
		XMMATRIX w, v, p;
	};
	ConstantBuffer staticVars;
	
	//Camera stuff
	float FOV = 0.8f;
	std::chrono::steady_clock::time_point previousFrame;
	POINT previousMousePos;
	bool lookAtTarget = false;

	std::vector<ObjRenderable<OBJ_VERT>*> renderables;
public:
	
	Renderer(GW::SYSTEM::GWindow _win, GW::GRAPHICS::GDirectX12Surface _d3d)
	{
		win = _win;
		d3d = _d3d;
		ID3D12Device* creator;
		d3d.GetDevice((void**)&creator);
		D3D12_CPU_DESCRIPTOR_HANDLE rtv;
		ID3D12CommandQueue* commandQueue;
		d3d.GetCurrentRenderTargetView((void**)&rtv);

		OBJ_VERT plane[] =
		{
			{ {-25.0f, 0, -25.0f}, {0, 1, 0}, {0,1,0} },
			{ {-25.0f, 0, 25.0f}, {1, 1, 0}, {0,1,0} },
			{ {25.0f, 0, -25.0f}, {0, 0, 0}, {0,1,0} },
			{ {25.0f, 0, 25.0f}, {1, 0, 0}, {0,1,0} }
		};

		const unsigned int planeIndeces[] = {
			0,1,2,
			1,3,2
		};

		OBJ_VERT groundGrid[100] = {};
		float x = 0;
		float y = 0;
		for (int i = 0; i < 10; i++)
		{
			y = i;
			for (int j = 0; j < 10; j++)
			{
				x = j;
				groundGrid[j + i * 10] = { {-1 + x / 5, 0, -1 + y / 5}, {0 + x / 10, 1 - y / 10, 0}, {0, 1, 0} };
			}
		}

		unsigned int groundIndeces[486] = {};
		int a, b, c, d;
		a = 10;
		b = 0;
		c = 11;
		d = 1;
		/*for (int i = 0; i < 486; i += 6)
		{
			groundIndeces[i] = a;
			groundIndeces[i + 1] = b;
			groundIndeces[i + 2] = c;
			groundIndeces[i + 3] = b;
			groundIndeces[i + 4] = d;
			groundIndeces[i + 5] = c;
			a++;
			b++;
			c++;
			d++;
		}*/
		int index = 0;
		for (int i = 0; i < 9; i++)
		{
			a = 0 + i * 10;
			b = 1 + i * 10;
			c = 10 + i * 10;
			d = 11 + i * 10;
			for (int j = 0; j < 9; j++)
			{
				groundIndeces[index] = a;
				groundIndeces[index + 1] = c;
				groundIndeces[index + 2] = d;
				groundIndeces[index + 3] = a;
				groundIndeces[index + 4] = d;
				groundIndeces[index + 5] = b;
				a++;
				b++;
				c++;
				d++;
				index += 6;
			}
		}

		//Init Resources
		for (size_t i = 0; i < 5; i++)
		{
			renderables.push_back(new ObjRenderable<OBJ_VERT>(creator, d3d));
		}
		//renderables.push_back(new GridRenderable<OBJ_VERT>(creator, d3d));
		renderables[0]->InitResources(test_pyramid_data, test_pyramid_indicies, test_pyramid_vertexcount, test_pyramid_indexcount);
		renderables[1]->InitResources(talon_data, talon_indicies, _countof(talon_data), _countof(talon_indicies));
		renderables[2]->InitResources(Cube_data, Cube_indicies, Cube_vertexcount, Cube_indexcount);
		renderables[3]->InitResources(plane, planeIndeces, 4, 6);
		renderables[4]->InitResources(groundGrid, groundIndeces, 100, 486);
		

		// Create Vertex Shader
		UINT compilerFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
		compilerFlags |= D3DCOMPILE_DEBUG;
#endif
		Microsoft::WRL::ComPtr<ID3DBlob> vsBlob, errors;
		// Create Pixel Shader
		Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> solidpsBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> gridvsBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> vs3dBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> ps3dBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> inputvsBlob;

		D3DReadFileToBlob(L"BaseVertexShader.cso", vsBlob.GetAddressOf());
		D3DReadFileToBlob(L"BasePixelShader.cso", psBlob.GetAddressOf());
		D3DReadFileToBlob(L"SolidPixelShader.cso", solidpsBlob.GetAddressOf());
		D3DReadFileToBlob(L"GridVertexShader.cso", gridvsBlob.GetAddressOf());
		D3DReadFileToBlob(L"3DVertexShader.cso", vs3dBlob.GetAddressOf());
		D3DReadFileToBlob(L"3DPixelShader.cso", ps3dBlob.GetAddressOf());
		D3DReadFileToBlob(L"InputMapVS.cso", inputvsBlob.GetAddressOf());


		//Constant Buffer stuff
		staticVars.w = XMMatrixIdentity();
		XMVECTOR Eye = XMVectorSet(1.0f, 2.0f, -10.0f, 0.0f);
		XMVECTOR At = XMVectorSet(1.0f, 0.25f, 0.0f, 0.0f);
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		staticVars.v = XMMatrixLookAtLH(Eye, At, Up);
		float ar = 1;
		d3d.GetAspectRatio(ar);
		staticVars.p = XMMatrixPerspectiveFovLH(FOV, ar, NEAR_PLANE, FAR_PLANE);
		
		renderables[0]->SetWorldMatrix(XMMatrixMultiply(XMMatrixTranslation(3, 0, 0), renderables[0]->GetWorldMatrix()));
		
		XMMATRIX skybox = XMMatrixIdentity();
		skybox.r[3] = renderables[2]->GetWorldMatrix().r[3];
		renderables[1]->SetLightColor(0, XMVectorSet(1, 1, 1, 1));
		renderables[3]->SetLightColor(0, XMVectorSet(1, 1, 1, 1));
		renderables[3]->SetWorldMatrix(XMMatrixTranslation(0, -0.5f, 0));
		renderables[4]->SetWorldMatrix(XMMatrixTranslation(50, -0.5f, 0));
		renderables[4]->SetWorldMatrix(XMMatrixMultiply(XMMatrixScaling(25, 0, 25), renderables[4]->GetWorldMatrix()));

		//Pipeline
		renderables[0]->CreatePipeline(vsBlob, psBlob);
		renderables[1]->CreatePipeline(vsBlob, psBlob);
		renderables[2]->CreatePipeline(vs3dBlob, ps3dBlob);
		renderables[3]->CreatePipeline(vsBlob, psBlob);
		renderables[4]->CreatePipeline(inputvsBlob, psBlob);

		std::vector<std::thread> threads;
		ThreadData* data = new ThreadData();
		threads.push_back(std::thread(&ObjRenderable<OBJ_VERT>::ThreadedLoadTexture, renderables[0], L"../Assets/troll.dds", data));
		threads.push_back(std::thread(&ObjRenderable<OBJ_VERT>::ThreadedLoadTexture, renderables[1], L"../Assets/marauder.dds", data));
		threads.push_back(std::thread(&ObjRenderable<OBJ_VERT>::ThreadedLoadTexture, renderables[2], L"../Assets/SunsetSkybox.dds", data));
		threads.push_back(std::thread(&ObjRenderable<OBJ_VERT>::ThreadedLoadTexture, renderables[3], L"../Assets/Box_grass.dds", data));
		threads.push_back(std::thread(&ObjRenderable<OBJ_VERT>::ThreadedLoadTexture, renderables[4], L"../Assets/Box_Sand.dds", data));
		for (auto& thread : threads)
			thread.join();
		//Renderables Texture
		//Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> troll = renderables[0]->LoadTexture(L"../Assets/troll.dds");
		//Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> temp = renderables[1]->LoadTexture(L"../Assets/marauder.dds");
		////Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> sky = renderables[2]->LoadTexture3D(L"../Assets/SkyboxOcean.dds");
		////Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> sky = renderables[2]->LoadTexture3D(L"../Assets/DuneCube.dds");
		//Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> sky = renderables[2]->LoadTexture(L"../Assets/SunsetSkybox.dds");
		//Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> ground = renderables[3]->LoadTexture(L"../Assets/Box_grass.dds");

		//d3d.GetCommandQueue((void**)&commandQueue);
		//Microsoft::WRL::ComPtr<ID3D12CommandList> tempCommandList[] = { troll.Get(), temp.Get(), sky.Get(), ground.Get()};
		//
		//commandQueue->ExecuteCommandLists(_countof(tempCommandList), tempCommandList->GetAddressOf());
		
		renderables[2]->ccw = true;
		// free temporary handle
		creator->Release();
		delete data;
		//commandQueue->Release();
		
	}
	void Render()
	{
		ID3D12GraphicsCommandList* cmd;
		D3D12_CPU_DESCRIPTOR_HANDLE rtv;
		D3D12_CPU_DESCRIPTOR_HANDLE dsv;
		d3d.GetCommandList((void**)&cmd);
		d3d.GetCurrentRenderTargetView((void**)&rtv);
		d3d.GetDepthStencilView((void**)&dsv);
		cmd->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

		renderables[2]->Draw(cmd);
		cmd->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAGS::D3D12_CLEAR_FLAG_DEPTH, 1, 0, 0, nullptr);
		//std::vector<std::thread> threads;
		//Microsoft::WRL::ComPtr<ID3D12CommandList> commands[3];
		//ThreadData* data = new ThreadData();
		for (size_t i = 0; i < renderables.size(); i++)
		{
			if (i == 2)
			{
				continue;
			}
			renderables[i]->Draw(cmd);
			//threads.push_back(std::thread(&ObjRenderable<OBJ_VERT>::ThreadedDraw, renderables[i], data));
		}
		
		/*for (auto& thread : threads)
			thread.join();*/
		
		// release temp handles
		cmd->Release();
	}
	void Update() {
		//Carry over Stuff
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		float timeElapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - previousFrame).count();
		timeElapsed /= 1000000;
		POINT mousePos;
		GetCursorPos(&mousePos);
		//Camera Stuff
		XMMATRIX newCamera = staticVars.v;
		newCamera = XMMatrixInverse(nullptr, newCamera);
		float cameraSpeed = 10.0f;
		float cameraRotationSpeed = 2.0f;
		float change = cameraSpeed * timeElapsed;
		bool cameraMoved = false;
		if (GetAsyncKeyState(0x49) & 0x01)
			lookAtTarget = !lookAtTarget;

		if (GetKeyState(0x57) & 0x8000) {
			newCamera.r[3] += (newCamera.r[2] * change);
		}
		if (GetKeyState(0x53) & 0x8000) {
			newCamera.r[3] += (newCamera.r[2] * -change);
		}
		if (GetKeyState(0x44) & 0x8000) {
			newCamera.r[3] += (newCamera.r[0] * change);
		}
		if (GetKeyState(0x41) & 0x8000) {
			newCamera.r[3] += (newCamera.r[0] * -change);
		}
		if (GetKeyState(VK_SPACE) & 0x8000)
		{
			newCamera.r[3] += (newCamera.r[1] * change);
		}
		if (GetKeyState(VK_SHIFT) & 0x8000) {
			newCamera.r[3] += (newCamera.r[1] * -change);
		}
			
		if (GetKeyState(VK_RBUTTON) & 0x8000 && !lookAtTarget) {
			std::vector<float> direction;
			direction.push_back(mousePos.x - previousMousePos.x);
			direction.push_back(mousePos.y - previousMousePos.y);
			for (size_t i = 0; i < direction.size(); i++)
			{
				direction[i] *= timeElapsed;
			}
			XMVECTOR pos = newCamera.r[3];
			newCamera.r[3] = XMVectorSet(0, 0, 0, 1);
			newCamera = XMMatrixMultiply(XMMatrixRotationX(direction[1]), newCamera);
			newCamera = XMMatrixMultiply(newCamera, XMMatrixRotationY(direction[0]));
			newCamera.r[3] = pos;
		}

		// Lock on
		// Only locks on the ship for now
		if (lookAtTarget) {
			XMVECTOR eye = newCamera.r[3];
			XMVECTOR at = renderables[1]->GetWorldMatrix().r[3];
			XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);;
			XMMATRIX newRotation = XMMatrixLookAtLH(eye, at, up);
			newRotation = XMMatrixInverse(nullptr, newRotation);
			newCamera = newRotation;
		}
		
		//skybox
		XMMATRIX skyboxWorld = XMMatrixTranslation(XMVectorGetX(newCamera.r[3]), XMVectorGetY(newCamera.r[3]), XMVectorGetZ(newCamera.r[3]));
		renderables[2]->SetWorldMatrix(skyboxWorld);

		//More Lighting stuff
		for (size_t i = 0; i < renderables.size() - 1; i++)
		{
			renderables[i]->SetLightPosition(1, newCamera.r[3]);
			renderables[i]->SetLightDirection(0, newCamera.r[2]);
			renderables[i]->SetWorldCamera(newCamera.r[3]);
		}

		newCamera = XMMatrixInverse(nullptr, newCamera);
		staticVars.v = newCamera;

		//FOV Stuff

		//Zoom out
		if (GetKeyState(0x51) & 0x8000 && FOV <= 1.2f) {
			FOV += timeElapsed * 0.2f;
		}
		//Zoom in
		if (GetKeyState(0x45) & 0x8000 && FOV >= 0.6f) {
			FOV -= timeElapsed * 0.2f;
		}

		// Setting the new projection and view matrix
		float ar = 1;
		d3d.GetAspectRatio(ar);
		for (int i = 0; i < renderables.size(); i++)
		{
			renderables[i]->SetProjectionMatrix(XMMatrixPerspectiveFovLH(FOV, ar, 0.01f, 1000.0f));
			renderables[i]->SetViewMatrix(newCamera);
		}
		
		//Rotation
		XMMATRIX pyramidMat = renderables[1]->GetWorldMatrix();
		XMVECTOR pyramidPos = pyramidMat.r[3];
		pyramidMat = XMMatrixMultiply(pyramidMat, XMMatrixRotationY(timeElapsed * 0.5f));
		pyramidMat.r[3] = pyramidPos;
		renderables[1]->SetWorldMatrix(pyramidMat);
		

		//Lighting stuff
		renderables[0]->ControlObject(VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN, timeElapsed * 10.0f);
		renderables[1]->SetLightPosition(0, renderables[0]->GetWorldMatrix().r[3]);
		renderables[3]->SetLightPosition(0, renderables[0]->GetWorldMatrix().r[3]);

		XMVECTOR lightposition = renderables[1]->GetConstantBuffer().lightPos[0];
		XMVECTOR lightColor = renderables[1]->GetConstantBuffer().lightColor[0];
		
		previousFrame = now;
		previousMousePos = mousePos;
	}
	~Renderer()
	{
		for (auto renderable : renderables) {
			delete renderable;
		}
	}
};
