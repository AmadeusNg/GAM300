using ScriptAPI;
using System;

public class PaintingTrigger_Script : Script
{
    //public GameObject _InteractUI;
    //public GameObject _EventThree;
    //public GameObject paintingRenderer;
    public Painting_Script _PaintingScript;
    public InventoryScript _InventoryScript;
    public bool collided;

    [Header("AudioStuff")]
    public AudioSource playerVOSource;
    public AudioComponent voClip;
    //public Text subtitles;
    //public Image subtitlesBG;
    bool playedAudio;

    // Update is called once per frame
    public override void Update()
    {
        //if (Input.GetKeyDown(Keycode.E) && collided == true)
        //{
        //    if (!_PaintingScript.opened)
        //    {
        //        //_PaintingScript._PaintingAnimator.SetBool("Open", true);
        //        //_InteractUI.SetActive(false);
        //        _PaintingScript.opened = true;
        //    }
        //    else if (_PaintingScript.opened)
        //    {
        //        if (!playedAudio)
        //        {
        //            //playerVOSource.clip = voClip;
        //            //playerVOSource.Play();
        //            //subtitles.enabled = true;
        //            //subtitlesBG.enabled = true;
        //            //subtitles.text = "Martin (Internal): \"One down. Hopefully this is worth a lot.\"";
        //            playedAudio = true;
        //            //paintingRenderer.SetActive(false);
        //            _InventoryScript.paintingObjsInInventory[0] = "Painting_1";
        //        }
        //    }
        //}

        //if (!playerVOSource.isPlaying && playedAudio)
        //{
        //    //Trigger Event Three
        //    subtitles.enabled = false;
        //    subtitlesBG.enabled = false;
        //    _EventThree.SetActive(true);
        //    _InteractUI.SetActive(false);
        //    this.transform.parent.gameObject.SetActive(false);
        //}
    }
}
