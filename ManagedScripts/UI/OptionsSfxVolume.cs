/*!*************************************************************************
****
\file OptionsSfxVolume.cs
\author Matthew Cheung
\par DP email: j.cheung@digipen.edu
\par Course: csd3450
\date 5-33-2024
\brief  Script for settings menu sfx volume
****************************************************************************
***/
using ScriptAPI;
using System;

public class OptionsSfxVolume : Script
{
    public bool SFXpressedVolUp;
    private float SfxVolume;
    private UISpriteComponent sprite;
    private AudioComponent sfxVol;
    private string sfxName;

    public GameObject sfxText;
    public override void Awake()
    {
        GraphicsManagerWrapper.ToggleViewFrom2D(true);
        sprite = gameObject.GetComponent<UISpriteComponent>();
        sfxVol = gameObject.GetComponent<AudioComponent>();
        // sfxName = "fireplace";

    }

    public override void Update()
    {
        SfxVolume = sfxVol.getSFXVol();

        if (Input.GetMouseButtonDown(Keycode.M1) && sprite.IsMouseCollided())
        {
            if (SFXpressedVolUp)
            {
                SfxVolume += 5.0f;
                sfxVol.setSFXVol(SfxVolume);
            }
            else
            {
                SfxVolume -= 5.0f;
                sfxVol.setSFXVol(SfxVolume);
            }
            //Console.WriteLine("sfx vol: " + sfxVol.getSFXVol());
            SfxVolume = Math.Clamp(SfxVolume, 0, 100);
            sfxText.GetComponent<UISpriteComponent>().SetFontMessage(((int)SfxVolume).ToString());
        }
    }

    public override void OnDestroy()
    {
    }
}


