#pragma once
#include "Rendering/MaterialFactory/MaterialShader.h"
namespace TDS
{
	class PhongBlinnMaterial final : public MaterialShader
	{
	public:
		PhongBlinnMaterial();
		~PhongBlinnMaterial();
		virtual void Init();
		virtual void Destroy();

	};


}