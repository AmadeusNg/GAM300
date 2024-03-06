/*!*************************************************************************
****
\file GhostMovement.cs
\author Thea Sea
\par DP email: thea.sea@digipen.edu
\par Course: csd3450
\date 15-1-2024
\brief  Script for ghost movement
****************************************************************************
***/
using ScriptAPI;
using System;
using System.Collections.Specialized;
using System.Runtime.CompilerServices;
using System.Security.Cryptography;
using System.Threading.Tasks;
using System.Globalization; //for time??

using static Waypoint; //to get the waypoints you placed
using System.Reflection.PortableExecutable;
using System.Numerics;

public class GhostMovement : Script
{

    // [SerializeField]
    // float ManualMovementUnit = 3.0f;

    [SerializeField]
    float patrolSpeed = 0.1f;

    [SerializeField]
    int currTargetWaypt = 0;
    // float angle = 0f;

    [SerializeField]
    float deltaTime = 0f;

    [SerializeField]
    int NumOfWaypoints = 1;

    [SerializeField]
    bool isPatrol = true;
    [SerializeField]
    public bool isChasingPlayer = false;

    public GameObject player;
    public GameObject hidingGameObject;

    public float speed;
    public float soundSpeed;

    public int walkingSoundCounter = -1;
    public String[] walkingSounds;
    String[] monsterPatrol;
    String[] monsterAlert;
    public String voiceClips;
    public bool playSound;
    public float playSoundTimer;

    public bool playerMoved;
    public bool hideEventDone;
    public static bool GhostGone;
    public bool hideEvent;
    public int hideEventStep;
    public GameObject SHDoor;

    //public void ReadWaypoint(Waypoint wp)
    //{

    //}

    //// rotate the ghost to look at the target waypoint/player
    //void LookAtPoint(Vector3 targetPoint) //WIP (need to finish asap)
    //{
    //    Vector3 upVector = new Vector3(0f, 1f, 0f);
    //    Vector3 directionToTarget = targetPoint - gameObject.GetComponent<TransformComponent>().GetPosition();
    //    directionToTarget.Y = 0; //rotation is only around the y-axis

    //    //Quaternion targetRotation = Quaternion.LookRotation(directionToTarget, upVector);

    //    //gameObject.GetComponent<TransformComponent>().SetRotation(targetRotation);
    //    //set rotation to targetRotation;
    //}

    //static int CounterBehaviour(int current, int max)
    //{


    //    if (current > max)
    //    {
    //        current = 0; // Reset to 1 after reaching the maximum value
    //    }

    //    return current;
    //}

    public override void Awake()
    {
        walkingSounds = new string[8];
        walkingSounds[0] = "mon_woodstep1";
        walkingSounds[1] = "mon_woodstep2";
        walkingSounds[2] = "mon_woodstep3";
        walkingSounds[3] = "mon_woodstep4";
        walkingSounds[4] = "mon_woodstep5";
        walkingSounds[5] = "mon_woodstep6";
        walkingSounds[6] = "mon_woodstep7";
        walkingSounds[7] = "mon_woodstep8";

        monsterPatrol = new string[8];
        monsterPatrol[0] = "mon_patrol1";
        monsterPatrol[1] = "mon_patrol2";
        monsterPatrol[2] = "mon_patrol3";
        monsterPatrol[3] = "mon_patrol4";
        monsterPatrol[4] = "mon_patrol5";
        monsterPatrol[5] = "mon_patrol6";
        monsterPatrol[6] = "mon_patrol7";
        monsterPatrol[7] = "mon_patrol8";

        monsterAlert = new string[7];
        monsterAlert[0] = "mon_alerted1";
        monsterAlert[1] = "mon_alerted2";
        monsterAlert[2] = "mon_alerted3";
        monsterAlert[3] = "mon_alerted4";
        monsterAlert[4] = "mon_alerted5";
        monsterAlert[5] = "mon_alerted6";
        monsterAlert[6] = "mon_alerted7";

        voiceClips = "pc_monstergoesaway1";

        speed = 1.0f;
        soundSpeed = 1.0f;
        hideEventStep = 0;

        playerMoved = false;
        hideEvent = false;
        hideEventDone = false;
        GhostGone = false;

        transform.SetPositionX(-2840.0f);
        transform.SetPositionZ(-650.0f);
    }

    public override void Update()
    {
        //isChasingPlayer = !isPatrol;//can only be one or another
        //if (gameObject.GetComponent<SphereColliderComponent>().GetIsTrigger())
        //{
        //    isPatrol = false;
        //}
        //else
        //{
        //    isPatrol = true;
        //}
        ////Console.WriteLine(isChasingPlayer);

        //List<Vector3> waypointVector = new List<Vector3>();
        ////Console.WriteLine( GameObjectScriptFind("Waypoint"));
        ////Console.WriteLine($"Ghost sees target : ({GameObjectScriptFind("Waypoint").GetComponent<TransformComponent>().GetPosition().X}, {GameObjectScriptFind("Waypoint").GetComponent<TransformComponent>().GetPosition().Z})");
        ////Waypoint waypt1 = GameObjectScriptFind("Waypoint").GetComponent<TransformComponent>().GetPosition().Z;
        ////^ better way of accessing waypoint
        ////List<GameObject> test = MultipleGameObjectScriptFind("Waypoint");
        ////MultipleGameObjectScriptFind("Waypoint");
        //Vector3 pos = gameObject.GetComponent<TransformComponent>().GetPosition();
        ////Console.WriteLine(playerPos.X + " " + playerPos.Y + " " + playerPos.Z);
        ////Vector3 playerPos = player.GetComponent<TransformComponent>().GetPosition();
        //deltaTime += 0.01f; //i need help syncing with game time

        //for (int i = 0; i < NumOfWaypoints; ++i)
        //{
        //    //set position where the ghost will go (ghost approaches cabinet in the room)
        //    Vector3 wp1 = new Vector3(
        //        GameObjectScriptFind(i.ToString()).GetComponent<TransformComponent>().GetPosition().X,
        //        0,
        //        GameObjectScriptFind(i.ToString()).GetComponent<TransformComponent>().GetPosition().Z);
        //    //push back the waypoints
        //    waypointVector.Add(wp1);


        //}

        ////patrol in loop
        //if (isPatrol)
        //{

        //    //move the ghost if its posX and posZ is not = target Pos (target can be either player or the patrol waypoint)
        //    if (pos.X != waypointVector[currTargetWaypt].X)
        //    {
        //        if (Math.Abs(pos.X - waypointVector[currTargetWaypt].X) <= patrolSpeed) { pos.X = waypointVector[currTargetWaypt].X; } //stop moving
        //        if (pos.X > waypointVector[currTargetWaypt].X) { pos.X -= patrolSpeed; }
        //        else if (pos.X < waypointVector[currTargetWaypt].X) { pos.X += patrolSpeed; }

        //    }
        //    if (pos.Z != waypointVector[currTargetWaypt].Z)
        //    {

        //        if (Math.Abs(pos.Z - waypointVector[currTargetWaypt].Z) <= patrolSpeed) { pos.Z = waypointVector[currTargetWaypt].Z; } //stop
        //        if (pos.Z > waypointVector[currTargetWaypt].Z) { pos.Z -= patrolSpeed; }
        //        else if (pos.Z < waypointVector[currTargetWaypt].Z) { pos.Z += patrolSpeed; }

        //    }
        //    if (pos.X == waypointVector[currTargetWaypt].X && pos.Z == waypointVector[currTargetWaypt].Z)
        //    {
        //        currTargetWaypt++; //move to next waypoint
        //        if (currTargetWaypt == NumOfWaypoints) { currTargetWaypt = 0; }

        //    }
        //}
        //else
        //{
        //    //Vector3 diff = (playerPos - pos).normalise();
        //    //Vector3 updatePos = diff * patrolSpeed;
        //    //pos.X = updatePos.X;
        //    //pos.Z = updatePos.Z;
        //}

        //// pos.X = startingX + patrolRadius * (float)Math.Cos(angle);
        //// pos.Z = startingZ + patrolRadius * (float)Math.Sin(angle);

        //gameObject.GetComponent<TransformComponent>().SetPositionX(pos.X);

        //gameObject.GetComponent<TransformComponent>().SetPositionZ(pos.Z);

        ////Console.WriteLine($"Enemy Position: ({pos.X}, {pos.Z})");
        ////Console.WriteLine($"Going to waypoint at : ({wp1.X}, {wp1.Z})");

        // Play monster walking sound

        if (playSound)
        {
            playSound = PlayMonsterWalkingSound();
        }
        else if (isChasingPlayer) // done playing sound, chasing player
        {
            ScriptAPI.Vector2 ghostPosition = new ScriptAPI.Vector2(transform.GetPosition().X, transform.GetPosition().Z);
            ScriptAPI.Vector2 playerPosition = new ScriptAPI.Vector2(player.transform.GetPosition().X, player.transform.GetPosition().Z);
            if (WaypointPathfinder.SameRoom(ghostPosition, playerPosition)) 
            {
                // If touches, loses
                if (ScriptAPI.Vector2.Distance(ghostPosition, playerPosition) <= 10.0f)
                {
                    // Losing screen
                    SceneLoader.LoadLoseScreen();
                }
                else
                {
                    // Go straight to player
                    ScriptAPI.Vector3 originalPosition = transform.GetPosition();
                    ScriptAPI.Vector2 nextPosition = ScriptAPI.Vector2.MoveTowards(ghostPosition, playerPosition, speed);
                    transform.SetPosition(new ScriptAPI.Vector3(nextPosition.X, originalPosition.Y, nextPosition.Y));
                    
                }
                playerMoved = true;
                speed += 0.001f;
            }
            else
            {
                if (playerMoved)
                {
                    WaypointPathfinder.FindPath(ghostPosition, playerPosition);
                }
                ScriptAPI.Vector3 originalPosition = transform.GetPosition();
                ScriptAPI.Vector2 nextPosition = WaypointPathfinder.NextStep(new ScriptAPI.Vector2(originalPosition.X, originalPosition.Z), speed);
                transform.SetPosition(new ScriptAPI.Vector3(nextPosition.X, originalPosition.Y, nextPosition.Y));
                playerMoved = false;
            }
            AudioComponent audio = gameObject.GetComponent<AudioComponent>();
            int ran_num = RandomNumberGenerator.GetInt32(7);
            audio.play(monsterAlert[ran_num]);
            audio.set3DCoords(transform.GetPosition(), monsterAlert[ran_num]);
        }
        else if (hideEvent)
        {
            BedroomHidingEvent();
        }

        if (hideEventDone)
        {
            // AudioComponent audio = gameObject.GetComponent<AudioComponent>();
            // if (audio.finished(voiceClips))
            // {
            //     audio.stop(voiceClips);
            //     GameplaySubtitles.counter = 8;
            // }
            GhostGone = true;
        }
        

    }

    public void PlayMonsterWalkingSoundInitial()
    {
        AudioComponent audio = gameObject.GetComponent<AudioComponent>();
        walkingSoundCounter = 0;
        audio.play(walkingSounds[walkingSoundCounter]);
        ScriptAPI.Vector3 temp = new ScriptAPI.Vector3(1000, 1000, 0);
        audio.set3DCoords(temp/*transform.GetPosition()*/ ,walkingSounds[walkingSoundCounter]);
        playSound = true;
    }

    public bool PlayMonsterWalkingSound()
    {
        AudioComponent audio = gameObject.GetComponent<AudioComponent>();
        ScriptAPI.Vector3 temp = new ScriptAPI.Vector3(1000, 1000, 0);

        if (audio.finished(walkingSounds[walkingSoundCounter]))
        {
            if (playSoundTimer < 0)
            {
                audio.stop(walkingSounds[walkingSoundCounter]);
                ++walkingSoundCounter;

                if (walkingSoundCounter == 7)  // finished
                {
                    audio.play("pc_afterscare_heartbeat");
                    return false;
                }

                audio.play(walkingSounds[walkingSoundCounter]);
                audio.set3DCoords(temp/*transform.GetPosition()*/, walkingSounds[walkingSoundCounter]);
                playSoundTimer = soundSpeed - walkingSoundCounter * 0.05f;

                //audio.set3DCoords(transform.GetPosition());
                if (!hideEvent)
                {
                    int ra = RandomNumberGenerator.GetInt32(8);
                    audio.play(monsterPatrol[ra]);
                    audio.set3DCoords(temp /*transform.GetPosition()*/, monsterPatrol[ra]);
                }
            }
            else
            {
                playSoundTimer -= Time.deltaTime;
            }
        }

        return true;
    }

    public void AlertMonster()
    {
        soundSpeed -= 0.1f;
        speed += 0.2f;
        playSoundTimer = soundSpeed;
        isPatrol = false;
        isChasingPlayer = true;

        // Teleport monster based on door
        WaypointPathfinder.FindPath(new ScriptAPI.Vector2(-2840, -650), new ScriptAPI.Vector2(player.transform.GetPosition().X, player.transform.GetPosition().Z)); // temp position

        player.transform.SetPosition(transform.GetPosition());

        PlayMonsterWalkingSoundInitial();
    }

    public void BedroomHidingEvent()
    {
        if (!hidingGameObject.GetComponent<Hiding>().hiding)
        {
            isChasingPlayer = true;
        }
        else
        {
            isChasingPlayer = false;
        }
        ScriptAPI.Vector3 originalPosition = transform.GetPosition();
        ScriptAPI.Vector2 ghostPosition = new ScriptAPI.Vector2(originalPosition.X, originalPosition.Z);
        AudioComponent audio = gameObject.GetComponent<AudioComponent>();
        switch (hideEventStep)
        {
            case 0:
                ScriptAPI.Vector2 firstPosition = new ScriptAPI.Vector2(1790, -323);
                ScriptAPI.Vector2 firstPositionNext = ScriptAPI.Vector2.MoveTowards(ghostPosition, firstPosition, 5.0f);
                transform.SetPosition(new ScriptAPI.Vector3(firstPositionNext.X, originalPosition.Y, firstPositionNext.Y));

                if (firstPositionNext.X == firstPosition.X && firstPositionNext.Y == firstPosition.Y)
                {
                    ++hideEventStep;
                }
                audio.play("pc_afterscare_breathing");
                audio.play("pc_afterscare_heartbeat"); 
                break;

            case 1:
                ScriptAPI.Vector2 secondPosition = new ScriptAPI.Vector2(2167, -100);
                ScriptAPI.Vector2 secondPositionNext = ScriptAPI.Vector2.MoveTowards(ghostPosition, secondPosition, 5.0f);
                transform.SetPosition(new ScriptAPI.Vector3(secondPositionNext.X, originalPosition.Y, secondPositionNext.Y));

                if (secondPositionNext.X == secondPosition.X && secondPositionNext.Y == secondPosition.Y)
                {
                    ++hideEventStep;
                }

                break;

            case 2:
                transform.SetPositionX(-2840.0f);
                transform.SetPositionZ(-650.0f);
                hideEventDone = true;
                hideEvent = false;
                isChasingPlayer = false;
                SHDoor.GetComponent<Door_Script>().forcedLocked = false;

                audio.FadeOut(2, "pc_afterscare_breathing");
                audio.FadeOut(2, "pc_afterscare_heartbeat");
                audio.play(voiceClips);
                GameplaySubtitles.counter = 11; // wth was that


                break;
        }
        
    }
}
