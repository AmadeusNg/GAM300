/*!*************************************************************************
****
\file OptionsBgmVolume.cs
\author Matthew Cheung
\par DP email: j.cheung@digipen.edu
\par Course: csd3450
\date 5-33-2024
\brief  Script for settings menu BGM volume
****************************************************************************
***/
using ScriptAPI;
using System;

public class OptionsBgmVolume : Script
{
    public bool BGMpressedVolUp;
    private float BgmVolume;
    private UISpriteComponent sprite;
    //private string bgmSoundName;
    private AudioComponent bgmVol;

    public GameObject bgmText;

    public override void Awake()
    {
        GraphicsManagerWrapper.ToggleViewFrom2D(true);
        sprite = gameObject.GetComponent<UISpriteComponent>();
        bgmVol = gameObject.GetComponent<AudioComponent>();
        //bgmSoundName = "basement_music";
    }

    public override void Update()
    {
        BgmVolume = bgmVol.getBGMVol();

        if (Input.GetMouseButtonDown(Keycode.M1) && sprite.IsMouseCollided())
        {
            if (BGMpressedVolUp)
            {
                BgmVolume += 5.0f;
                bgmVol.setBGMVol(BgmVolume);
            }
            else
            {
                BgmVolume -= 5.0f;
                bgmVol.setBGMVol(BgmVolume);
            }
            //Console.WriteLine(bgmVol.getBGMVol());
            BgmVolume = Math.Clamp(BgmVolume, 0, 100);
            bgmText.GetComponent<UISpriteComponent>().SetFontMessage(((int)BgmVolume).ToString());
        }
    }

    public override void OnDestroy()
    {
    }
}

