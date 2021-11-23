#pragma once
#include "ObjRenderable.h"

template<typename Vertex>
class SkyboxRenderable : public ObjRenderable<Vertex>
{
protected:
public:
	SkyboxRenderable(ID3D12Device* _device, GW::GRAPHICS::GDirectX12Surface _d3d) : ObjRenderable(_device, _d3d) {};

private:

};


