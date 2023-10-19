using ScriptAPI;
using System;

public class Test : Script
{
    public override void Awake() { }

    public override void OnEnable() 
    {
        Console.WriteLine("Enabled");
    }

    public override void Start()
    {
        Console.WriteLine("Start Test");
    }

    public override void Update()
    {
        Console.WriteLine("Aye Lmao");
        TransformComponent tf = GetTransformComponent();
        tf.SetPositionX(3.0f);
    }

    public override void LateUpdate() { }

    public override void OnDisable() 
    {
        Console.WriteLine("Disabled");
    }

    public override void OnDestroy() 
    {
        Console.WriteLine("Exit");
    }
}
