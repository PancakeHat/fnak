#include "raylib.h"
#include <vector>
#include "maps.hpp"
#include <iostream>
#include <cmath>
#include <format>
#include "graphics.hpp"
#include "rlimgui/rlImGui.h"
#include "rlimgui/imgui.h"
#include "editor.hpp"
#include <filesystem>
#include <stdio.h>
#include <stdlib.h>
#include "fileio.hpp"
#include "errors.hpp"
#include "mouse.hpp"
#include "ui.hpp"

#define SCREENWIDTH 1280
#define SCREENHEIGHT 720

std::vector<Sprite> sprites;
// std::vector<Sprite> backgrounds;
std::vector<GameSound> sounds;
std::vector<Anim> anims;
std::vector<Entity> staticEntities;

ErrorHandler errors;

RenderTexture2D screen;
Font opensans;

int lookAngle = 0;
bool debugMenu = false;

bool usingCam = false;
bool togglingCam = false;
int camRoom = 2;
int staticLevel = 128;
int power = 100;
int gameTime = 360;
int night = 1;

int aiLevel = 5;
int attackWindup = 180;
int moveCooldown = 180;
int jumpscare = 0;
int deathTimer = 300;

bool dead = false;

int location = 2;
/*
    classroom - 0
    commons - 1
    cafeteria - 2
    hallway - 3
    staircase - 4
    not shown - 5
    left door - 6
    center door - 7
    right door - 8
*/

Color globalTint = WHITE;

int doorLHeight = 0;
bool doorL = false;

int doorCHeight = 0;
bool doorC = false;

int doorRHeight = 0;
bool doorR = false;

float masterVolume = 0.7f;

// buttons
bool bRetry = false;
bool bQuitToTitle = false;
bool bNextNight = false;

// mm buttons
bool bContinue = false;
bool bNewGame = false;
bool bQuitGame = false;
bool bBack, bNight1, bNight2, bNight3, bNight4, bNight5, bNightmare = false;
bool bSettings = false;
bool settings = false;
bool bVUp, bVDown, bToggleFullscreen = true;

bool bPauseQuit, bResume = false;

int nightsBeaten = 0;
bool mainMenu = true;
bool choosingNight = false;
int save = 1;
bool saveFile = false;
bool fullscreen = false;

int tx, ty, th, tw = 0;

int introTimer = 300;
bool bSkipIntro = false;

bool paused = false;
int frames = 0;
bool gameRunning = true;

void Update();
void Render();
void MainMenu();
void LoadingScreen();
void PauseMenu();
void Intro();

// int main() {
//     InitWindow(800, 600, "test");
//     std::cout << "TEST\n";

//     while (!WindowShouldClose()) {
//         BeginDrawing();
//         ClearBackground(BLACK);
//         EndDrawing();
//     }

//     CloseWindow();
// }

int main()
{
    // ForceErrorHandlerOverride(true, errors);
    std::cout << "GAME: Starting game\n";
    SetTraceLogLevel(TraceLogLevel::LOG_ERROR);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    std::cout << "TEST\n";
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Five Nights at Korniluk's");
    std::cout << "TEST2\n";
    InitAudioDevice();
    SetTargetFPS(60);

    rlImGuiSetup(true);
    
    SetExitKey(-1);
    
    SetRenderSize(SCREENWIDTH, SCREENHEIGHT);
    // texture that is rendered to and then scaled
    screen = LoadRenderTexture(SCREENWIDTH, SCREENHEIGHT);
    srand(time(0));
    
    opensans = LoadFont("./assets/fonts/opensans.ttf");

    saveFile = LoadGameFromFile("save.txt", nightsBeaten, save, fullscreen, masterVolume);

    if(fullscreen)
        ToggleFullscreen();

    LoadingScreen();
    
    LoadSpritesFromDirMinimal("./assets/", sprites, errors);
    LoadAnimsFromDir("./assets/", anims, errors);
    LoadSoundsFromDirMinimal("./assets/sounds/", sounds, errors);

    staticEntities.reserve(7);
    staticEntities.push_back({ 0, {0, 0}, {0, 0}, 0, {0, 0}, nullptr });
    
    // Camera Buttons
    staticEntities.push_back({ 0, {128, 298}, {36, 64}, 0, {0, 0}, &staticEntities[0].pos });
    staticEntities.push_back({ 0, {128, 373}, {36, 64}, 0, {0, 0}, &staticEntities[0].pos });
    staticEntities.push_back({ 0, {1184, 163}, {48, 48}, 0, {0, 0}, &staticEntities[0].pos });
    staticEntities.push_back({ 0, {1184, 219}, {48, 48}, 0, {0, 0}, &staticEntities[0].pos });
    staticEntities.push_back({ 0, {1754, 298}, {34, 62}, 0, {0, 0}, &staticEntities[0].pos });
    staticEntities.push_back({ 0, {1754, 374}, {34, 62}, 0, {0, 0}, &staticEntities[0].pos });

    SetVectorSoundVolume("title", 0.5f, masterVolume, sounds);
    SetVectorSoundVolume("ambience", (usingCam) ? 0.15f : 0.3f, masterVolume, sounds);

    while(!WindowShouldClose() && gameRunning)
    {
        if(!mainMenu)
        {
            if(IsKeyPressed(KEY_ESCAPE) && introTimer <= 0)
            {
                paused = !paused;
                SetVectorSoundVolume("ambience", (paused || power <= 0) ? 0.0f : 0.3f, masterVolume, sounds);
            }

            if(!paused)
            {
                Update();
                Render();
            }
            else
            {
                if(introTimer <= 0)
                    PauseMenu();
                else
                    Intro();
            }
        }
        else
        {
            MainMenu();
        }

        frames++;
    }

    CloseAudioDevice();
    CloseWindow();
    UnloadSpritesFromVector(sprites);
    UnloadSoundsFromVector(sounds);
    rlImGuiShutdown();
    std::cout << "GAME: Shutting down\n";
    return 0;
}

int WinMain()
{
    return main();
}

void LoadingScreen()
{
    BeginTextureMode(screen);
        ClearBackground(BLACK);
        DrawCenteredText("Loading...", opensans, {640, 360}, 60, 3, WHITE);
    EndTextureMode();

    BeginDrawing();
        DrawSpriteDirect(screen.texture, {0, 0}, {(float)GetScreenWidth(), (float)GetScreenHeight()});
    EndDrawing();
}

void StartNight(int newNight)
{
    night = newNight;
    aiLevel = newNight;
    doorL = false;
    doorR = false;
    doorC = false;
    power = 100;
    gameTime = 360;
    dead = false;
    deathTimer = 300;
    jumpscare = 0;
    attackWindup = 180;
    usingCam = false;
    togglingCam = false;
    camRoom = 2;
    lookAngle = 0;
    location = 2;
    mainMenu = false;
    save = newNight;

    if(newNight > 1)
    {
        paused = false;
        introTimer = 0;
        SetVectorSoundVolume("title", 0.0f, masterVolume, sounds);
    }
    else
    {
        paused = true;
        introTimer = 300;
        SetVectorSoundVolume("title", 0.5f, masterVolume, sounds);
    }
    
    SaveGameToFile("save.txt", nightsBeaten, save, fullscreen, masterVolume);
    saveFile = true;

    SetVectorSoundVolume("ambience", 0.3f, masterVolume, sounds);
    
    if(night == 6)
    {
        globalTint = { 70, 70, 100, 255 };
    }
}

void SkipIntro()
{
    paused = false;
    introTimer = 0;
    SetVectorSoundVolume("title", 0.0f, masterVolume, sounds);
}

void Intro()
{
    if(!PlayingSound("title", sounds))
        PlaySoundFromVectorDontSet("title", sounds);

    if(introTimer > 1)
        introTimer--;
    else if (introTimer == 1)
    {
        paused = false;
        introTimer--;
        SetVectorSoundVolume("title", 0.0f, masterVolume, sounds);
    }

    int opacity = 255;
    if(introTimer >= 215 && introTimer <= 300)
        opacity -= ((introTimer - 215) * 3);

    if(introTimer >= 0 && introTimer <= 85)
    {
        opacity -= (255 - (introTimer * 3));
        SetVectorSoundVolume("title", (0.00588f * introTimer), masterVolume, sounds);
    }

    BeginTextureMode(screen);
        ClearBackground(BLACK);

        DrawSpriteFromVectorAlpha("intro", {0, 0}, {1280, 720}, sprites, opacity);

        DrawAnimFromVector("noise", {0, 0}, {1280, 720}, anims, sprites, 100);

        RenderTextButton(opensans, {10, 670}, 40, 3, "Skip", ">Skip", SkipIntro, bSkipIntro, sounds, masterVolume);
    EndTextureMode();

    BeginDrawing();
        ClearBackground(BLACK);
        DrawSpriteDirect(screen.texture, {0, 0}, {(float)GetScreenWidth(), (float)GetScreenHeight()});
    EndDrawing();
}

void ContinueNight()
{
    StartNight(save);
}

void ChooseNightBack()
{
    choosingNight = false;
    settings = false;
}

void NewGame()
{
    choosingNight = true;
}

void Quit()
{
    gameRunning = false;
}

void Settings()
{
    settings = true;
}

void VolumeUp()
{
    masterVolume += 0.05f;
    SaveGameToFile("save.txt", nightsBeaten, save, fullscreen, masterVolume);

    SetVectorSoundVolume("title", 0.5f, masterVolume, sounds);

    if(masterVolume > 1.0f)
        masterVolume = 1.0f;
    else if(masterVolume < 0.0f)
        masterVolume = 0.0f;
}

void VolumeDown()
{
    masterVolume -= 0.05f;
    SaveGameToFile("save.txt", nightsBeaten, save, fullscreen, masterVolume);

    SetVectorSoundVolume("title", 0.5f, masterVolume, sounds);

    if(masterVolume > 1.0f)
        masterVolume = 1.0f;
    else if(masterVolume < 0.0f)
        masterVolume = 0.0f;
}

void ToggleFullscreenSetting()
{
    fullscreen = !fullscreen;
    ToggleFullscreen();
    SaveGameToFile("save.txt", nightsBeaten, save, fullscreen, masterVolume);
}

void MainMenu()
{
    if(!PlayingSound("title", sounds))
        PlaySoundFromVectorDontSet("title", sounds);

    BeginTextureMode(screen);
        ClearBackground(BLACK);

        DrawAnimFromVector("noise", {0, 0}, {1280, 720}, anims, sprites, 128);

        if(choosingNight == false && settings == false)
        {
            DrawTextEx(opensans, "Five Nights", {10, 10}, 70, 3, WHITE);
            DrawTextEx(opensans, "at Korniluk's", {10, 75}, 70, 3, WHITE);
    
            RenderTextButtonAllowed(opensans, {10, 180}, 60, 3, (saveFile) ? ("Continue Night " + std::to_string(save)) : "Continue", (">Continue Night " + std::to_string(save)), ContinueNight, bContinue, saveFile, sounds, masterVolume);
            RenderTextButton(opensans, {10, 240}, 60, 3, "New Game", ">New Game", NewGame, bNewGame, sounds, masterVolume);
            RenderTextButton(opensans, {10, 300}, 60, 3, "Settings", ">Settings", Settings, bSettings, sounds, masterVolume);
            RenderTextButton(opensans, {10, 360}, 60, 3, "Quit", ">Quit", Quit, bQuitGame, sounds, masterVolume);

            DrawSpriteFromVectorAlpha("title", {920, 280}, {400, 550}, sprites, 130);
        }
        else if(choosingNight)
        {
            DrawTextEx(opensans, "Choose Night", {10, 10}, 70, 3, WHITE);
    
            RenderTextButton(opensans, {10, 180}, 60, 3, "Back", ">Back", ChooseNightBack, bBack, sounds, masterVolume);
            RenderTextButtonAllowed1i(opensans, {10, 240}, 60, 3, "Night 1", ">Night 1", StartNight, 1, bNight1, true, sounds, masterVolume);
            RenderTextButtonAllowed1i(opensans, {10, 300}, 60, 3, "Night 2", ">Night 2", StartNight, 2, bNight2, (nightsBeaten > 0), sounds, masterVolume);
            RenderTextButtonAllowed1i(opensans, {10, 360}, 60, 3, "Night 3", ">Night 3", StartNight, 3, bNight3, (nightsBeaten > 1), sounds, masterVolume);
            RenderTextButtonAllowed1i(opensans, {10, 420}, 60, 3, "Night 4", ">Night 4", StartNight, 4, bNight4, (nightsBeaten > 2), sounds, masterVolume);
            RenderTextButtonAllowed1i(opensans, {10, 480}, 60, 3, "Night 5", ">Night 5", StartNight, 5, bNight5, (nightsBeaten > 3), sounds, masterVolume);
            RenderTextButtonAllowed1i(opensans, {10, 540}, 60, 3, "Nightmare", ">Nightmare", StartNight, 6, bNightmare, (nightsBeaten > 4), sounds, masterVolume);
        }
        else if(settings)
        {
            DrawTextEx(opensans, "Settings", {10, 10}, 70, 3, WHITE);
    
            RenderTextButton(opensans, {10, 180}, 60, 3, "Back", ">Back", ChooseNightBack, bBack, sounds, masterVolume);
            RenderTextButton(opensans, {10, 240}, 60, 3, " - ", "[-]", VolumeDown, bVDown, sounds, masterVolume);
            DrawTextEx(opensans, std::format("Volume: {:.0f}%", (masterVolume * 100)).c_str(), {70, 240}, 60, 3, WHITE);
            // DrawTextEx(opensans, std::format("Volume: {}%", (masterVolume * 100)).c_str(), {70, 240}, 60, 3, WHITE);
            RenderTextButton(opensans, {395, 240}, 60, 3, " + ", "[+]", VolumeUp, bVUp, sounds, masterVolume);
            RenderTextButton(opensans, {10, 300}, 60, 3, "Toggle Fullscreen", ">Toggle Fullscreen", ToggleFullscreenSetting, bToggleFullscreen, sounds, masterVolume);
        }

    EndTextureMode();

    BeginDrawing();
        ClearBackground(BLACK);
        DrawSpriteDirect(screen.texture, {0, 0}, {(float)GetScreenWidth(), (float)GetScreenHeight()});
    EndDrawing();
}

void QuitToTitle()
{
    mainMenu = true;
    globalTint = WHITE;
    paused = false;
    SetVectorSoundVolume("clock", 0.0f, masterVolume, sounds);
    SetVectorSoundVolume("title", 0.5f, masterVolume, sounds);
}

void Resume()
{
    paused = false;
    if(power >= 0)
        SetVectorSoundVolume("ambience", 0.3f, masterVolume, sounds);
}

void PauseMenu()
{
    BeginTextureMode(screen);
        ClearBackground(BLACK);
        DrawAnimFromVector("noise", {0, 0}, {1280, 720}, anims, sprites, 128);

        DrawCenteredText("Paused", opensans, { 640, 330 }, 100, 3, WHITE);
        RenderTextButtonCentered(opensans, {640, 450}, 60, 3, "Resume", ">Resume", Resume, bResume, sounds, masterVolume);
        RenderTextButtonCentered(opensans, {640, 510}, 60, 3, "Quit to Title", ">Quit to Title", QuitToTitle, bPauseQuit, sounds, masterVolume);
    EndTextureMode();

    BeginDrawing();
        ClearBackground(BLACK);
        DrawSpriteDirect(screen.texture, {0, 0}, {(float)GetScreenWidth(), (float)GetScreenHeight()});
    EndDrawing();
}

void UpdateUI()
{
    if(checkBoxCollison(MousePositionStandard(), {1, 1}, {280, 620}, {720, 100}) && power > 0)
    {
        if(togglingCam == false && !dead && power > 0)
        {
            usingCam = !usingCam;
            SetVectorSoundVolume("ambience", (usingCam) ? 0.15f : 0.3f, masterVolume, sounds);
            PlaySoundFromVector("cam", 0.4f, masterVolume, sounds);
        }
        togglingCam = true;
    }
    else if(togglingCam == true)
    {
        togglingCam = false;
    }

    if(debugMenu)
    {
        if(IsKeyDown(KEY_RIGHT))
            tx++;
    
        if(IsKeyDown(KEY_LEFT))
            tx--;
    
        if(IsKeyDown(KEY_UP))
            ty--;
    
        if(IsKeyDown(KEY_DOWN))
            ty++;
    }

    // camera selector slop
    if(checkBoxCollison(MousePositionStandard(), {1, 1}, {1098, 661}, {29, 31}) && IsMouseButtonPressed(0)) { camRoom = 0; staticLevel = 230; PlaySoundFromVector("click", 0.5f, masterVolume, sounds); }
    if(checkBoxCollison(MousePositionStandard(), {1, 1}, {1096, 617}, {65, 28}) && IsMouseButtonPressed(0)) { camRoom = 1; staticLevel = 230; PlaySoundFromVector("click", 0.5f, masterVolume, sounds); }
    if(checkBoxCollison(MousePositionStandard(), {1, 1}, {1096, 540}, {86, 60}) && IsMouseButtonPressed(0)) { camRoom = 2; staticLevel = 230; PlaySoundFromVector("click", 0.5f, masterVolume, sounds); }
    if(checkBoxCollison(MousePositionStandard(), {1, 1}, {1218, 556}, {32, 44}) && IsMouseButtonPressed(0)) { camRoom = 3; staticLevel = 230; PlaySoundFromVector("click", 0.5f, masterVolume, sounds); }
    if(checkBoxCollison(MousePositionStandard(), {1, 1}, {1218, 626}, {32, 67}) && IsMouseButtonPressed(0)) { camRoom = 4; staticLevel = 230; PlaySoundFromVector("click", 0.5f, masterVolume, sounds); }
}

void AIMove(int newLocation)
{
    if(newLocation == 7 && !usingCam)
        return;

    if(newLocation == 6 && lookAngle > 30 && !usingCam)
        newLocation = 8;

    if(newLocation == 8 && lookAngle < -30 && !usingCam)
        newLocation = 6;

    if(camRoom == newLocation || camRoom == location)
        staticLevel = 255;

    location = newLocation;

    if(night >= 5)
        moveCooldown = 90;
    else
        moveCooldown = 180;

    if(!dead)
    {
        float volume;
        std::string sound = "steps";

        if(newLocation == 2)
            volume = 0.3f;

        if(newLocation == 3 || newLocation == 1)
            volume = 0.5f;

        if(newLocation == 0 || newLocation == 4)
            volume = 0.8f;

        if(newLocation == 7)
            sound = "vent";

        if(newLocation == 6 || newLocation == 8)
            sound = "door";

        if(sound == "steps")
            PlaySoundFromVector(sound, volume * ((usingCam) ? 1.0f : 0.5f), masterVolume, sounds);
        else
            PlaySoundFromVector(sound, 1.0f, masterVolume, sounds);
    }
}

void AIAttack()
{
    if(!PlayingSound("bang", sounds) && !dead)
        PlaySoundFromVector("bang", 1.0f, masterVolume, sounds);

    if(power >= 0)
    {
        if(location == 6 && doorL == true)
            return;
        if(location == 7 && doorC == true)
            return;
        if(location == 8 && doorR == true)
            return;
    
    }

    if(!dead)
        PlaySoundFromVector("jumpscare", 0.8f, masterVolume, sounds);
        
    dead = true;
    usingCam = false;
    SetVectorSoundVolume("ambience", 0.0f, masterVolume, sounds);
}

void UpdateAI()
{
    if(dead && jumpscare < 30)
        jumpscare += 5;

    if(dead && deathTimer > 0)
        deathTimer--;

    if(location == 6 || location == 7 || location == 8)
    {
        if(attackWindup > 0)
            attackWindup--;
        
        if(attackWindup <= 0)
        {
            AIAttack();
        }
    }

    if(moveCooldown > 0)
        moveCooldown--;

    if(frames % 300 == 0 && frames != 0 && moveCooldown == 0)
    {
        if(location == 2)
        {
            int choice = GetRandomValue(0, 6 - aiLevel);

            if(choice == 0)
                AIMove(1);
            else if(choice == 1)
                AIMove(3);
        }
        else if(location == 3)
        {
            int choice = GetRandomValue(0, 6 - aiLevel);

            if(choice == 0)
            {
                if(aiLevel <= 3)
                    AIMove(2);
                else
                    AIMove(4);
            }
            else if(choice == 1)
                AIMove(4);
        }
        else if(location == 1)
        {
            int choice = GetRandomValue(0, 8 - aiLevel);

            if(choice == 0)
            {
                if(aiLevel <= 3)
                    AIMove(2);
                else
                    AIMove((GetRandomValue(0, 1) == 0) ? 0 : 4);
            }
            else if(choice == 1)
                AIMove(0);
            else if(choice == 2)
                AIMove(4);
            else if(choice == 3 && aiLevel >= 3)
                AIMove(7);
        }
        else if(location == 0)
        {
            int choice = GetRandomValue(0, 6 - aiLevel);

            if(choice == 0)
            {
                if(aiLevel <= 3)
                    AIMove(1);
                else
                    AIMove(6);
            }
            else if(choice == 1)
                AIMove(6);
        }
        else if(location == 4)
        {
            int choice = GetRandomValue(0, 8 - aiLevel);

            if(choice == 0)
            {
                if(aiLevel <= 3)
                    AIMove(3);
                else
                    AIMove((GetRandomValue(0, 1) == 0) ? 8 : 7);
            }
            else if(choice == 1)
            {
                if(aiLevel <= 3)
                    AIMove(1);
                else
                    AIMove(8);
            }
            else if(choice == 2)
                AIMove(8);
            else if(choice == 3 && aiLevel >= 3)
                AIMove(7);
            
        }
        else if(location == 6)
        {
            if(attackWindup <= 0 && doorL == true)
            {
                if(aiLevel < 5)
                    AIMove(2);
                else
                    AIMove((GetRandomValue(0, 1) == 0) ? 0 : 1);

                attackWindup = 120;
            }
        }
        else if(location == 7)
        {
            if(attackWindup <= 0 && doorC == true)
            {
                if(aiLevel < 5)
                    AIMove(2);
                else
                    AIMove(1);
                    
                attackWindup = 120;
            }
        }
        else if(location == 8)
        {
            if(attackWindup <= 0 && doorR == true)
            {
                if(aiLevel < 5)
                    AIMove((GetRandomValue(0, 1) == 0) ? 2 : 3);
                else
                    AIMove(4);
                    
                attackWindup = 120;
            }
        }
    }
}

void Update()
{
    Vector2 mp = MousePositionStandard();

    if(!dead && !PlayingSound("ambience", sounds))
        PlaySoundFromVectorDontSet("ambience", sounds);

    if(IsKeyPressed(KEY_GRAVE))
        debugMenu = !debugMenu;

    // looking controls
    if(!usingCam && !dead)
    {
        if(mp.x >= 1080 && lookAngle >= -300)
        {
            lookAngle -= 15;
            if(lookAngle < -300)
                lookAngle = -300;
        }
    
        if(mp.x <= 200 && lookAngle <= 300)
        {
            lookAngle += 15;
            if(lookAngle > 300)
                lookAngle = 300;
    
        }
    }
    
    // static fade
    if(staticLevel > 128)
    {
        staticLevel -= 5;
        
        if(staticLevel < 128)
            staticLevel = 128;
    }

    if(frames % 300 == 0 && frames != 0 && !dead)
    {
        if(gameTime > 0)
            gameTime -= 10;
    }

    if(frames % 300 == 0 && frames != 0 && !dead)
    {
        // if(gameTime > 0)
        //     gameTime -= 10;

        if(gameTime == 0)
        {
            dead = true;
            deathTimer = 0;
            SetVectorSoundVolume("ambience", 0.0f, masterVolume, sounds);
            PlaySoundFromVector("clock", 0.7f, masterVolume, sounds);
            
            if(night == nightsBeaten + 1)
                nightsBeaten++;

            save++;
            if(save > 6)
                save = 6;

            SaveGameToFile("save.txt", nightsBeaten, save, fullscreen, masterVolume);
            saveFile = true;
        }
    }

    // power reduction
    if(frames % 300 == 0 && frames != 0 && !dead)
    {   
        if(power > 0)
        {
            if(usingCam)
                power -= 2;
            if(doorL)
                power -= 5;
            if(doorC)
                power -= 5;
            if(doorR)
                power -= 5;
    
            power--;
    
            if(power <= 0)
            {
                power = 0;
                doorL = false;
                doorC = false;
                doorR = false;
                usingCam = false;
                attackWindup = 600;
                location = 7;
                globalTint = { 70, 70, 100, 255 };
                SetVectorSoundVolume("ambience", 0.0f, masterVolume, sounds);
                PlaySoundFromVector("shutdown", 0.7f, masterVolume, sounds);
            }
        }
    }

    // move all world entities
    staticEntities[0].pos.x = -320 + lookAngle;

    // door buttons
    if(!usingCam && power > 0 && !dead)
    {
        if(MouseClickingEntity(staticEntities[1])) { doorL = false; PlaySoundFromVector("powerdoor", 0.5f, masterVolume, sounds); }
        if(MouseClickingEntity(staticEntities[2])) { doorL = true; PlaySoundFromVector("powerdoor", 0.5f, masterVolume, sounds); }
        if(MouseClickingEntity(staticEntities[3])) { doorC = false; PlaySoundFromVector("powerdoor", 0.5f, masterVolume, sounds); }
        if(MouseClickingEntity(staticEntities[4])) { doorC = true; PlaySoundFromVector("powerdoor", 0.5f, masterVolume, sounds); }
        if(MouseClickingEntity(staticEntities[5])) { doorR = false; PlaySoundFromVector("powerdoor", 0.5f, masterVolume, sounds); }
        if(MouseClickingEntity(staticEntities[6])) { doorR = true; PlaySoundFromVector("powerdoor", 0.5f, masterVolume, sounds); }
    }

    // door animations
    if(doorL == true && doorLHeight < 600)
        doorLHeight += 50;
    else if(doorL == false && doorLHeight > 0)
        doorLHeight -= 50;

    if(doorR == true && doorRHeight < 600)
        doorRHeight += 50;
    else if(doorR == false && doorRHeight > 0)
        doorRHeight -= 50;

    if(doorC == true && doorCHeight < 250)
        doorCHeight += 50;
    else if(doorC == false && doorCHeight > 0)
        doorCHeight -= 50;

    UpdateAI();
    UpdateUI();
}

void RenderStaticEntities()
{
    for(Entity e : staticEntities)
    {
        if(debugMenu) // Debug entity hitbox
            DrawRectangleLines(((e.parent == nullptr) ? 0 : (*e.parent).x) + e.pos.x, ((e.parent == nullptr) ? 0 : (*e.parent).y) + e.pos.y, e.size.x, e.size.y, RED);
    }
}

void RenderDoors()
{
    // DrawRectangle(160 + lookAngle - 300, doorLHeight - 500, 150, 600, GRAY);
    DrawSpriteFromVectorTint("sidedoor", {160 + (float)lookAngle - 300, (float)doorLHeight - 500}, {-150, 600}, sprites, globalTint);
    // DrawRectangle(980 + lookAngle + 300, doorRHeight - 500, 150, 600, GRAY);
    DrawSpriteFromVectorTint("sidedoor", {980 + (float)lookAngle + 300, (float)doorRHeight - 500}, {150, 600}, sprites, globalTint);
    // DrawRectangle(470 + lookAngle, doorCHeight - 150, 350, 250, GRAY);
    DrawSpriteFromVectorTint("centerdoor", {470 + (float)lookAngle, (float)doorCHeight - 150}, {350, 250}, sprites, globalTint);
}

void RenderEnemyOffice()
{
    if(location == 6)
    {
        DrawSpriteFromVectorTint("stand", {160 + (float)lookAngle - 300, 200}, {150, 500}, sprites, globalTint);
    }
    else if(location == 8)
    {
        DrawSpriteFromVectorTint("stand", {980 + (float)lookAngle + 300, 200}, {-150, 500}, sprites, globalTint);
    }
    else if(location == 7)
    {
        DrawSpriteFromVectorTint("scare", {560 + (float)lookAngle, 150}, {150, 200}, sprites, globalTint);
    }
}

void RenderDoorDebug()
{
    DrawRectangleLines(160 + lookAngle - 300, doorLHeight - 500, 150, 600, GREEN);
    DrawRectangleLines(980 + lookAngle + 300, doorRHeight - 500, 150, 600, GREEN);
    DrawRectangleLines(475 + lookAngle, doorCHeight - 150, 350, 250, GREEN);
}

void RenderUI()
{
    if(deathTimer > 255)
    {
        DrawSpriteFromVector("power" + std::to_string((int)ceil(((float)power / 20))), {20, 650}, {100, 50}, sprites);
        DrawTextEx(opensans, std::format("{}%", power).c_str(), {130, 645}, 50, 2, WHITE);
        
        // timeslop
        DrawTextEx(opensans, std::format("{:02d}:{:02d}", ((gameTime > 300) ? 12 : (int)(6 - ceil(gameTime / 60)) - ((gameTime % 60 == 0) ? 0 : 1)), ((gameTime % 60 == 0) ? 0 : (60 - (int)(gameTime % 60)))).c_str(), {10, 10}, 50, 2, WHITE);
        // DrawTextEx(opensans, std::format("{}:{}", ((gameTime > 300) ? 12 : (int)(6 - ceil(gameTime / 60)) - ((gameTime % 60 == 0) ? 0 : 1)), ((gameTime % 60 == 0) ? 0 : (60 - (int)(gameTime % 60)))).c_str(), {10, 10}, 50, 2, WHITE);
    
        DrawSpriteFromVector("cam", {280, 620}, {720, (float)((usingCam) ? -100 : 100)}, sprites);
    
        if(usingCam)
        {
            DrawSpriteFromVector("map", {1080, 520}, {200, 200}, sprites);
    
            if(camRoom == 0)
                DrawRectangle(1098, 661, 29, 31, {200, 200, 0, 128});
            else if(camRoom == 1)
                DrawRectangle(1096, 617, 65, 28, {200, 200, 0, 128});
            else if(camRoom == 2)
                DrawRectangle(1098, 540, 86, 60, {200, 200, 0, 128});
            else if(camRoom == 3)
                DrawRectangle(1218, 556, 32, 44, {200, 200, 0, 128});
            else if (camRoom == 4)
                DrawRectangle(1218, 626, 32, 67, {200, 200, 0, 128});
        }
    
        if(debugMenu)
            DrawRectangle(tx, ty, tw, th, { 255, 0, 0, 200 });
    }
}

void RetryNight()
{
    StartNight(night);
}

void NextNight()
{
    StartNight(night + 1);
}

void Render()
{
    BeginTextureMode(screen);
        ClearBackground(BLACK);

        if(deathTimer > 255)
        {
            if(!usingCam)
            {
    
                if(night < 6)
                    DrawSpriteFromVector((power > 0) ? "hall" : "halldark", {float(-320) + lookAngle, -200}, {1920, 1080}, sprites);
                else
                    DrawSpriteFromVector("halldark", {float(-320) + lookAngle, -200}, {1920, 1080}, sprites);
    
                if(!dead)
                    RenderEnemyOffice();
    
                RenderDoors();

                if(night < 6)
                    DrawAnimFromVector((power > 0) ? "office" : "officedark", {float(-320) + lookAngle, -200}, {1920, 1080}, anims, sprites, 255);
                else
                    DrawAnimFromVector("officedark", {float(-320) + lookAngle, -200}, {1920, 1080}, anims, sprites, 255);
                
                //door buttons
                if(power > 0)
                {
                    if(doorL)
                        DrawRectOnEntity(staticEntities[2], {255, 100, 100, 180});
                    else
                        DrawRectOnEntity(staticEntities[1], {100, 255, 100, 140});
                    
                    if(doorC)
                        DrawRectOnEntity(staticEntities[4], {255, 100, 100, 180});
                    else
                        DrawRectOnEntity(staticEntities[3], {100, 255, 100, 140});
                    
                    if(doorR)
                        DrawRectOnEntity(staticEntities[6], {255, 100, 100, 180});
                    else
                        DrawRectOnEntity(staticEntities[5], {100, 255, 100, 140});
                }
            }
            else
            {
                if(camRoom == 0)
                    DrawSpriteFromVector((location == 0) ? "classroom1" : "classroom", {0, 0}, {1280, 720}, sprites);
                else if(camRoom == 1)
                    DrawSpriteFromVector((location == 1) ? "hallway1" : "hallway", {0, 0}, {1280, 720}, sprites);
                else if(camRoom == 2)
                    DrawSpriteFromVector((location == 2) ? "cafeteria1" : "cafeteria", {0, 0}, {1280, 720}, sprites);
                else if(camRoom == 3)
                    DrawSpriteFromVector((location == 3) ? "commons1" : "commons", {0, 0}, {1280, 720}, sprites);
                else if(camRoom == 4)
                    DrawSpriteFromVector((location == 4) ? "staircase1" : "staircase", {0, 0}, {1280, 720}, sprites);
                
                DrawAnimFromVector("noise", {0, 0}, {1280, 720}, anims, sprites, staticLevel);
            }
    
            DrawSpriteFromVectorTint("attack", {380 + (float)(5 * sin(frames * jumpscare)), 720 - (20 * (float)jumpscare)}, {600, 600}, sprites, globalTint);
        }

        if(deathTimer <= 255)
        {
            if(deathTimer > 0)
                DrawAnimFromVector("noise", {0, 0}, {1280, 720}, anims, sprites, (deathTimer));

            if(gameTime > 0)
                DrawCenteredText("You Died", opensans, { 640, 330 }, 100, 3, WHITE);
            else
                DrawCenteredText("6:00 AM", opensans, { 640, 330 }, 100, 3, WHITE);

            if(gameTime > 0)
                RenderTextButtonCentered(opensans, {640, 450}, 60, 3, "Retry Night", ">Retry Night", RetryNight, bRetry, sounds, masterVolume);
            else if(gameTime <= 0 && night < 5)
                RenderTextButtonCentered(opensans, {640, 450}, 60, 3, "Next Night", ">Next Night", NextNight, bNextNight, sounds, masterVolume);

            RenderTextButtonCentered(opensans, {640, 510}, 60, 3, "Quit to Title", ">Quit to Title", QuitToTitle, bQuitToTitle, sounds, masterVolume);
        }
        
        if(debugMenu)
        {
            RenderDoorDebug();
            RenderStaticEntities();
        }
    
        RenderUI();
    EndTextureMode();

    BeginDrawing();
        ClearBackground(BLACK);

        // draw scaled render texture to screen
        DrawSpriteDirect(screen.texture, {0, 0}, {(float)GetScreenWidth(), (float)GetScreenHeight()});

        rlImGuiBegin();

            if(debugMenu)
            {
                ImGui::SetNextWindowSize({300, 0});
                ImGui::Begin("Debug");
                ImGui::Text(std::to_string(frames).c_str());
                ImGui::Text(std::to_string(lookAngle).c_str());
                ImGui::Text(std::format("{}, {}", MousePositionStandard().x, MousePositionStandard().y).c_str());
                ImGui::Text(std::format("Cam: {}", usingCam).c_str());
                ImGui::Text(std::format("Noise: {}", staticLevel).c_str());
                ImGui::Text(std::format("L: {}, {} C: {}, {} R: {}, {}", doorL, doorLHeight, doorC, doorCHeight, doorR, doorRHeight).c_str());
                ImGui::Text(std::format("Game time: {}", gameTime).c_str());
                ImGui::Text(std::format("Location: {}", location).c_str());
                ImGui::Text(std::format("AI Level: {}", aiLevel).c_str());
                ImGui::Text(std::format("Move Cooldown: {}", moveCooldown).c_str());
                ImGui::Text(std::format("Attack Windup: {}", attackWindup).c_str());
                ImGui::Text(std::format("Night: {}", night).c_str());
                ImGui::Text(std::format("Paused: {}", paused).c_str());
                ImGui::Text(std::format("Intro Timer: {}", introTimer).c_str());

                // ImGui::SliderInt("X", &tx, 0, 1280);
                // ImGui::SliderInt("Y", &ty, 0, 720);
                // ImGui::SliderInt("W", &tw, 0, 500);
                // ImGui::SliderInt("H", &th, 0, 500);
                ImGui::End();
            }

            // error window
            if(errors.activeError)
            {
                ImGui::SetNextWindowSize({0, 0});
                if(ImGui::Begin(errors.isFatal ? "Fatal Error" : "Nonfatal Error"))   
                {
                    ImGui::TextColored({255, 0, 0, 255}, errors.errorMessage.c_str());
                    if(ImGui::Button("OK"))
                    {
                            errors.activeError = false;
                            errors.overridable = true;
                    }

                }
                ImGui::End();
            }
        rlImGuiEnd();
    EndDrawing();
}