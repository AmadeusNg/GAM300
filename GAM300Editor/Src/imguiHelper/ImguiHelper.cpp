
#include "ImguiHelper/ImguiSceneBrowser.h"
#include "imguiHelper/ImguiHelper.h"
#include "imguiHelper/ImguiHierarchy.h"
#include "imguiHelper/ImguiProperties.h"
#include "imguiHelper/ImguiAssetBrowser.h"
#include "imguiHelper/ImguiAudio.h"
#include "imguiHelper/ImguiBehaviourTree.h"

#include "imguiHelper/ImguiConsole.h"
#include "imguiHelper/ImguiToolbar.h"
#include "imguiHelper/ImguiProfiler.h"
#include "ImguiHelper/ImguiFunctionHelper.h"

namespace TDS
{
	// LevelEditorManager ============================================================================================
	std::unique_ptr<LevelEditorManager> LevelEditorManager::m_instance;

	std::unique_ptr<LevelEditorManager>& LevelEditorManager::GetInstance()
	{
		if (m_instance == nullptr)
		{
			m_instance = std::make_unique<LevelEditorManager>();

			m_instance->panels[PanelTypes::HIERARCHY] = std::make_shared<Hierarchy>();
			m_instance->panels[PanelTypes::PROPERTIES] = std::make_shared<Properties>();
			m_instance->panels[PanelTypes::ASSETBROWSER] = std::make_shared<AssetBrowser>();
			//m_instance->panels[PanelTypes::AUDIOLER] = std::make_shared<AudioImgui>();
			m_instance->panels[PanelTypes::SCENEBROWSER] = std::make_shared<SceneBrowser>();
			m_instance->panels[PanelTypes::CONSOLE] = std::make_shared<EditorConsole>();
			m_instance->panels[PanelTypes::TOOLBAR] = std::make_shared<Toolbar>();
			m_instance->panels[PanelTypes::PROFILER] = std::make_shared<Profiler>();
			m_instance->panels[PanelTypes::BEHAVIOURTREEEDITOR] = std::make_shared<BehaviourTreePanel>();
		}
		return m_instance;
	}

	// imguiHelper ===================================================================================================
	void imguiHelper::InitializeImgui(ImGui_ImplVulkan_InitInfo initinfo, VkRenderPass RenderPass, void* inHwnd)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io{ ImGui::GetIO() };
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		//ImGui::StyleColorsDark();
		CustomDarkTheme();

		ImGui_ImplVulkan_Init(&initinfo, RenderPass);
		ImGui_ImplWin32_Init(inHwnd);
		Profiler::getVulkanInfo(initinfo);
	}

	void imguiHelper::CustomDarkTheme()
	{
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
		colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.14f, 0.14f, 0.14f, 0.52f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.14f, 0.14f, 0.14f, 0.36f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
		colors[ImGuiCol_Separator] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		//colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		//colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		//colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		//colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		//colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(8.00f, 8.00f);
		style.FramePadding = ImVec2(5.00f, 2.00f);
		//style.CellPadding = ImVec2(6.00f, 6.00f);
		style.ItemSpacing = ImVec2(6.00f, 6.00f);
		style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
		style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
		style.IndentSpacing = 25;
		style.ScrollbarSize = 15;
		style.GrabMinSize = 10;
		style.WindowBorderSize = 1;
		style.ChildBorderSize = 1;
		style.PopupBorderSize = 1;
		style.FrameBorderSize = 1;
		style.TabBorderSize = 1;
		style.WindowRounding = 7;
		style.ChildRounding = 4;
		style.FrameRounding = 3;
		style.PopupRounding = 4;
		style.ScrollbarRounding = 9;
		style.GrabRounding = 3;
		style.LogSliderDeadzone = 4;
		style.TabRounding = 4;
	}

	void imguiHelper::ImguiCreateFont(VkCommandBuffer SingleUseCommandBuffer)
	{
		ImGui_ImplVulkan_CreateFontsTexture(SingleUseCommandBuffer);
	}

	void imguiHelper::Update()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// Docking 
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoBackground;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("DockSpace", 0, window_flags);

		ImGui::PopStyleVar(3);

		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::End();

		// Panels
		for (auto currentPanel : LevelEditorManager::GetInstance()->panels)
		{
			//ImGui::GetStyle().WindowPadding = currentPanel.second->windowPadding;

			if (ImGui::Begin(currentPanel.second->panelTitle.c_str(), (bool*)0, currentPanel.second->flags))
			{
				if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					currentPanel.second->rightClick = true;
				}
				else
				{
					currentPanel.second->rightClick = false;
				}

				currentPanel.second->update();
			}
			ImGui::End();
		}
	}

	void imguiHelper::Draw(VkCommandBuffer FCB)
	{
		ImGuiIO& io{ ImGui::GetIO() };

		if (const ImGuiViewport* pViewPort{ ImGui::GetMainViewport() }; pViewPort != nullptr)
		{
			io.DisplaySize = pViewPort->Size;
		}
		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), FCB);
	}

	void imguiHelper::Exit()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	/*!*************************************************************************
	This function is a helper function for draw TEXT variables
	****************************************************************************/
	std::string ImguiTextInput(std::string variableName, std::string textVariable)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text(variableName.c_str());

		ImGui::TableNextColumn();
		char temp[100];
		strcpy_s(temp, textVariable.c_str());
		ImGui::InputText(("##" + variableName).c_str(), temp, 100);

		return std::string(temp);
	}

	/*!*************************************************************************
	This function is a helper function for draw BOOl variables
	****************************************************************************/
	bool ImguiBoolInput(std::string variableName, bool boolVariable)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text(variableName.c_str());

		ImGui::TableNextColumn();
		ImGui::Checkbox(("##" + variableName).c_str(), &boolVariable);

		return boolVariable;
	}

	/*!*************************************************************************
	This function is a helper function for draw INT variables
	****************************************************************************/
	int ImguiIntInput(std::string variableName, int intVariable, float speed, int min, int max)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text(variableName.c_str());

		ImGui::TableNextColumn();
		if (max > 0)
		{
			ImGui::DragInt(("##" + variableName).c_str(), &intVariable, speed, min);
		}
		else
		{
			ImGui::DragInt(("##" + variableName).c_str(), &intVariable, speed, min, max);
		}

		return intVariable;
	}

	/*!*************************************************************************
	This function is a helper function for draw FLOAT variables
	****************************************************************************/
	float ImguiFloatInput(std::string variableName, float floatVariable, float speed, float min, float max)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text(variableName.c_str());

		ImGui::TableNextColumn();
		ImGui::DragFloat(("##" + variableName).c_str(), &floatVariable, speed, min, max);

		return floatVariable;
	}

	/*!*************************************************************************
	This function is a helper function for draw VEC2 variables
	****************************************************************************/
	Vec2 ImguiVec2Input(std::string variableName, Vec2 Vec2Variable)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text(variableName.c_str());

		ImGui::TableNextColumn();
		float temp[2]{ Vec2Variable.x, Vec2Variable.y };
		ImGui::DragFloat2(("##" + variableName).c_str(), temp, 1.0f);
		Vec2Variable.x = temp[0];
		Vec2Variable.y = temp[1];

		return Vec2Variable;
	}

	/*!*************************************************************************
	This function is a helper function for draw VEC3 variables
	****************************************************************************/
	Vec3 ImguiVec3Input(std::string variableName, Vec3 Vec3Variable)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text(variableName.c_str());

		ImGui::TableNextColumn();
		float temp[3]{ Vec3Variable.x, Vec3Variable.y, Vec3Variable.z };
		ImGui::DragFloat3(("##" + variableName).c_str(), temp, 1.0f);
		Vec3Variable.x = temp[0];
		Vec3Variable.y = temp[1];
		Vec3Variable.z = temp[2];

		return Vec3Variable;
	}
}