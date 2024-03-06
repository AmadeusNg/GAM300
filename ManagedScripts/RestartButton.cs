/*!*************************************************************************
****
\file RestartButton.cs
\author Matthew Cheung
\par DP email: j.cheung@digipen.edu
\par Course: csd3450
\date 20-1-2024
\brief  Script for pause menu restart game button
****************************************************************************
***/
using System;
using ScriptAPI;

public class RestartButton : Script
{
    public GameBlackboard? gameBlackboard;
    UISpriteComponent restartButtonSprite;
    public override void Awake()
    {
    }

    public override void Start()
    {
        restartButtonSprite = gameObject.GetComponent<UISpriteComponent>();
    }

    public override void Update()
    {
        //if (PopupUI.isDisplayed)
        if (gameBlackboard.gameState == GameBlackboard.GameState.Paused)
        {
            restartButtonSprite.SetEnabled(true);
        }
        else
        {
            restartButtonSprite.SetEnabled(false);
        }

        //if (Input.GetMouseButtonDown(Keycode.M1) && restartButtonSprite.IsMouseCollided() && PopupUI.isDisplayed == true)
        if (Input.GetMouseButtonDown(Keycode.M1) && restartButtonSprite.IsMouseCollided() && gameBlackboard.gameState == GameBlackboard.GameState.Paused)
        {
            Console.WriteLine("Restart Button Pressed");
            AudioComponent audio = gameObject.GetComponent<AudioComponent>();
            audio.stopAll();
            //PopupUI.changeDisplayed = true;
            SceneLoader.LoadMainGame();
        }
    }
}