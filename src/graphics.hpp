#include "raylib.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "errors.hpp"
#include <format>

#pragma once

struct Sprite {
    Texture2D img;
    std::string id;
    int width;
    int height;
};

struct Anim {
    std::vector<std::string> frames;
    std::string id;
    int numFrames;
    int fps;
    int frameCount;
    int frame;
};

void LoadAnimToVector(std::string fileName, std::string id, std::vector<Anim>& anims, ErrorHandler& eh)
{
    Anim anim;
    anim.id = id;

    anim.frame = 0;
    anim.frameCount = 0;

    std::ifstream file(fileName);

    if(!file.is_open())
    {
        ThrowNewError(std::format("Failed to load anim {}", id), ERROR_NONFATAL, true, eh);
    }

    std::string line;

    std::getline(file, line);
    anim.numFrames = std::stoi(line);

    std::getline(file, line);
    anim.fps = std::stoi(line);

    while(std::getline(file, line))
    {
        anim.frames.push_back(line);
    }

    file.close();

    // std::cout << anim.numFrames << std::endl;
    // std::cout << anim.fps << std::endl;

    // for(int i = 0; i < anim.frames.size(); i++)
    // {
    //     std::cout << anim.frames[i] << "\n";
    // }

    anims.push_back(anim);
}

void LoadSpriteToVector(std::string fileName, std::string id, std::vector<Sprite>& sprites, ErrorHandler& eh)
{
    Sprite sprite;
    sprite.id = id;

    std::string normalName = fileName;

    Image i = LoadImage(normalName.c_str()); 

    if(i.width == 0)
    {
        ThrowNewError(std::format("Failed to load sprite {}", id), ERROR_NONFATAL, true, eh);
    }

    sprite.width = i.width;
    sprite.height = i.height;

    sprite.img = LoadTextureFromImage(i);
    UnloadImage(i);

    sprites.push_back(sprite);
}

// void LoadSpriteToVector(std::string fileName, std::string id, std::vector<Sprite>& sprites)
// {
//     Sprite sprite;
//     sprite.id = id;

//     std::string overrideName = "./override/" + fileName;
//     std::string normalName = "./assets/" + fileName;

//     Image i = LoadImage(overrideName.c_str());
//     if(i.width == 0)
//     {
//         i = LoadImage(normalName.c_str()); 
//         std::cout << "GAME: Registered texture " << normalName << "\n";
//     }
//     else 
//     {
//         std::cout << "GAME: Registered texture " << overrideName << "\n";
//     }

//     sprite.width = i.width;
//     sprite.height = i.height;

//     sprite.img = LoadTextureFromImage(i);
//     UnloadImage(i);

//     sprites.push_back(sprite);
// }

void DrawCenteredText(std::string text, Font font, Vector2 pos, float size, float spacing, Color color)
{
    Vector2 s = MeasureTextEx(font, text.c_str(), size, spacing);

    DrawTextEx(font, text.c_str(), { pos.x - s.x / 2, pos.y - s.y / 2 }, size, spacing, color);
}

void DrawOutlinedText(const char *text, int posX, int posY, int fontSize, Color color, int outlineSize, Color outlineColor) {
    DrawText(text, posX - outlineSize, posY - outlineSize, fontSize, outlineColor);
    DrawText(text, posX + outlineSize, posY - outlineSize, fontSize, outlineColor);
    DrawText(text, posX - outlineSize, posY + outlineSize, fontSize, outlineColor);
    DrawText(text, posX + outlineSize, posY + outlineSize, fontSize, outlineColor);
    DrawText(text, posX, posY, fontSize, color);
}

void DrawOutlinedTextEx(const char *text, int posX, int posY, int fontSize, Color color, int outlineSize, Color outlineColor, Font font, float spacing) {
    DrawTextEx(font, text, {(float)posX - (float)outlineSize, (float)posY - (float)outlineSize}, fontSize, spacing, outlineColor);
    DrawTextEx(font, text, {(float)posX + (float)outlineSize, (float)posY - (float)outlineSize}, fontSize, spacing, outlineColor);
    DrawTextEx(font, text, {(float)posX - (float)outlineSize, (float)posY + (float)outlineSize}, fontSize, spacing, outlineColor);
    DrawTextEx(font, text, {(float)posX + (float)outlineSize, (float)posY + (float)outlineSize}, fontSize, spacing, outlineColor);
    DrawTextEx(font, text, {(float)posX, + (float)posY}, fontSize, spacing, color);
}

Sprite GetSpriteFromVector(std::string id, std::vector<Sprite> sprites)
{
    for(Sprite sprite : sprites)
    {
        if(sprite.id == id)
            return sprite;
    }

    return {0, 0, 0, 0};
}

Anim* GetAnimFromVector(std::string id, std::vector<Anim>& anims)
{
    for(int i = 0; i < anims.size(); i++)
    {
        if(id == anims[i].id)
            return &anims[i];
    }

    return nullptr;
}

void DrawSpriteFromVector(std::string id, Vector2 position, Vector2 size, std::vector<Sprite> sprites)
{
    if(id == "")
        return;
    
    Sprite s = GetSpriteFromVector(id, sprites);
    
    Rectangle src = {0, 0, (float)((size.x < 0) ? -s.width : s.width), (float)((size.y < 0) ? -s.height : s.height)};
    Rectangle dest = {position.x, position.y, size.x, size.y};
    
    DrawTexturePro(s.img, src, dest, {0, 0}, 0, WHITE);
}

void DrawSpriteFromVectorTint(std::string id, Vector2 position, Vector2 size, std::vector<Sprite> sprites, Color tint)
{
    if(id == "")
        return;
    
    Sprite s = GetSpriteFromVector(id, sprites);
    
    Rectangle src = {0, 0, (float)((size.x < 0) ? -s.width : s.width), (float)((size.y < 0) ? -s.height : s.height)};
    Rectangle dest = {position.x, position.y, size.x, size.y};
    
    DrawTexturePro(s.img, src, dest, {0, 0}, 0, tint);
}

void DrawSpriteFromVectorAlpha(std::string id, Vector2 position, Vector2 size, std::vector<Sprite> sprites, unsigned char opacity)
{
    if(id == "")
        return;

    Sprite s = GetSpriteFromVector(id, sprites);

    Rectangle src = {0, 0, (float)s.width, (float)s.height};
    Rectangle dest = {position.x, position.y, size.x, size.y};

    DrawTexturePro(s.img, src, dest, {0, 0}, 0, Color{opacity, opacity, opacity, opacity});
}

// anim slop
void DrawAnimFromVector(std::string id, Vector2 position, Vector2 size, std::vector<Anim>& anims, std::vector<Sprite> sprites, unsigned char alpha)
{
    if(id == "")
        return;

    Anim* a = GetAnimFromVector(id, anims);
    
    DrawSpriteFromVectorAlpha(a->frames[a->frame], position, size, sprites, alpha);

    a->frameCount++;
    if(a->frameCount >= 60)
        a->frameCount = 0;
        
    if(a->frameCount % int(60 / a->fps) == 0)
    {
        a->frame++;
        if(a->frame >= a->numFrames)
            a->frame = 0;
    }

}

void DrawSpriteDirect(Texture2D img, Vector2 position, Vector2 size)
{
    Rectangle src = {0, 0, (float)img.width, (float)-img.height};
    Rectangle dest = {position.x, position.y, size.x, size.y};
    
    DrawTexturePro(img, src, dest, {0, 0}, 0, WHITE);
}

void DrawSpriteFromVectorRotation(std::string id, Vector2 position, Vector2 size, std::vector<Sprite> sprites, float rotation)
{
    if(id == "")
    return;
    
    Sprite s = GetSpriteFromVector(id, sprites);

    Rectangle src = {0, 0, (float)s.width, (float)s.height};
    Rectangle dest = {position.x + 20, position.y + 20, size.x, size.y};

    DrawTexturePro(s.img, src, dest, {20, 20}, rotation, WHITE);
}



void UnloadSpritesFromVector(std::vector<Sprite>& sprites)
{
    for(Sprite sprite : sprites)
    {
        UnloadTexture(sprite.img);
    }
}