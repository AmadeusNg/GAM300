#include "imguiHelper/ImguiToolbar.h"
#include "imguiHelper/ImguiConsole.h"
#include "imguiHelper/ImguiHierarchy.h"
#include "Rendering/GraphicsManager.h"
#include "sceneManager/sceneManager.h"
#include <Windows.h>
#include <shellapi.h>
#include <commdlg.h> //file dialogue
#include "../EditorApp.h" //get hWnd
#include "sceneManager/serialization.h" //create new files
#include "sceneManager/sceneManager.h" //for scenemanager instance
#include "imguiHelper/ImguiSceneBrowser.h" //for "savefile() save as" function
#include "imguiHelper/ImguiGamePlayScene.h" //for "savefile() save as" function
#include "Physics/CollisionSystem.h"
#include "Rendering/GraphicsManager.h"

namespace TDS
{
	/*EditorConsole console;*/ //Wrong method of getting instance
	//std::shared_ptr<EditorConsole> console = static_pointer_cast<EditorConsole>(LevelEditorManager::GetInstance()->panels[PanelTypes::CONSOLE]);

	Toolbar::Toolbar()
	{
		//selected = 0;
		//selectedFolder = -1;
		//renameCheck = false;

		flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;
		panelTitle = "Toolbar";
		windowPadding = ImVec2(0.f, 0.f);
		

		//insertEntities();
	}

	
	void Toolbar::update()
	{
		ImGui::Text("Display: "); ImGui::SameLine();

		ImGui::SetItemDefaultFocus();

		/*if (isPlaying)
		{
			std::cout << "playinggggggggggggg" << std::endl;
		}*/

		//when play button is pressed, make button red
		if (isPlaying)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1,0.2f,0,1 });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 1,0.2f,0,1 });
		
			if (ImGui::ArrowButton("Play", ImGuiDir_Right))
			{
				//console->AddLog("Play button pressed");
				//console->AddLog("Play button pressed");
				//if (isPlay) {
				//	//App->timeManagement->Play();
				//	////TODO: Call the Init of the particles
				//	//App->scene->PlayScene(App->scene->GetRoot(), App->scene->GetRoot());
				//}
				//else if (isPause) {
				//	//set game to resume
				//	//App->timeManagement->Resume();
				//}
				/*App->scene->inGame = true;*/
				
				isPlaying = false;
				SceneManager::GetInstance()->loadScene(SceneManager::GetInstance()->getCurrentScene());
				LevelEditorManager::GetInstance()->panels[PanelTypes::SCENE]->makeFocus = true;
				
			}
			//when playing, make button red
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
		}
		else
		{
			if (ImGui::ArrowButton("Play", ImGuiDir_Right))
			{
				SceneManager::GetInstance()->saveCurrentScene();
				LevelEditorManager::GetInstance()->panels[PanelTypes::GAMEPLAYSCENE]->makeFocus = true;
				isPlaying = true;
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("||", { 23, 19 }))
		{
			//console->AddLog("Pause button pressed");
			//if (isPause) {
			//	//pause
			//	console->AddLog("Pause button pressed");

			//}

			isPlaying = isPlaying ? false : true;
		}
		ImGui::SameLine();
		//ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f,0,0,1 });
		//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1,0.2f,0,1 });
		//if (ImGui::Button("STOP", { 40, 19 }))
		//{
		//	TDS_INFO("Stop button pressed");
		//	if (isStopped) {

		//		//stop the app
		//		/*App->timeManagement->Stop();
		//		App->scene->inGame = false;
		//		App->scene->StopScene(App->scene->GetRoot(), App->scene->GetRoot());*/
		//	}
		//}
		//ImGui::PopStyleColor();
		//ImGui::PopStyleColor();


		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f,0,0,1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1,0.2f,0,1 });
		if (ImGui::Button("Open FMOD Studio", { 120, 19 }))
		{

			//console->AddLog("Opening fmod");
			if (isOpenFMOD) {

				/*const char* fmodStudioPath = "..\\Dependencies\\FMOD_Studio_2.02.17\\FMOD_Studio.exe";
				const char* command = fmodStudioPath;*/
				ShellExecute(0, L"open", L"..\\Dependencies\\FMOD_Studio_2.02.17\\FMOD_Studio.exe", NULL, NULL, SW_SHOWNORMAL);
			}
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		//ImGui::SameLine();
		//ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f,0.1f,0.1f,1 });
		//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1,0.1f,0,1 });
		//if (ImGui::Button("Load Scene", { 100, 19 }))
		//{
		//	console->AddLog("Load Scene Button Pressed");
		//	if (isLoadScene) {


		//	}
		//}
		//ImGui::PopStyleColor();
		//ImGui::PopStyleColor();

		ImGui::SameLine();

		if (ImGui::Button("Save Scene", { 90, 19 }))
		{
			//console->AddLog("Save Scene Button Pressed");
			//if (isSaveScene) {

				//std::shared_ptr<Hierarchy> hierarchyPanel = static_pointer_cast<Hierarchy>(LevelEditorManager::GetInstance()->panels[PanelTypes::HIERARCHY]);
				SceneManager::GetInstance()->saveCurrentScene();
			//}
		}


		//Save As File Dialogue:
		//note: this is to save current edited scene to a new filename based on user input
		ImGui::SameLine();
		
		if (ImGui::Button("Save Scene As...", { 120, 19 }))
		{
			TDS_INFO("Save Button Pressed");
			
			std::string entered_filename = SceneBrowser::SaveFile("*.json", Application::GetWindowHandle());
			if (!entered_filename.empty())
			{
				SceneManager::GetInstance()->SerializeToFile(entered_filename);

			}
			
		}
		
		//end save as

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f,0.1f,0.1f,1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1,0.1f,0,1 });
		if (ImGui::Button("Toggle 2D/3D view", { 120, 19 }))
		{
			static bool view2Dtoggle = false;
			//console->AddLog("Save Scene Button Pressed");
			view2Dtoggle = !view2Dtoggle;
			GraphicsManager::getInstance().ToggleViewFrom2D(view2Dtoggle);
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f,0.1f,0.1f,1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1,0.1f,0,1 });
		if (ImGui::Button("Toggle Debug Drawings", { 120, 19 }))
		{
			static bool debugDrawing = false;
			debugDrawing = !debugDrawing;
			CollisionSystem::m_RenderDebugDrawing = debugDrawing;
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		//Set scene fade to black, not a button but a slider
		/*ImGui::SameLine();*/
		//static float fadeFactor = 1.f;
		//ImGui::SliderFloat("Scene Fade", &fadeFactor, 0.0f, 1.0f);

		//GraphicsManager::getInstance().SetFadeFactor(fadeFactor);



		//ImGui::SameLine();
		//ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f,0.1f,0.1f,1 });
		//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1,0.1f,0,1 });
		//if (ImGui::Button("Add Component", { 100, 19 }))
		//{

		//	console->AddLog("Adding Component");
		//	//do add component
		//}
		//ImGui::PopStyleColor();
		//ImGui::PopStyleColor();

		float panelWidth = ImGui::GetContentRegionAvail().x;
		//for auto resizing of buttons
		int columnCount = 3;
		ImGui::Columns(std::max(columnCount, 1), 0, false);
		
		ImGui::Checkbox("Show Grid", &grid_visible);
		ImGui::NextColumn();
			

#include "GridManager/GridManager.h"
		ImGui::InputInt("Rows (X)", &GridMap::m_NumRows, 0);
		ImGui::NextColumn();

		ImGui::InputInt("Cols (Z)", &GridMap::m_NumCols, 0);

		//show the current cursor pos in the GhostPathfinding.cs script
	}
}