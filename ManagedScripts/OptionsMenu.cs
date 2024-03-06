/*!*************************************************************************
****
\file OptionsMenu.cs
\author Matthew Cheung
\par DP email: j.cheung@digipen.edu
\par Course: csd3450
\date 26-2-2024
\brief  Script for options menu
****************************************************************************
***/
using ScriptAPI;
using System;

public class OptionsMenu : Script
{
    public GameBlackboard? gameBlackboard;

    [SerializeField]
    public static bool changeDisplayed = false;
    //public static bool isDisplayed = false;
    
    public bool lockpickDisplayed;
    UISpriteComponent OptionScreen;

    public GameObject player;

    public override void Awake()
    {
        lockpickDisplayed = false;
    }

    public override void Start()
    {
        OptionScreen = gameObject.GetComponent<UISpriteComponent>();
    }
    public override void Update()
    {
          
    }

}
