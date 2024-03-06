#include "imguiHelper/ImguiGamePlayScene.h"
#include "Rendering/GraphicsManager.h"
#include "Rendering/RenderTarget.h"

TDS::GamePlayScene::GamePlayScene()
{
	flags = /*ImGuiWindowFlags_NoTitleBar |*/ ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiDockNodeFlags_AutoHideTabBar;
	panelTitle = "GamePlayScene";
	windowPadding = ImVec2(0.f, 0.f);

	//Input::setExitCursor(true);
	InputSystem::GetInstance()->setCursorVisible(true);
	show_cursor = 1;
}

void TDS::GamePlayScene::init()
{
	m_GamePlayDesc = ImGui_ImplVulkan_AddTexture(GraphicsManager::getInstance().getFinalImage().getSampler(), GraphicsManager::getInstance().getFinalImage().getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

}
void TDS::GamePlayScene::update()
{
	static auto& inputSystem = InputSystem::GetInstance();

	isFocus = ImGui::IsWindowFocused() && ImGui::IsItemVisible();
	if (ImGui::BeginMenuBar())
	{
		if (isPlaying)
		{
			if (ImGui::BeginMenu("Game is Running..."))
			{
				ImGui::EndMenu();
			}
		}
		else
		{
			if (ImGui::BeginMenu("Game is Paused"))
			{
				ImGui::EndMenu();
			}
		}

		ImGui::EndMenuBar();
	}


	if (isPlaying)
	{
		//if (Input::isKeyPressed(TDS_ESCAPE))
		//{
		//	Input::setExitCursor(false);
		//	Input::releaseTheKey(TDS_ESCAPE);
		//}
		//else if (!Input::getExitCursor() && isFocus && Input::isMouseButtonPressed(TDS_MOUSE_LEFT))
		//{
		//	Input::setExitCursor(true);
		//}
		if ((inputSystem->isKeyDown(VK_SHIFT) || inputSystem->isKeyPressed(VK_SHIFT)) && inputSystem->isKeyPressed(VK_ESCAPE))
		{
			gamePaused = true;
			inputSystem->setCursorVisible(true);
			inputSystem->setMouseLock(false);
		}
		else if (inputSystem->getCursorVisible() && ImGui::IsWindowHovered() && inputSystem->isMousePressed(VK_LBUTTON) && gamePaused)
		{
			proxy_audio_system::ScriptPlayAllPaused();
			inputSystem->setCursorVisible(false);
			inputSystem->setMouseLock(true);
			gamePaused = false;
		}


		window_size = ImGui::GetWindowSize();
		window_pos = ImGui::GetWindowPos();
		//Input::mousePosition globalMousePos = Input::getMousePosition();
		Vec2 globalMousePos = inputSystem->getLocalMousePos();

		//if (Input::getExitCursor() && !GraphicsManager::getInstance().IsViewingFrom2D())
		if (!inputSystem->getCursorVisible() && !GraphicsManager::getInstance().IsViewingFrom2D())
		{

			ImGuiIO& cursor_input = ImGui::GetIO();
			cursor_input.WantSetMousePos = true;
			cursor_input.MousePos = { window_pos.x + (window_size.x * .5f), window_pos.y + (window_size.y * .5f) };
			show_cursor = 0;

			//Input::setCenteredMouse(cursor_input.MousePos.x, cursor_input.MousePos.y);

			//ImGui::SetMouseCursor(ImGuiMouseCursor_None);

		}
		else
		{
			ImGuiIO& cursor_input = ImGui::GetIO();
			cursor_input.WantSetMousePos = false;
			show_cursor = 1;

			float normalizedLocalMouseX = ((globalMousePos.x - window_pos.x) / (window_size.x * 0.5f)) - 1.f;
			float normalizedLocalMouseY = ((window_pos.y + window_size.y - globalMousePos.y) / (window_size.y * 0.5f)) - 1.f;

			InputSystem::setUIMousePos(Vec2(normalizedLocalMouseX, normalizedLocalMouseY));

		}
		ShowCursor(show_cursor);
	}
	else
	{
		// Reset values
		CameraSystem::SetIsPlaying(false);

		inputSystem->setCursorVisible(true);
		inputSystem->setMouseLock(false);
	}

	ImVec2 vSize /*= ImGui::GetContentRegionAvail()*/;
	if (ImGui::GetContentRegionAvail().x < (ImGui::GetContentRegionAvail().y * (16.0f / 9.0f)))
	{
		vSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().x * (9.0f / 16.0f)); //scale y
	}
	else
	{
		vSize = ImVec2(ImGui::GetContentRegionAvail().y * (16.0f / 9.0f), ImGui::GetContentRegionAvail().y); //scale x
	}

	ImGui::Image((ImTextureID)m_GamePlayDesc, vSize);
}

void TDS::GamePlayScene::Resize()
{
	if (m_GamePlayDesc)
	{
		/*ImGui_ImplVulkan_RemoveTexture(m_GamePlayDesc);
		m_GamePlayDesc = ImGui_ImplVulkan_AddTexture(GraphicsManager::getInstance().getFinalImage().getSampler(), GraphicsManager::getInstance().getFinalImage().getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);*/
		ImGui_ImplVulkan_UpdateTexture(GraphicsManager::getInstance().getFinalImage().getSampler(), m_GamePlayDesc, GraphicsManager::getInstance().getFinalImage().getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
}
