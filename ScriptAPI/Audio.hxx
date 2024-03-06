#pragma once
#include "IncludeFromEngine.hxx"
#include <fmod_engine/AudioEngine.h>
#include "Components/AudioComponent.hxx"
#include "Vector3.hxx"
#include <components/SoundInfo.h>
#include <filesystem>

namespace ScriptAPI
{    
    public ref class AudioSource
    {
    //public:
    //    static ref struct volume
    //    {
    //        float value;
    //    };
    //    
    //    static ref struct pitch
    //    {
    //        float value;
    //    };

    public:
        AudioSource();

        static void AddClips(AudioComponent clip);
        
        static void Play(System::String^ clip, unsigned int delay);
        static void Play(System::String^ clip);
        static void Pause(System::String^ clip);
        static void Stop(System::String^ clip);
        static void StopAll();

        /*float get_volume();
        void set_volume(float vol);*/
        static void Loop(bool set);
        static bool finished(System::String^ clip);
        /*float get_pitch();
        void set_pitch(float set);*/

        void SetListenerPos(Vector3 pos, System::String^ name);
        void SetSoundPos(Vector3 pos, System::String^ name);

        //template<typename T>
        //T& operator=(float val);

        static System::Collections::SortedList^ clips;
        static TDS::AudioWerks::AudioEngine* audio_engine;
        static unsigned long wait;
        static float deltatime;
    };
}