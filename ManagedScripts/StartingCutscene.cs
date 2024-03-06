/*!*************************************************************************
****
\file StartingCutscene.cs
\author Thea Sea
\par DP email: thea.sea@digipen.edu
\par Course: csd3450
\date 24-11-2023
\brief  Script for cutscene at the start of the game
****************************************************************************
***/
using ScriptAPI;
using System;

public class StartingCutscene : Script
{
    String[] Textures;
    int counter;
    private bool fadeIn = true;
    private bool fadeOut = true;
    private float incrementFading = Time.deltaTime / 3f;
    public GameObject blackScreen;
    public override void Awake()
    {
        GraphicsManagerWrapper.ToggleViewFrom2D(true);
        Textures = new string[9];
        Textures[0] = "Scene00.dds";
        Textures[1] = "Scene01.dds";
        Textures[2] = "NEW_Scene02.dds";
        Textures[3] = "NEW_Scene03.dds";
        Textures[4] = "NEW_Scene04.dds";
        Textures[5] = "NEW_Scene05.dds";
        Textures[6] = "NEW_Scene06.dds";
        Textures[7] = "NEW_Scene07.dds";
        Textures[8] = "NEW_Scene08.dds";

        counter = 0;
    }

    public override void Update()
    {

        if (fadeIn == true)
        {
            float alpha = blackScreen.GetComponent<UISpriteComponent>().getColourAlpha();
            alpha -= incrementFading;
            alpha = Mathf.Clamp(alpha, 0, 1);
            blackScreen.GetComponent<UISpriteComponent>().setColourAlpha(alpha);
            if (alpha <= 0)
            {
                fadeIn = false;
            }
        }
        if (fadeOut == true)
        {
            float alpha = blackScreen.GetComponent<UISpriteComponent>().getColourAlpha();
            alpha += incrementFading;
            alpha = Mathf.Clamp(alpha, 0, 1);
            blackScreen.GetComponent<UISpriteComponent>().setColourAlpha(alpha);
            if (alpha >= 1)
            {
                fadeOut = false;
            }
        }
        counter = CutsceneSubtitle.counter;

        UISpriteComponent Sprite = gameObject.GetComponent<UISpriteComponent>();

        //if audio plays finish
        if (counter <= 4)
        {
            Sprite.SetTextureName(Textures[0]);//change texture
            Sprite.ColorAlphafade(0.046785f);
        }
        else if (counter == 5)
        {
            Sprite.setColourAlpha(1);
            Sprite.SetTextureName(Textures[1]);
        }
        else if (counter == 6 || counter == 7)
            Sprite.SetTextureName(Textures[2]);
        else if (counter == 8 || counter == 9)
            Sprite.SetTextureName(Textures[3]);
        else if (counter == 10 || counter == 11)
            Sprite.SetTextureName(Textures[4]);
        else if (counter == 12 || counter == 13)
            Sprite.SetTextureName(Textures[5]);
        else if (counter == 14)
            Sprite.SetTextureName(Textures[6]);
        else if (counter == 15)
            Sprite.SetTextureName(Textures[7]);
        else if (counter > 15)
        {
            Sprite.SetTextureName(Textures[8]);
            fadeOut = true;
        }
        

        //change subtitle

    }
}
