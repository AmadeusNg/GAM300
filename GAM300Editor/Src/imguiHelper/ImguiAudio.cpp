/*!*************************************************************************
****
\file ImguiAudio.cpp
\author w.chongsheng
\par DP email: w.chongsheng@digipen.edu
\par Course: CSD3400
\par Section: a
\brief  Imgui UI in charge of audio manipulations
****************************************************************************
***/

#include "imguiHelper/ImguiAudio.h"
#include <string.h>

namespace TDS
{
	AudioImgui::AudioImgui()
	{
		//flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;
		panelTitle = "Audio Files";
		windowPadding = ImVec2(0.f, 0.f);

		appear = playing = false;
		bar = 0.f;

		music.clear();
		SFX.clear();
		background.clear();
		VO.clear();

		std::cout << "AudImg Constructor" << '\n';

		audeng = AudioWerks::AudioEngine::get_audioengine_instance();
		add_audio_files("../assets/audioFiles/");
	}

	AudioImgui::~AudioImgui()
	{
		//delete audeng;
	}

	bool AudioImgui::ToggleControls(bool state)
	{
		appear = state;

		if (appear)
		{
			flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;
		}
		else
		{
			flags = ImGuiWindowFlags_None;

			if (playing)
			{
				

				playing = false;
			}
		}

		return appear;
	}

	void AudioImgui::add_audio_files(std::filesystem::path folder_path)
	{
		/*std::filesystem::path Music_path{ folder_path / "Music" }, Background_path{ folder_path / "Songs" },
			SFX_path{ folder_path / "Sound Effects" }, Voice_path{ folder_path / "Voice Overs" };*/

		std::filesystem::path full_path = folder_path; //pathing / append;
		std::vector<std::filesystem::path> all_files; //store all file path

		if (std::filesystem::is_directory(folder_path))
		{
			all_files = go_deeper(full_path);
		}

		for (auto& str : all_files)
		{
			if (str.string().find("/Music\\") != std::string::npos)
			{
				SoundInfo temp(str.string());

				background.push_back(temp);
			}
			else if (str.string().find("/Songs\\") != std::string::npos)
			{
				SoundInfo temp(str.string());

				music.push_back(temp);
			}
			else if (str.string().find("/Sound Effects\\") != std::string::npos)
			{
				SoundInfo temp(str.string());

				SFX.push_back(temp);
			}
			else if (str.string().find("/Voice Overs\\") != std::string::npos)
			{
				SoundInfo temp(str.string());

				VO.push_back(temp);
			}
		}
	}

	std::vector<std::filesystem::path> AudioImgui::go_deeper(std::filesystem::path f_path)
	{
		std::vector<std::filesystem::path> folders;
		std::vector<std::filesystem::path> files;

		for (auto& temp : std::filesystem::directory_iterator(f_path))
		{
			if (temp.is_directory())
			{
				folders = go_deeper(temp);

				for (auto& temp2 : folders)
				{
					files.push_back(temp2);
				}
			}
			else
			{
				files.push_back(temp);
			}
		}

		return files;
	}

	void AudioImgui::play(std::string file)
	{
		SoundInfo incase{};
		SoundInfo* this_one{ &incase };

		for (SoundInfo& temp : music)
		{
			if (temp.getFilePath().find(file) != std::string::npos && temp.getFilePath().find(".meta") == std::string::npos)
			{
				this_one = &temp;
				goto FOUND;
			}
		}
		for (SoundInfo& temp : background)
		{
			if (temp.getFilePath().find(file) != std::string::npos && temp.getFilePath().find(".meta") == std::string::npos)
			{
				this_one = &temp;
				goto FOUND;
			}
		}
		for (SoundInfo& temp : SFX)
		{
			if (temp.getFilePath().find(file) != std::string::npos && temp.getFilePath().find(".meta") == std::string::npos)
			{
				this_one = &temp;
				goto FOUND;
			}
		}
		for (SoundInfo& temp : VO)
		{
			if (temp.getFilePath().find(file) != std::string::npos && temp.getFilePath().find(".meta") == std::string::npos)
			{
				this_one = &temp;
				goto FOUND;
			}
		}

	FOUND:

		audeng->loadSound(*this_one);
		audeng->playSound(*this_one);

		playing = true;
	}

	void AudioImgui::update()
	{
		if (appear)
		{
			ImGui::Text("Controls: ");
			ImGui::SameLine();

			if (ImGui::Selectable("Play Song"))
			{
				if (ImGui::ArrowButton("Play", ImGuiDir_Right))
				{
					/*SoundInfo selected{};
					selected = ImGui::IsItemClicked();

					audeng->playSound();*/
				}
			}

			ImGui::Columns(3);
			ImGui::SliderFloat("Playback", &bar, 16, 512);
		}

		audeng->update();
	}
}