using ScriptAPI;
using System;

public class FireplaceAudio : Script
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
        //commenting this out causes crash

        Vector3 fireplace_pos = GameObjectScriptFind("(Living room) Fireplace light").transform.GetPosition();
        audioComponent.set3DCoords(fireplace_pos, "fireplace");
        audioComponent.play("fireplace");
        //audioComponent.set3DCoords(200.0f, 200.0f, 200.0f);
        
    }

    public override void Update()
    {
        //commenting this out causes crash
        //audioComponent.play("fireplace");
    }
}