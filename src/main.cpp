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

char officeFrame = 1;
int lookAngle = 0;
bool debugMenu = false;

bool usingCam = false;
bool togglingCam = false;

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

    LoadSpritesFromDir("assets/", sprites, backgrounds, errors);
    LoadAnimsFromDir("./assets", anims, errors);

    rlImGuiSetup(true);

    SetExitKey(-1);
    EditorInit();

    SetRenderSize(SCREENWIDTH, SCREENHEIGHT);

    srand(time(0));

    staticEntities.reserve(3);
    staticEntities.push_back({ 0, {0, 0}, {0, 0}, 0, {0, 0}, nullptr });
    staticEntities.push_back({ 0, {200, 200}, {50, 50}, 0, {0, 0}, &staticEntities[0].pos });
    staticEntities.push_back({ 0, {200, 400}, {50, 50}, 0, {0, 0}, &staticEntities[0].pos });

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
    if(checkBoxCollison(MousePositionStandard(), {1, 1}, {280, 620}, {720, 100}))
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
}

void Update()
{
    Vector2 mp = MousePositionStandard();

    if(IsKeyPressed(KEY_GRAVE))
        debugMenu = !debugMenu;

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

    staticEntities[0].pos.x = -320 + lookAngle;

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


void RenderUI()
{
    DrawSpriteFromVector("power5", {20, 670}, {60, 30}, sprites);

    DrawSpriteFromVector("cam", {280, 620}, {720, (float)((usingCam) ? -100 : 100)}, sprites);

    if(usingCam)
        DrawSpriteFromVector("map", {1080, 520}, {200, 200}, sprites);
}

void Render()
{
    BeginTextureMode(screen);
        ClearBackground(BLACK);

        if(!usingCam)
        {
            DrawAnimFromVector("office", {float(-320) + lookAngle, -200}, {1920, 1080}, anims, sprites);
        }
        else
        {
            
        }

        RenderStaticEntities();
    
        RenderUI();
    EndTextureMode();

    BeginDrawing();
        ClearBackground(BLACK);

        // draw scaled render texture to screen
        DrawSpriteDirect(screen.texture, {0, 0}, {(float)GetScreenWidth(), (float)GetScreenHeight()});

        rlImGuiBegin();

            if(debugMenu)
            {
                ImGui::SetNextWindowSize({0, 0});
                ImGui::Begin("Debug");
                ImGui::Text(std::to_string(officeFrame).c_str());
                ImGui::Text(std::to_string(lookAngle).c_str());
                ImGui::Text(std::format("{}, {}", MousePositionStandard().x, MousePositionStandard().y).c_str());
                ImGui::Text(std::format("Cam: {}", usingCam).c_str());
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