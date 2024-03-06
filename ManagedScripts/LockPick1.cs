/*!*************************************************************************
****
\file LockPick1.cs
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\par Course: csd3450
\date 12-11-2023
\brief  Script lock picking gameplay
****************************************************************************
***/
using System;
using System.ComponentModel.DataAnnotations;
using ScriptAPI;

public class LockPick1 : Script
{
    public GameBlackboard? gameBlackboard;

    float toDegree(float radians)
    {
        return radians * (180 / 3.1415926535897931f);
    }
    float toRadians(float degree)
    {
        return degree * (3.1415926535897931f / 180);
    }

    //[Header("Tutorial UI Variables")]
    //public Image _TutorialImage;
    //public Sprite[] _TutorialImgSprites;
    //public Text _Press2Continue;
    //[SerializeField] int _TutorialStep;

    //public Text mySubtitles;
    //public Image mySubtitlesBG;
    String[] Subtitles;
    [SerializeField]
    public static int counter;

    public AudioSource myVOSource;
    public string startingVOstr;
    public string[] endVOstr;
    public bool _TutorialCompleted;

    [Header("Lockpick Variables")]
    public GameDataManager myGameDataManager;
    public GameObject Door_UI;
    public CameraComponent cam;
    public CameraComponent playerCam;
    public TransformComponent innerLock;
    public TransformComponent pickPosition;
    public GameObject playerController;
    public GameObject lockGroup;
    public string difficultyLvl;
    public float maxAngle = 90;
    public float lockSpeed = 10;
    public bool unlocked;
    //public AudioClip[] lockSoundEffects;
    //public AudioClip[] rattleSoundEffects;
    public String[] lockSoundEffects;
    public String[] rattleSoundEffects;
    public static String[] playerGuideVO;
    float delay = 0.4f;
    public GameObject _NumOfTries;
    public UISpriteComponent _AmtOfTries;

    [Range(1, 25)]
    public float lockRange = 10;

    [SerializeField] private int numOfTries;
    public int savedNumOfTries = 0;
    private float percentage;
    private float eulerAngle;
    private float unlockAngle;
    private Vector2 unlockRange;
    private float keyPressTime;
    private bool movePick;
    private bool deduct;
    private bool displayTutorial;
    private bool next_VO;
    private bool pickWasCloseButYouMovedAway;
    private bool firstTimeTutorial = true;
    private bool playOnce = true;
    [SerializeField] bool played;

    private Vector3 originalPosition = new Vector3(0.0f, 350.0f, 1500.0f);
    private Vector3 originalRotation;
    public TransformComponent arrowTransform;
    public UISpriteComponent arrowSprite;
    private Vector3 originalArrowPosition;
    public static AudioComponent audio;
    public static bool failed;
    public static bool passed;
    float timer;

    public int doorIndex;
    public GameObject doorText;
    public GameObject monster;
    public GameObject doorStates;

    // Start is called before the first frame update
    override public void Awake()
    {
        lockSoundEffects = new String[3];
        lockSoundEffects[0] = "lockpick_turn";
        lockSoundEffects[1] = "lockpick success";
        lockSoundEffects[2] = "lockpick_failtry";

        rattleSoundEffects = new String[7];
        rattleSoundEffects[0] = "lockpick_move1";
        rattleSoundEffects[1] = "lockpick_move2";
        rattleSoundEffects[2] = "lockpick_move3";
        rattleSoundEffects[3] = "lockpick_move4";
        rattleSoundEffects[4] = "lockpick_move5";
        rattleSoundEffects[5] = "lockpick_move6";
        rattleSoundEffects[6] = "lockpick_move7";

        playerGuideVO = new String[7];
        playerGuideVO[0] = "pc_lockpickstart";
        playerGuideVO[1] = "pc_lockpicksuccess1";
        playerGuideVO[2] = "pc_lockpicksuccess2";
        playerGuideVO[3] = "pc_lockpickfail";
        playerGuideVO[4] = "pc_findtherightspot";
        playerGuideVO[5] = "pc_turnthelock";
        playerGuideVO[6] = "";

        Subtitles = new String[9];
        Subtitles[0] = "Martin (Internal): Hopefully, I won\'t forget how to do this.";
        Subtitles[1] = "Martin (Internal): Alright, looks like I\'m in.";
        Subtitles[2] = "Martin (Internal): No turning back now.";
        Subtitles[3] = "Martin (Internal): That was too loud... I better not screw up again.";
        Subtitles[4] = "";
        Subtitles[5] = "Move [mouse] to adjust pick";
        Subtitles[6] = "Press [E] to turn lock";
        Subtitles[7] = "Martin (Internal): Find the right spot, and it should click...";
        Subtitles[8] = "Martin (Internal): There, now to turn the lock.";

        counter = 0;
        savedNumOfTries = 0;
        audio = gameObject.GetComponent<AudioComponent>();
        next_VO = true;
        // GameplaySubtitles.counter = 5; //no effect on set gameplay subtitles to be empty
        originalArrowPosition = arrowTransform.GetPosition();

        newLock();
    }

    public override void Start()
    {
        //audio.play(startingVOstr);
        movePick = true;
        next_VO = true;
        pickWasCloseButYouMovedAway = false;
    }

    // Update is called once per frame
    override public void Update()
    {
        //Input.Lock(false);
        doorText.SetActive(false);

        if (Input.GetKeyDown(Keycode.ESC))
        {
            playerController.SetActive(true);
            gameBlackboard.gameState = GameBlackboard.GameState.InGame;
            //Input.Lock(true);
            //    playerCam.SetEnabled(true);
            //    Door_UI.SetActive(false);
            lockGroup.SetActive(false);
            GraphicsManagerWrapper.ToggleViewFrom2D(false);
            doorStates.SetActive(true);

            savedNumOfTries = numOfTries;
        }

        //if (!_TutorialCompleted)
        //{
        //    // NOTE: Audio
        //    //if (!myVOSource.isPlaying() && !played)
        //    //{
        //    //    myVOSource.Play();
        //    //    mySubtitlesBG.enabled = true;
        //    //    mySubtitles.text = "Martin (Internal): Hopefully, I won’t forget how to do this.";
        //    //    played = true;

        //    //}
        //    //else if (!myVOSource.isPlaying() && played)
        //    //{
        //    //    mySubtitles.text = "";
        //    //    mySubtitlesBG.enabled = false;
        //    //    _NumOfTries.SetActive(true);
        //    //    movePick = true;
        //    //    _TutorialCompleted = true;
        //    //}
        //}
        //else
        //{
        //_NumOfTries.SetActive(true);

        //subtitles
        UISpriteComponent Sprite = GameObjectScriptFind("VOSubtitles").GetComponent<UISpriteComponent>();
        UISpriteComponent ClosedSub = GameObjectScriptFind("Subtitles").GetComponent<UISpriteComponent>();
        //UISpriteComponent Sprite = gameObject.GetComponent<UISpriteComponent>();

        if (counter < 5 && next_VO)
        {
            audio.play(playerGuideVO[counter]);
            next_VO = false;
        }
        if (audio.finished(playerGuideVO[0]))
            counter = 6;

        #region Move Pick
        float eulerAngleDegree = toDegree(eulerAngle);
        percentage = Mathf.Round(100 - Mathf.Abs(((eulerAngleDegree - unlockAngle) / 100) * 100));
        float maxRotation = (percentage / 100) * maxAngle;
        float lockRotation = maxRotation * keyPressTime;

        if (movePick)
        {
            //Vector3 dir = Input.mousePosition - cam.WorldToScreenPoint(transform.position);
            Vector3 dir = Input.GetLocalMousePos() - new Vector3(Screen.width / 2, Screen.height / 2, 0);  //cam.WorldToScreenPoint(transform.GetPosition());

            //eulerAngle = Vector3.Angle(dir, Vector3.Up());
            eulerAngle = Vector3.Angle(dir, Vector3.Down());

            Vector3 cross = Vector3.Cross(Vector3.Up(), dir);
            if (cross.Z < 0) { eulerAngle = -eulerAngle; }
            eulerAngle = Mathf.Clamp(eulerAngle, toRadians(-maxAngle), toRadians(maxAngle));

            Quaternion rotateTo = Quaternion.AngleAxis(eulerAngle, Vector3.Forward());
            //transform.SetRotation(Quaternion.EulerAngle(rotateTo));

            Vector3 originalRotation = transform.GetRotation();
            transform.SetRotation(new Vector3(originalRotation.X, originalRotation.Y, -eulerAngle));

            Vector2 newPosition = new Vector2(originalPosition.X * Mathf.Cos(-eulerAngle) - originalPosition.Y * Mathf.Sin(-eulerAngle),
                                                originalPosition.X * Mathf.Sin(-eulerAngle) + originalPosition.Y * Mathf.Cos(-eulerAngle));
            transform.SetPosition(new Vector3(newPosition.X, newPosition.Y, originalPosition.Z));

            #region Green / Red Arrow

            Vector2 newArrowPosition = new Vector2(originalArrowPosition.X * Mathf.Cos(-eulerAngle) - originalArrowPosition.Y * Mathf.Sin(-eulerAngle),
                                                originalArrowPosition.X * Mathf.Sin(-eulerAngle) + originalArrowPosition.Y * Mathf.Cos(-eulerAngle));
            arrowTransform.SetPosition(new Vector3(-newArrowPosition.X, newArrowPosition.Y, 0.0f));
            arrowTransform.SetRotation(new Vector3(0.0f, 0.0f, eulerAngle));

            #endregion

            //do rattling sounds based on pick angle
            #region Rattle Sounds & Arrow Color

            //note on ranges (the wider the range in if statement, the further the pick is)
            // STRICTEST if statement first (small range to wide range)
            //perfect! 1 is when eulerAngleDegree < unlockRange.Y && eulerAngleDegree > unlockRange.X
            //VERY close! 2 is when eulerAngleDegree < unlockRange.Y + 10 && eulerAngleDegree > unlockRange.X -10
            //almost close 3 is when eulerAngleDegree < unlockRange.Y + 20 && eulerAngleDegree > unlockRange.X - 20
            //close 4 is when eulerAngleDegree < unlockRange.Y + 30 && eulerAngleDegree > unlockRange.X - 30
            //not very close 5 is when eulerAngleDegree < unlockRange.Y + 40 && eulerAngleDegree > unlockRange.X - 40
            //not close 6 is when eulerAngleDegree < unlockRange.Y + 50 && eulerAngleDegree > unlockRange.X - 50
            //7 is if u were in 2 previously but are in range 3 to 6 now
            //(else) no sound when none of the cases above

            //range 1
            if (eulerAngleDegree < unlockRange.Y && eulerAngleDegree > unlockRange.X)
            {
                arrowSprite.SetTextureName("Arrow-Green.dds");

                if (audio.finished(lockSoundEffects[0]))
                {
                    audio.stop(lockSoundEffects[0]);
                    delay -= Time.deltaTime;

                    if (delay <= 0)
                    {
                        // Not sure if there is a better way to do this
                        if (audio.finished(rattleSoundEffects[0]))
                            audio.stop(rattleSoundEffects[0]);
                        if (audio.finished(rattleSoundEffects[1]))
                            audio.stop(rattleSoundEffects[1]);
                        if (audio.finished(rattleSoundEffects[2]))
                            audio.stop(rattleSoundEffects[2]);
                        if (audio.finished(rattleSoundEffects[3]))
                            audio.stop(rattleSoundEffects[3]);
                        if (audio.finished(rattleSoundEffects[4]))
                            audio.stop(rattleSoundEffects[4]);
                        if (audio.finished(rattleSoundEffects[5]))
                            audio.stop(rattleSoundEffects[5]);

                        //audio.setVolume(50.0f);
                        audio.play(rattleSoundEffects[0]);
                        delay = 0.4f;
                    }
                }

                //play VO
                if (playOnce && audio.finished("pc_findtherightspot") && audio.finished(playerGuideVO[0]))
                {
                    audio.play("pc_turnthelock");
                    
                    playOnce = false;
                }
                if (audio.finished("pc_turnthelock"))
                {
                    audio.stop("pc_turnthelock");
                }
                //if (audio.finished(rattleSoundEffects[0]))
                //{
                //    audio.stop(rattleSoundEffects[0]);

                //    audio.play("pc_findtherightspot");
                //    if (audio.finished("pc_findtherightspot"))
                //    {
                //        audio.stop("pc_findtherightspot");
                //        audio.play("pc_turnthelock");
                //        if (audio.finished("pc_turnthelock"))
                //        {
                //            audio.stop("pc_turnthelock");
                //        }
                //    }
                //}
                pickWasCloseButYouMovedAway = true;
            }
            //range 2
            else if (eulerAngleDegree < unlockRange.Y + 15 && eulerAngleDegree > unlockRange.X - 15)
            {
                arrowSprite.SetTextureName("Arrow-Red.dds");

                if (audio.finished(lockSoundEffects[0]))
                {
                    audio.stop(lockSoundEffects[0]);
                    delay -= Time.deltaTime;

                    if (delay <= 0)
                    {
                        // Not sure if there is a better way to do this
                        if (audio.finished(rattleSoundEffects[0]))
                            audio.stop(rattleSoundEffects[0]);
                        if (audio.finished(rattleSoundEffects[1]))
                            audio.stop(rattleSoundEffects[1]);
                        if (audio.finished(rattleSoundEffects[2]))
                            audio.stop(rattleSoundEffects[2]);
                        if (audio.finished(rattleSoundEffects[3]))
                            audio.stop(rattleSoundEffects[3]);
                        if (audio.finished(rattleSoundEffects[4]))
                            audio.stop(rattleSoundEffects[4]);
                        if (audio.finished(rattleSoundEffects[5]))
                            audio.stop(rattleSoundEffects[5]);

                        //audio.setVolume(50.0f);
                        audio.play(rattleSoundEffects[1]);
                        delay = 0.4f;
                    }
                }

                
            }
            //range 3
            else if (eulerAngleDegree < unlockRange.Y + 30 && eulerAngleDegree > unlockRange.X - 30)
            {
                arrowSprite.SetTextureName("Arrow-Red.dds");

                if (audio.finished(lockSoundEffects[0]))
                {
                    audio.stop(lockSoundEffects[0]);
                    delay -= Time.deltaTime;

                    if (delay <= 0)
                    {
                        // Not sure if there is a better way to do this
                        if (audio.finished(rattleSoundEffects[0]))
                            audio.stop(rattleSoundEffects[0]);
                        if (audio.finished(rattleSoundEffects[1]))
                            audio.stop(rattleSoundEffects[1]);
                        if (audio.finished(rattleSoundEffects[2]))
                            audio.stop(rattleSoundEffects[2]);
                        if (audio.finished(rattleSoundEffects[3]))
                            audio.stop(rattleSoundEffects[3]);
                        if (audio.finished(rattleSoundEffects[4]))
                            audio.stop(rattleSoundEffects[4]);
                        if (audio.finished(rattleSoundEffects[5]))
                            audio.stop(rattleSoundEffects[5]);

                        //audio.setVolume(51.0f);
                        audio.play(rattleSoundEffects[2]);
                        delay = 0.4f;
                    }
                }

            }
            //range 4
            else if (eulerAngleDegree < unlockRange.Y + 45 && eulerAngleDegree > unlockRange.X - 45)
            {
                arrowSprite.SetTextureName("Arrow-Red.dds");

                if (audio.finished(lockSoundEffects[0]))
                {
                    audio.stop(lockSoundEffects[0]);
                    delay -= Time.deltaTime;

                    if (delay <= 0)
                    {
                        // Not sure if there is a better way to do this
                        if (audio.finished(rattleSoundEffects[0]))
                            audio.stop(rattleSoundEffects[0]);
                        if (audio.finished(rattleSoundEffects[1]))
                            audio.stop(rattleSoundEffects[1]);
                        if (audio.finished(rattleSoundEffects[2]))
                            audio.stop(rattleSoundEffects[2]);
                        if (audio.finished(rattleSoundEffects[3]))
                            audio.stop(rattleSoundEffects[3]);
                        if (audio.finished(rattleSoundEffects[4]))
                            audio.stop(rattleSoundEffects[4]);
                        if (audio.finished(rattleSoundEffects[5]))
                            audio.stop(rattleSoundEffects[5]);

                        //audio.setVolume(51.0f);

                        audio.play(rattleSoundEffects[3]);
                        delay = 0.4f;
                    }
                }

                
            }
            //range 5
            else if (eulerAngleDegree < unlockRange.Y + 60 && eulerAngleDegree > unlockRange.X - 60)
            {
                arrowSprite.SetTextureName("Arrow-Red.dds");

                if (audio.finished(lockSoundEffects[0]))
                {
                    audio.stop(lockSoundEffects[0]);
                    delay -= Time.deltaTime;

                    if (delay <= 0)
                    {
                        // Not sure if there is a better way to do this
                        if (audio.finished(rattleSoundEffects[0]))
                            audio.stop(rattleSoundEffects[0]);
                        if (audio.finished(rattleSoundEffects[1]))
                            audio.stop(rattleSoundEffects[1]);
                        if (audio.finished(rattleSoundEffects[2]))
                            audio.stop(rattleSoundEffects[2]);
                        if (audio.finished(rattleSoundEffects[3]))
                            audio.stop(rattleSoundEffects[3]);
                        if (audio.finished(rattleSoundEffects[4]))
                            audio.stop(rattleSoundEffects[4]);
                        if (audio.finished(rattleSoundEffects[5]))
                            audio.stop(rattleSoundEffects[5]);

                        //audio.setVolume(51.0f);
                        audio.play(rattleSoundEffects[4]);
                        delay = 0.4f;
                    }
                }

                
            }
            //range 6
            else if (eulerAngleDegree < unlockRange.Y + 75 && eulerAngleDegree > unlockRange.X - 75)
            {
                arrowSprite.SetTextureName("Arrow-Red.dds");

                if (audio.finished(lockSoundEffects[0]))
                {
                    audio.stop(lockSoundEffects[0]);
                    delay -= Time.deltaTime;

                    if (delay <= 0)
                    {
                        // Not sure if there is a better way to do this
                        if (audio.finished(rattleSoundEffects[0]))
                            audio.stop(rattleSoundEffects[0]);
                        if (audio.finished(rattleSoundEffects[1]))
                            audio.stop(rattleSoundEffects[1]);
                        if (audio.finished(rattleSoundEffects[2]))
                            audio.stop(rattleSoundEffects[2]);
                        if (audio.finished(rattleSoundEffects[3]))
                            audio.stop(rattleSoundEffects[3]);
                        if (audio.finished(rattleSoundEffects[4]))
                            audio.stop(rattleSoundEffects[4]);
                        if (audio.finished(rattleSoundEffects[5]))
                            audio.stop(rattleSoundEffects[5]);

                        //audio.setVolume(51.0f);
                        audio.play(rattleSoundEffects[5]);
                        delay = 0.4f;
                    }
                }
            }
            //range 7
            else if (pickWasCloseButYouMovedAway) 
            {
                arrowSprite.SetTextureName("Arrow-Red.dds");

                if (audio.finished(lockSoundEffects[0]))
                {
                    audio.stop(lockSoundEffects[0]);
                    delay -= Time.deltaTime;

                    if (delay <= 0)
                    {
                        // Not sure if there is a better way to do this
                        if (audio.finished(rattleSoundEffects[0]))
                            audio.stop(rattleSoundEffects[0]);
                        if (audio.finished(rattleSoundEffects[1]))
                            audio.stop(rattleSoundEffects[1]);
                        if (audio.finished(rattleSoundEffects[2]))
                            audio.stop(rattleSoundEffects[2]);
                        if (audio.finished(rattleSoundEffects[3]))
                            audio.stop(rattleSoundEffects[3]);
                        if (audio.finished(rattleSoundEffects[4]))
                            audio.stop(rattleSoundEffects[4]);
                        if (audio.finished(rattleSoundEffects[5]))
                            audio.stop(rattleSoundEffects[5]);

                        //audio.setVolume(51.0f);
                        audio.play(rattleSoundEffects[6]); //play 7
                        delay = 0.4f;
                    }
                }
                if (audio.finished(rattleSoundEffects[6]) && audio.finished(playerGuideVO[0]) && audio.finished("pc_turnthelock")) //prevent multiple VO playing
                {
                    audio.stop(rattleSoundEffects[0]);

                    audio.play("pc_findtherightspot");
                }
                playOnce = true;
            }

            if (audio.finished("pc_findtherightspot"))
            {
                audio.stop("pc_findtherightspot");
            }

            #endregion
        }

        if (!failed && Input.GetKeyDown(Keycode.E)) //lock turns
        {
            originalRotation = transform.GetRotation();
            movePick = false;
            keyPressTime = 1;
            audio.play(lockSoundEffects[0]); //lock turning sound

        }
        if (Input.GetKey(Keycode.E))
        {
            counter = 5; //"Move [mouse] to adjust pick";
            next_VO = true;

        }
        if (Input.GetKeyUp(Keycode.E)) //lock not turning
        {
            movePick = true;
            keyPressTime = 0;
            deduct = true;
            if (audio.finished(playerGuideVO[0]))
            {
                //;
                //wait for "Hopefully I won't forget how to 
                //do this".. to finish playing before showing ui instructions
                counter = 6; //"Press [E] to turn lock";
                next_VO = true;
            }
        }
        #endregion

        #region Check if pick is at correct position
        float lockLerp = Mathf.LerpAngle(toDegree(innerLock.GetRotation().Z), lockRotation, Time.deltaTime * lockSpeed);
        innerLock.SetRotation(new Vector3(0, 0, toRadians(lockLerp)));

        if (!movePick && (lockLerp >= maxRotation - 1))
        {
            //if you pick correct
            if (eulerAngleDegree < unlockRange.Y && eulerAngleDegree > unlockRange.X)
            {
                audio.stop(lockSoundEffects[0]);
                movePick = true;
                keyPressTime = 0;
                // NOTE: Audio
                audio.play(lockSoundEffects[1]);
                timer = 1.2f;
                passed = true;

            }
            //you pick but it wasnt correct
            else
            {
                if (deduct == true)
                {
                    numOfTries -= 1;
                    deduct = false;
                }

                if (numOfTries <= 0)
                {
                    // NOTE: Audio
                    audio.play(lockSoundEffects[2]);
                    movePick = false;
                    timer = 1.0f;
                    failed = true;
                }
            }
        }
        #endregion

        _AmtOfTries.SetFontMessage("Number of tries left: " + numOfTries.ToString());

        if (numOfTries <= 1)
        {
            _AmtOfTries.SetFontColour(new Vector4(1.0f, 0.0f, 0.0f, 1.0f)); // red
        }

        if (passed)
        {
            audio.stop(rattleSoundEffects[0]);
            counter = 1;
            firstTimeTutorial = false;
            if (timer <= 0 && audio.finished(playerGuideVO[1]))
            {
                playerController.SetActive(true);
                Vector3 rotation = playerController.transform.GetRotation();
                Quaternion quat = new Quaternion(rotation);
                Vector3 rotationToVector = new Vector3(-Mathf.Sin(toRadians(rotation.Y)), 0.0f, Mathf.Cos(toRadians(rotation.Y))) * 200;
                playerController.GetComponent<RigidBodyComponent>().SetPositionRotationAndVelocity(playerController.transform.GetPosition() + rotationToVector, new Vector4(quat.X, quat.Y, quat.Z, quat.W), new Vector3(1, 1, 1).Normalize(), new Vector3(1, 1, 1).Normalize());

                gameBlackboard.gameState = GameBlackboard.GameState.InGame;
                //Input.Lock(true);
                //    playerCam.SetEnabled(true);
                //    Door_UI.SetActive(false);
                unlocked = true;
                doorStates.SetActive(true);
                doorStates.GetComponent<DoorState>().Doors[doorIndex] = DoorState.State.Unlocked;
                lockGroup.SetActive(false);
                GraphicsManagerWrapper.ToggleViewFrom2D(false);
                savedNumOfTries = 0;

                //no turning back now
                //ClosedSub.SetFontMessage(Subtitles[1]); no effect

                if (doorIndex == 0)
                {
                    counter = 2;
                    audio.play(playerGuideVO[2]); //aite looks like im in
                    next_VO = true;
                    GameplaySubtitles.counter = 7;
                }
                if (doorIndex == 1)
                {
                    GameplaySubtitles.counter = 21;
                    audio.play("creak3"); 
                    audio.play("pc_approachbedroom"); //placeholder
                }

                if (doorIndex == 3)
                {
                    //bathroom is near

                }
                if (doorIndex == 4) //you are in bathroom
                {
                   

                }

                // if (audio.finished(playerGuideVO[2]))
                // {
                //     GameplaySubtitles.counter = 5; //no effect
                // }


                // if (audio.finished(playerGuideVO[1])) //also no effect, doont do this
                // {
                //     audio.stop(playerGuideVO[1]);
                //     counter = 2;
                //     audio.play(playerGuideVO[2]);

                // }

            }
            else
            {
                timer -= Time.deltaTime;
            }
        }

        if (failed)
        {
            counter = 3;
            if (timer <= 0 && audio.finished(playerGuideVO[3]))
            {
                //audio.stop(playerGuideVO[3]);
                playerController.SetActive(true);
                gameBlackboard.gameState = GameBlackboard.GameState.InGame;
                //Input.Lock(true);
                //    playerCam.SetEnabled(true);
                //    Door_UI.SetActive(false);
                lockGroup.SetActive(false);
                GraphicsManagerWrapper.ToggleViewFrom2D(false);
                doorStates.SetActive(true);
                savedNumOfTries = 0;

                if (doorIndex != 0)
                {
                    monster.GetComponent<GhostMovement>().AlertMonster();
                }
                counter = 5; //move mouse to adjust pick
                next_VO = true;
            }
            else
            {
                timer -= Time.deltaTime;
            }
        }

        Sprite.SetFontMessage(Subtitles[counter]); //update last
    }

    public void newLock()
    {
        gameBlackboard.gameState = GameBlackboard.GameState.Lockpicking;
        originalPosition = new Vector3(0.0f, 600.0f, 2500.0f);
        originalRotation = transform.GetRotation();

        audio.stop(lockSoundEffects[1]);
        audio.stop(lockSoundEffects[2]);
        //popupUI.GetComponent<PopupUI>().lockpickDisplayed = true;
        //Input.Lock(false);

        failed = false;
        passed = false;

        //Door_UI.SetActive(true);
        _AmtOfTries.SetFontColour(new Vector4(1.0f, 1.0f, 1.0f, 1.0f)); // white
        keyPressTime = 0;
        unlocked = false;
        deduct = true;
        //playerController.SetEnabled(false);
        //playerCam.SetEnabled(false);
        //cam.transform.SetRotation(new Vector3(0, 0, 0));
        unlockAngle = ScriptAPI.Random.Range(-maxAngle + lockRange, maxAngle - lockRange);
        unlockRange = new Vector2(unlockAngle - lockRange, unlockAngle + lockRange);

        if (savedNumOfTries == 0)
        {
            numOfTries = 5;
            if (difficultyLvl == "Easy")
            {
                numOfTries = 10;
            }
            else if (difficultyLvl == "Normal")
            {
                numOfTries = 5;
            }
            else if (difficultyLvl == "Hard")
            {
                numOfTries = 3;
            }
        }
        else
        {
            numOfTries = savedNumOfTries;
        }

        if (_TutorialCompleted)
        {
            movePick = true;
        }
        //start with subtitle press E to turn lock
    }
}
