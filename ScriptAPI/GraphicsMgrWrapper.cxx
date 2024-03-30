#include "GraphicsMgrWrapper.hxx"


namespace ScriptAPI
{
	 void GraphicsManagerWrapper::ToggleViewFrom2D(bool condition)
	{
		 TDS::GraphicsManagerWrapperCalls::ToggleViewFrom2D(condition);
	}

	 bool GraphicsManagerWrapper::IsViewingFrom2D()
	{
		 return TDS::GraphicsManagerWrapperCalls::IsViewingFrom2D();
	}

	 void GraphicsManagerWrapper::SetLayerToRender(int ID)
	 {
		 TDS::GraphicsManagerWrapperCalls::SetLayerToRender(ID);
	 }
	 void GraphicsManagerWrapper::SetFadeFactor(float value)
	 {
		 TDS::GraphicsManagerWrapperCalls::SetFadeFactor(value);
	 }
	 float GraphicsManagerWrapper::GetFadeFactor()
	 {
		 return TDS::GraphicsManagerWrapperCalls::GetFadeFactor();
	 }
	 int GraphicsManagerWrapper::CurrentRenderedLayer()
	 {
		 return TDS::GraphicsManagerWrapperCalls::CurrentRenderedLayer();
	 }
	 unsigned int GraphicsManagerWrapper::GetPickedObject()
	 {
		 return TDS::GraphicsManagerWrapperCalls::GetPickedObject();
	 }
	 void GraphicsManagerWrapper::ToggleLayerToRender(int layerID, bool condition)
	 {
		 TDS::UiSystem::ToggleEnableLayer(layerID, condition);
	 }

	 void GraphicsManagerWrapper::ToggleInventoryLayer(bool condition)
	 {
		 ToggleLayerToRender(0, condition);
		 ToggleLayerToRender(1, condition);
	 }
	 void GraphicsManagerWrapper::SetClearColor(Vector4 clearColorVector)
	 {
		 TDS::GraphicsManagerWrapperCalls::SetClearColor(clearColorVector.X, clearColorVector.Y, clearColorVector.Z, clearColorVector.W);
	 }
	 void GraphicsManagerWrapper::SetNormalizedView(bool condition)
	 {
		 TDS::GraphicsManagerWrapperCalls::SetNormalizedView(condition);
	 }
	 bool GraphicsManagerWrapper::IsNormalizedView()
	 {
		 return TDS::GraphicsManagerWrapperCalls::IsNormalizedView();
	 }
}