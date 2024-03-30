#pragma once
#include "Rendering/GraphicsManagerWrapperCalls.h"
#include "Rendering/UiSystem.h"
#include "Vector4.hxx"

namespace ScriptAPI 
{

    public ref class GraphicsManagerWrapper
    {
    public:
		static void ToggleViewFrom2D(bool condition);

		static bool IsViewingFrom2D();
		static void ToggleLayerToRender(int layerID, bool condition);
		static void ToggleInventoryLayer(bool condition);
		static void SetLayerToRender(int ID);
		static void SetFadeFactor(float value);
		static float GetFadeFactor();
		static int CurrentRenderedLayer();
		static unsigned int GetPickedObject();
		static void SetClearColor(Vector4 clearColorVector);
		static void SetNormalizedView(bool	condition);
		static bool	IsNormalizedView();
    };
}
