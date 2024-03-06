/*!*************************************************************************
****
\file Note_Script.cs
\author Celine Leong
\par DP email: jiayiceline.leong@digipen.edu
\par Course: csd3450
\date 15-1-2024
\brief  Gameplay script for player interaction with notes
****************************************************************************
***/
using ScriptAPI;
using System;
using System.Threading.Tasks.Dataflow;

public class Note_Script : Script
{
    //RigidBodyComponent rigidBodyComponent; //for raycast?
    AudioComponent clip;
    AudioSource player;
    //bool once;
    String[] voClip;

    [SerializeField]
    public string Note_Name;
    public string Note_Texture;
    public string Note_VO;
    public GameObject? _InteractUI;
    public static bool isNotePicked = false;

    public override void Awake()
    {
        Note_VO = "pc_checkreceipt";
        //once = true;
    }

    public override void Start()
    {
        //rigidBodyComponent = gameObject.GetComponent<RigidBodyComponent>();
        clip = gameObject.GetComponent<AudioComponent>();
        player = gameObject.GetComponent<AudioSource>();
    }

    public override void Update()
    {
        if (gameObject.GetComponent<RigidBodyComponent>().IsRayHit())
        {
            Console.WriteLine("Note");
            if (Input.GetKeyDown(Keycode.E))
            {
                Console.WriteLine("Picked up note");
                InventoryScript.addNoteIntoInventory(Note_Name, Note_Texture);
                gameObject.GetComponent<GraphicComponent>().SetView2D(true);
                gameObject.transform.SetPosition(new Vector3(-10000.0f, -10000.0f, -10000.0f));
                gameObject.transform.SetRotation(new Vector3(-0.0f, -0.0f, -0.0f));
                gameObject.SetActive(false);
                clip.play(Note_VO);
                GameplaySubtitles.counter = 14;
                isNotePicked = true;

            }
        }
        
        
    }

    public override void LateUpdate()
    {   
        if (gameObject.GetComponent<RigidBodyComponent>().IsRayHit())
        {
            _InteractUI.SetActive(true);
        }
        else
        {
            _InteractUI.SetActive(false);
        }
        
    }
}