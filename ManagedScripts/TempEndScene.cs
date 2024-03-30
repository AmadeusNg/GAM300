using ScriptAPI;
using System;
public class TempEndScene : Script
{
    private bool fadeIn = false;
    private bool fadeOut = true;
    private bool once = true;
    private float incrementFading = Time.deltaTime / 5f;
    readonly string runningSFX = "end_grassrunning";
    readonly string monsterSFX = "mon_death";
    AudioComponent audio;
    private float waitTimer;
    public override void Awake()
    {
        //audio = new AudioComponent();
        audio = gameObject.GetComponent<AudioComponent>();
        audio.play(monsterSFX);
        audio.FadeOut(3, monsterSFX);
        waitTimer = 0.0f;
    }
    public override void Start()
    {

    }
    public override void Update()
    {
        if(once)
        {
            audio.play(runningSFX);
            once = false;
        }

        if (fadeIn == true)
        {
            float alpha = gameObject.GetComponent<UISpriteComponent>().getColourAlpha();
            alpha -= incrementFading;
            alpha = Mathf.Clamp(alpha, 0, 1);
            gameObject.GetComponent<UISpriteComponent>().setColourAlpha(alpha);
            if (alpha <= 0)
            {
                if (waitTimer <= 0.0f)
                {
                    fadeIn = false;
                    //SceneLoader.LoadTempJumpscare();
                    SceneLoader.LoadMainMenu();
                }
                waitTimer -= Time.deltaTime;
            }
        }
        if (fadeOut == true)
        {            
            float alpha = gameObject.GetComponent<UISpriteComponent>().getColourAlpha();
            alpha += incrementFading;
            alpha = Mathf.Clamp(alpha, 0, 1);
            gameObject.GetComponent<UISpriteComponent>().setColourAlpha(alpha);
            if (alpha >= 1)
            {
                fadeOut = false;
                fadeIn = true;
                audio.FadeOut(3, runningSFX);
            }
        }
    }
    public override void LateUpdate()
    {

    }

}