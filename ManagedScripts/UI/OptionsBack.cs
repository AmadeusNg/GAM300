/*!*************************************************************************
****
\file OptionsBack.cs
\author Matthew Cheung
\par DP email: j.cheung@digipen.edu
\par Course: csd3450
\date 5-33-2024
\brief  Script for settings menu to return to main menu
****************************************************************************
***/
using ScriptAPI;
using System;

public class OptionsBack : Script
{
    private UISpriteComponent sprite;
    public AudioComponent optionsmenusound;
    public override void Awake()
    {
        GraphicsManagerWrapper.ToggleViewFrom2D(true);
        sprite = gameObject.GetComponent<UISpriteComponent>();
    }
    public override void Start()
    {
        optionsmenusound = gameObject.GetComponent<AudioComponent>();
    }
    public override void Update()
    {
        if (Input.GetMouseButtonDown(Keycode.M1) && sprite.IsMouseCollided())
        {
            optionsmenusound.stop("ambience_gallery");
            SceneLoader.LoadMainMenu();
        }
    }

    public override void OnDestroy()
    {
    }
}