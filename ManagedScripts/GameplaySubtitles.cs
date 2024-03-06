/*!*************************************************************************
****
\file GameplaySubtitles.cs
\author Thea Sea
\par DP email: thea.sea@digipen.edu
\par Course: csd3450
\date 24-11-2023
\brief  Script for any subtitles that appear in main game
****************************************************************************
***/
using ScriptAPI;
using System;

public class GameplaySubtitles : Script
{
    String[] Audiofiles;
    String[] BGMfile;
    String[] Subtitles;
    [SerializeField]
    public static int counter;
    public static bool next = true;
    float timer;

    public override void Awake()
    {
        Audiofiles = new String[17];
        Subtitles = new String[23];
        GraphicsManagerWrapper.ToggleViewFrom2D(false);
        Subtitles[0] = "Press [F] for flashlight";
        Subtitles[1] = "Press [WASD] to move";
        Subtitles[2] = "Press [I] to open/close Inventory";
        Subtitles[3] = "Press [I] to open/close Inventory";
        Subtitles[4] = "Press [E] to interact with objects"; //do lockpicking
                                                             //note: 
                                                             // these subtitles: "Martin (Internal): Hopefully, I won\’t forget how to do this.";
                                                             //"Move [mouse] to adjust pick", "Press [E] to turn lock"
                                                             //are done in LockPick1.cs already, so this handles overall gameplay subtitles

        Subtitles[5] = "";
        Subtitles[6] = "Alright, looks like I\'m in.";

        Subtitles[7] = "No turning back now.";


        Subtitles[8] = "";
        Subtitles[9] = "Nothing inside,";

        Subtitles[10] = "But I could hide in here in case someone shows up";
        Subtitles[11] = "What the hell was that?";
        //to be continued, cause how to detect player is in bedroom? change of scene?
        Subtitles[12] = "Empty, but i could've sworn someone opened that door";
        Subtitles[13] = "One down. Hopefully this is worth a lot";

        Subtitles[14] = "According to this, there's a painting somewhere in here. But where?";

        Subtitles[15] = "I have to watch my back. I can't get seen by that thing";

        Subtitles[16] = "The shower's running... but I don't hear anyone in there.";
        Subtitles[17] = "Okay, uncle, if you're an artist,";
        Subtitles[18] = "that means you've got paintings for me to steal.";
        Subtitles[19] = "Let's see where you're keeping them.";
        Subtitles[20] = "Something's behind this painting.";
        Subtitles[21] = "Is someone here?";
        Subtitles[22] = "Someone's coming... I better hide.";

        Audiofiles[0] = ""; //wasd no audio
        Audiofiles[1] = ""; //no audio
        Audiofiles[2] = ""; //no
        Audiofiles[3] = "";
        Audiofiles[4] = "";
        Audiofiles[5] = "";
        Audiofiles[6] = "houseenter";
        Audiofiles[7] = "";
        Audiofiles[8] = "";
        Audiofiles[9] = "";
        Audiofiles[10] = "";
        Audiofiles[11] = "";
        Audiofiles[12] = "pc_enterbedroom"; //empty, but i could've sworn someone opened that door
        Audiofiles[13] = "pc_approachbedroom"; //is someone here?
        Audiofiles[14] = "intro8_2";
        Audiofiles[15] = "intro9_1";
        Audiofiles[16] = "intro9_2";

        BGMfile = new String[1];
        BGMfile[0] = "houseenter";

        counter = 0;
        next = true;
        timer = 3.0f;
    }

    public override void Update()
    {

        UISpriteComponent Sprite = gameObject.GetComponent<UISpriteComponent>();
        AudioComponent audio = gameObject.GetComponent<AudioComponent>();

        if (counter == 0)
        {
            if (Input.GetKeyDown(Keycode.F))
                counter++;

        } if (counter == 1)
        {
            if (Input.GetKeyDown(Keycode.W) || Input.GetKeyDown(Keycode.A) || Input.GetKeyDown(Keycode.S) || Input.GetKeyDown(Keycode.D))
            {
                //go next line
                counter++;
            }
        }
        if (counter == 2 || counter == 3)
        {
            if (Input.GetKeyDown(Keycode.I))
                counter++;

        }
        if (counter == 4)
        {
            if (Input.GetKeyDown(Keycode.E))
                counter++;

        }
        if (LockPick1.counter == 2) //only this works when LockPick1 script is no longer active, checking passed doesnt work
        {
            //this is already handled in lockpick script
            //counter = 7; 
            if (LockPick1.audio.finished(LockPick1.playerGuideVO[2])) //no turning back now
            {
                LockPick1.audio.stop(LockPick1.playerGuideVO[2]);
                counter = 17;
                LockPick1.counter = 6;//prevent audio repeat
                //play enter house bgm
                audio.play(BGMfile[0]);
                audio.play("pc_okuncle"); //placeholder
            }

        }
        if (counter == 9 || counter == 10)
        {
            if (audio.finished("pc_hideinclosetfirst"))
            {
                audio.stop("pc_hideinclosetfirst");
                GameplaySubtitles.counter = 8;
            }
        }
        if (counter == 13)
        {
            if(audio.finished("gallery_movepainting"))
            {
                audio.play("pc_monsterrattledoor"); //someone's coming...better hide
                GameplaySubtitles.counter = 22;

            }

        }
        if (counter == 15)
        {
            if (audio.finished("pc_monstergoesaway2"))
            {
                audio.stop("pc_monstergoesaway2");
                GameplaySubtitles.counter = 8;
            }
        }
        if (counter == 17)
        { 
            timer -= Time.deltaTime;
            if (timer<=0.0f)
            {
                counter = 18;
            }
            
        }
        if (counter == 18)
        {
            if (audio.finished("pc_okuncle")) 
            {
               
                counter = 19;
                audio.play("pc_letsseewhere");
            }
        }
        if (counter == 19)
        {
            if (audio.finished("pc_letsseewhere"))
            {
                audio.stop("pc_letsseewhere");
                counter = 8;
            }
        }
        if (counter == 20)
        {
            if (audio.finished("pc_shinelightbeforereceipt") && audio.finished("pc_stealpainting1"))
            {
                counter = 8;
            }
        }
        if (counter == 21)
        {
            if (audio.finished("pc_approachbedroom"))
            {
                audio.stop("pc_approachbedroom");
                counter = 8;
            }
        }
        if (counter == 22)
        {
            if (audio.finished("pc_monsterrattledoor"))
            {
                audio.stop("pc_monsterrattledoor");
                GameplaySubtitles.counter = 8;
            }
        }
        if (Note_Script.isNotePicked)
        {
            if (audio.finished("pc_checkreceipt"))
            {
                audio.stop("pc_checkreceipt");
                GameplaySubtitles.counter = 8;
            }
            
            //Note_Script.isNotePicked = false;

        }
        
        if (Painting_Script.isPaintingCollected)
        {
            //if (audio.finished("pc_stealpainting1"))
            //{
            //    audio.stop("pc_stealpainting1");
            //    GameplaySubtitles.counter = 8;
            //}
            //Painting_Script.isPaintingCollected = false; //reset for other paintings
        }
        if (counter == 8)
        {
            Note_Script.isNotePicked = false;
            Painting_Script.isPaintingCollected = false; //reset for other paintings

        }

        // if (Input.GetKeyDown(Keycode.SPACE))
        // {
        //     audio.stop(Audiofiles[counter]);
        //     GraphicsManagerWrapper.ToggleViewFrom2D(false);
        //     SceneLoader.LoadMainGame();
        // }
        // else
        // {
        //     audio.playQueue();

        //     if (counter > 16)//cutscene over
        //     {
        //         GraphicsManagerWrapper.ToggleViewFrom2D(false);
        //         SceneLoader.LoadMainGame();
        //     }
        //     else
        //     {
        //         if (next)
        //         {
        Sprite.SetFontMessage(Subtitles[counter]);
        //             audio.play(Audiofiles[counter]);
        //             next = false;
        //         }
        //         else if (audio.finished(Audiofiles[counter]))
        //         {
        //             next = true;
        //             ++counter;
        //         }
        //     }
        // }
        
    }
}