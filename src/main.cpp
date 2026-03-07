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
#include "menu.hpp"
#include "mouse.hpp"

#define SCREENWIDTH 1280
#define SCREENHEIGHT 720

std::vector<Sprite> sprites;
std::vector<Sprite> backgrounds;
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
int camRoom = 0;
int staticLevel = 128;
int power = 100;
int gameTime = 360;

int aiLevel = 1;

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

int doorLHeight = 0;
bool doorL = false;

int doorCHeight = 0;
bool doorC = false;

int doorRHeight = 0;
bool doorR = false;

int tx, ty, th, tw = 0;

int frames = 0;

void Update();
void Render();

int main()
{
    ForceErrorHandlerOverride(true, errors);
    std::cout << "GAME: Starting game\n";
    SetTraceLogLevel(TraceLogLevel::LOG_ERROR);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "FNAK");
    InitAudioDevice();
    SetTargetFPS(60);

    LoadSpritesFromDirMinimal("./assets/", sprites, errors);
    LoadAnimsFromDir("./assets", anims, errors);

    opensans = LoadFont("./assets/fonts/opensans.ttf");

    rlImGuiSetup(true);

    SetExitKey(-1);
    EditorInit();

    SetRenderSize(SCREENWIDTH, SCREENHEIGHT);

    srand(time(0));

    staticEntities.reserve(7);
    staticEntities.push_back({ 0, {0, 0}, {0, 0}, 0, {0, 0}, nullptr });

    // Camera Buttons
    staticEntities.push_back({ 0, {128, 298}, {36, 64}, 0, {0, 0}, &staticEntities[0].pos });
    staticEntities.push_back({ 0, {128, 373}, {36, 64}, 0, {0, 0}, &staticEntities[0].pos });
    staticEntities.push_back({ 0, {1184, 163}, {48, 48}, 0, {0, 0}, &staticEntities[0].pos });
    staticEntities.push_back({ 0, {1184, 219}, {48, 48}, 0, {0, 0}, &staticEntities[0].pos });
    staticEntities.push_back({ 0, {1754, 298}, {34, 62}, 0, {0, 0}, &staticEntities[0].pos });
    staticEntities.push_back({ 0, {1754, 374}, {34, 62}, 0, {0, 0}, &staticEntities[0].pos });

    // texture that is rendered to and then scaled
    screen = LoadRenderTexture(SCREENWIDTH, SCREENHEIGHT);

    while(!WindowShouldClose())
    {
        Update();
        Render();

        frames++;
    }

    CloseAudioDevice();
    CloseWindow();
    UnloadSpritesFromVector(sprites);
    UnloadSpritesFromVector(backgrounds);
    UnloadSoundsFromVector(sounds);
    rlImGuiShutdown();
    std::cout << "GAME: Shutting down\n";
    return 0;
}

int WinMain()
{
    return main();
}

void UpdateUI()
{
    if(checkBoxCollison(MousePositionStandard(), {1, 1}, {280, 620}, {720, 100}) && power > 0)
    {
        if(togglingCam == false)
        {
            usingCam = !usingCam;
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
    if(checkBoxCollison(MousePositionStandard(), {1, 1}, {1098, 661}, {29, 31}) && IsMouseButtonPressed(0)) { camRoom = 0; staticLevel = 230; }
    if(checkBoxCollison(MousePositionStandard(), {1, 1}, {1096, 617}, {65, 28}) && IsMouseButtonPressed(0)) { camRoom = 1; staticLevel = 230; }
    if(checkBoxCollison(MousePositionStandard(), {1, 1}, {1096, 540}, {86, 60}) && IsMouseButtonPressed(0)) { camRoom = 2; staticLevel = 230; }
    if(checkBoxCollison(MousePositionStandard(), {1, 1}, {1218, 556}, {32, 44}) && IsMouseButtonPressed(0)) { camRoom = 3; staticLevel = 230; }
    if(checkBoxCollison(MousePositionStandard(), {1, 1}, {1218, 626}, {32, 67}) && IsMouseButtonPressed(0)) { camRoom = 4; staticLevel = 230; }
}

void AIMove(int newLocation)
{
    if(camRoom == newLocation || camRoom == location)
        staticLevel = 255;

    location = newLocation;
}

void UpdateAI()
{
    if(frames % 420 == 0 && frames != 0)
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
            int choice = GetRandomValue(0, 7 - aiLevel);

            if(choice == 0)
            {
                if(aiLevel <= 3)
                    AIMove(2);
                else
                    AIMove((GetRandomValue(0, 1) == 0) ? 0 : 4);
            }
            else if(choice == 1)
                AIMove(0);
            else if(choice == 3)
                AIMove(4);
        }
    }
}

void Update()
{
    Vector2 mp = MousePositionStandard();

    if(IsKeyPressed(KEY_GRAVE))
        debugMenu = !debugMenu;

    // looking controls
    if(!usingCam)
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

    // power reduction
    if(frames % 300 == 0 && frames != 0)
    {
        if(gameTime > 0)
            gameTime -= 10;
        
        if(power > 0)
        {
            if(usingCam)
                power--;
            if(doorL)
                power--;
            if(doorC)
                power--;
            if(doorR)
                power--;
    
            power--;
    
            if(power <= 0)
            {
                power = 0;
                doorL = false;
                doorC = false;
                doorR = false;
                usingCam = false;
            }
        }
    }

    // move all world entities
    staticEntities[0].pos.x = -320 + lookAngle;

    // door buttons
    if(!usingCam && power > 0)
    {
        if(MouseClickingEntity(staticEntities[1])) { doorL = false; }
        if(MouseClickingEntity(staticEntities[2])) { doorL = true; }
        if(MouseClickingEntity(staticEntities[3])) { doorC = false; }
        if(MouseClickingEntity(staticEntities[4])) { doorC = true; }
        if(MouseClickingEntity(staticEntities[5])) { doorR = false; }
        if(MouseClickingEntity(staticEntities[6])) { doorR = true; }
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
    DrawSpriteFromVector("sidedoor", {160 + (float)lookAngle - 300, (float)doorLHeight - 500}, {-150, 600}, sprites);
    // DrawRectangle(980 + lookAngle + 300, doorRHeight - 500, 150, 600, GRAY);
    DrawSpriteFromVector("sidedoor", {980 + (float)lookAngle + 300, (float)doorRHeight - 500}, {150, 600}, sprites);
    // DrawRectangle(470 + lookAngle, doorCHeight - 150, 350, 250, GRAY);
    DrawSpriteFromVector("centerdoor", {470 + (float)lookAngle, (float)doorCHeight - 150}, {350, 250}, sprites);
}

void RenderEnemyOffice()
{
    if(location == 6)
    {
        DrawSpriteFromVector("stand", {160 + (float)lookAngle - 300, 200}, {150, 500}, sprites);
    }
    else if(location == 8)
    {
        DrawSpriteFromVector("stand", {980 + (float)lookAngle + 300, 200}, {-150, 500}, sprites);
    }
    else if(location == 7)
    {
        DrawSpriteFromVector("scare", {560 + (float)lookAngle, 150}, {150, 200}, sprites);
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
    DrawSpriteFromVector("power" + std::to_string((int)ceil(((float)power / 20))), {20, 650}, {100, 50}, sprites);
    DrawTextEx(opensans, std::format("{}%", power).c_str(), {130, 645}, 50, 2, WHITE);
    
    // timeslop
    DrawTextEx(opensans, std::format("{:02d}:{:02d}", ((gameTime > 300) ? 12 : (int)(6 - ceil(gameTime / 60)) - ((gameTime % 60 == 0) ? 0 : 1)), ((gameTime % 60 == 0) ? 0 : (60 - (int)(gameTime % 60)))).c_str(), {10, 10}, 50, 2, WHITE);

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

void Render()
{
    BeginTextureMode(screen);
        ClearBackground(BLACK);

        if(!usingCam)
        {

            DrawSpriteFromVector((power > 0) ? "hall" : "halldark", {float(-320) + lookAngle, -200}, {1920, 1080}, sprites);
            RenderEnemyOffice();
            RenderDoors();
            DrawAnimFromVector((power > 0) ? "office" : "officedark", {float(-320) + lookAngle, -200}, {1920, 1080}, anims, sprites, 255);
            
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