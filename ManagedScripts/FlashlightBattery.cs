/*!*************************************************************************
****
\file FlashlightBattery.cs
\author Thea Sea
\par DP email: thea.sea@digipen.edu
\par Course: csd3450
\date 4-3-2024
\brief  Script for flashlight battery
****************************************************************************
***/
using ScriptAPI;
using System;

public class FlashlighBattery : Script
{
    String[] Textures;



    public override void Awake()
    {
        Textures = new string[9];
        Textures[0] = "battery_full.dds";
        Textures[1] = "battery_75.dds";
        Textures[2] = "battery_50.dds";
        Textures[3] = "battery_25.dds";
        Textures[4] = "battery_0.dds";
    }

    public override void Start()
    {
        
        
    }

    public override void Update()
    {
        UISpriteComponent Sprite = gameObject.GetComponent<UISpriteComponent>();

        if (Flashlight_Script.batteryLife > 75) //show full battery
        {
            Sprite.SetTextureName(Textures[0]);
        }
        else if (Flashlight_Script.batteryLife > 50) //show 51-75%
        {
            Sprite.SetTextureName(Textures[1]);
        }
        else if (Flashlight_Script.batteryLife > 25) //show 26-50%
        {
            Sprite.SetTextureName(Textures[2]);
        }
        else if (Flashlight_Script.batteryLife > 0) //show 0-25%
        {
            Sprite.SetTextureName(Textures[3]);
        }
        else if (Flashlight_Script.batteryLife <= 0) //show empty
        {
            Sprite.SetTextureName(Textures[4]);
        }

    }

    public override void LateUpdate()
    {
        
    }

    
}
