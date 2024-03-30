using ScriptAPI;
using System;

public class EventDining : Script
{
    public GameObject? door;

    public override void Update()
    {
    }

    public override void OnTriggerEnter(ColliderComponent collider)
    {
        gameObject.GetComponent<ColliderComponent>().SetEnabled(false);
    }
}