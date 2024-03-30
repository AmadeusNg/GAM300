#include "components/SoundInfo.h"
#include "fmod_engine/AudioEngine.h"

RTTR_REGISTRATION
{
    using namespace TDS;

    rttr::registration::class_<SoundInfo>("Audio")
        .property("File path", &SoundInfo::filePath)
        .property("Loop", &SoundInfo::isLoop)
        .property("3D", &SoundInfo::is3D)
        .property("Muted", &SoundInfo::isMuted)
        .property("Position", &SoundInfo::position)
        .property("volume", &SoundInfo::volume)
        .property("Reverb", &SoundInfo::ReverbAmount)
        .property("MS Length", &SoundInfo::MSLength)

        .method("set3DCoords", rttr::select_overload<void(float, float, float)>(&SoundInfo::set3DCoords))
        .method("set3DCoords", rttr::select_overload<void(Vec3)>(&SoundInfo::set3DCoords))
        .method("setFilePath", &SoundInfo::setFilePath)
        .method("isLoaded", &SoundInfo::isLoaded);
}

namespace TDS
{
    void SoundInfo::set3DCoords(float x, float y, float z)
    {
        position.x = x;
        position.y = y;
        position.z = z;
    }

    void SoundInfo::set3DCoords(Vec3 set_this)
    {
        position = set_this;
    }

    void SoundInfo::setFilePath(std::string _path)
    {
        filePath = _path;
    }

    bool SoundInfo::isLoaded()
    {
        return (whatState == SOUND_LOADED);
    }

    Vec3 SoundInfo::get3DCoords()
    {
        return position;
    }

    SOUND_STATE SoundInfo::getState()
    {
        return whatState;
    }

    unsigned int SoundInfo::getUniqueID()
    {
        return uniqueID;
    }

    std::string SoundInfo::getFilePath()
    {
        return filePath;
    }

    const char* SoundInfo::getFilePath_inChar()
    {
        const char* name = filePath.c_str();

        return name;
    }

    float SoundInfo::getX()
    {
        return position[0]; //!!!!!!!To be replaced when vec container is used
    }

    float SoundInfo::getY()
    {
        return position[1]; //!!!!!!!To be replaced when vec container is used
    }

    float SoundInfo::getZ()
    {
        return position[2]; //!!!!!!!To be replaced when vec container is used
    }

    float SoundInfo::getReverbAmount()
    {
        return ReverbAmount;
    }

    float SoundInfo::getVolume()
    {
        return volume;
    }

    /**
    * Parameter takes in Volume values (0 - 100)
    */
    void SoundInfo::setVol(float vol)
    {
        vol /= 100.f;
        volume = Mathf::Clamp(vol, 0.f, 1.f);
    }

    SoundInfo::SoundInfo(std::string _filePath, bool _isLoop, bool _is3D, bool _muted, SOUND_STATE _theState, float _x, float _y, float _z, float _volume, float _reverbamount)
        : filePath(_filePath), isLoop(_isLoop), is3D(_is3D), isMuted(_muted), whatState(_theState), volume(_volume), ReverbAmount(_reverbamount)
    {
        position.x = _x;
        position.y = _y;
        position.z = _z;
        MSLength = 0;
        uniqueID = ID_Count++; //Change UID to include time when added
    }
    
    SoundInfo* GetSoundInfo(EntityID ID)
    {
        return ecs.getComponent<SoundInfo>(ID);
    }
}