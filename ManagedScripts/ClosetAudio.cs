using ScriptAPI;
using System;

public class ClosetAudio : Script
{
    String[] ClosetVO;
    AudioComponent audioComponent;

    public override void Awake()
    {
        ClosetVO = new string[17];

        audioComponent = new AudioComponent();
    }

    public override void Start()
    {
        
    }

    public override void Update()
    {
        
    }
}