#include "Audio.hxx"
#include "TypeConversion.hxx"
#include <ctime>
#include <msclr\marshal_cppstd.h>

using namespace TDS::AudioWerks;

namespace ScriptAPI
{
	AudioSource::AudioSource()
	{
		audio_engine = AudioEngine::AudioEngine::get_audioengine_instance();
		deltatime = 0.f;
		wait = 0;
		//clips = gcnew System::Collections::SortedList<System::String^, AudioComponent^>();
	}

	void AudioSource::AddClips(AudioComponent clip)
	{
		clips->Add(toSystemString(clip.getFilePath()), clip);
	}
	
	void AudioSource::Play(System::String^ clip, unsigned int delay)
	{		
		/*msclr::interop::marshal_context context;
		TDS::SoundInfo temp = context.marshal_as<TDS::SoundInfo>(clip);*/

		TDS::SoundInfo temp(toStdString(clip));

		audio_engine->FadeInSound(delay, temp);
	}

	void AudioSource::Play(System::String^ clip)
	{
		TDS::SoundInfo temp(toStdString(clip));

		audio_engine->playSound(temp);
	}

	void AudioSource::Pause(System::String^ clip)
	{
		TDS::SoundInfo temp(toStdString(clip));

		audio_engine->pauseSound(temp);
	}

	void AudioSource::Stop(System::String^ clip)
	{
		TDS::SoundInfo temp(toStdString(clip));

		audio_engine->stopSound(temp);
	}

	void AudioSource::StopAll()
	{
		audio_engine->stopAllSound();
	}

	/*float AudioSource::get_volume()
	{

	}

	void AudioSource::set_volume(float vol)
	{

	}*/

	void AudioSource::Loop(bool set)
	{
		
	}

	bool AudioSource::finished(System::String^ clip)
	{
		TDS::SoundInfo* temp = audio_engine->findSound(toStdString(clip));

		return audio_engine->soundFinished(*temp);
	}

	/*float AudioSource::get_pitch()
	{

	}

	void AudioSource::set_pitch(float vol)
	{

	}*/

	//template<typename T>
	//T& AudioSource::operator=(float val)
	//{
	//	value = val;
	//}

	void AudioSource::SetListenerPos(Vector3 pos, System::String^ name)
	{

	}

	void AudioSource::SetSoundPos(Vector3 pos, System::String^ name)
	{

	}
}