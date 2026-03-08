#include <raylib.h>
#include <string>
#include "graphics.hpp"
#include "mouse.hpp"
#include "maps.hpp"

#pragma once

void RenderTextButton(Font font, Vector2 position, float size, float spacing, std::string inactiveText, std::string activeText, void (*onClick)(), bool& active)
{
    DrawTextEx(font, (active) ? activeText.c_str() : inactiveText.c_str(), position, size, spacing, WHITE);

    Vector2 s = MeasureTextEx(font, (active) ? activeText.c_str() : inactiveText.c_str(), size, spacing);
    Vector2 m = MousePositionStandard();
    
    if(checkBoxCollison(m, {1, 1}, position, s))
    {
        active = true;

        if(IsMouseButtonPressed(0))
        {
            onClick();
        }
    }
    else
        active = false;
}

void RenderTextButtonAllowed(Font font, Vector2 position, float size, float spacing, std::string inactiveText, std::string activeText, void (*onClick)(), bool& active, bool allowed)
{
    DrawTextEx(font, (active) ? activeText.c_str() : inactiveText.c_str(), position, size, spacing, (allowed) ? WHITE : GRAY);

    if(allowed)
    {
        Vector2 s = MeasureTextEx(font, (active) ? activeText.c_str() : inactiveText.c_str(), size, spacing);
        Vector2 m = MousePositionStandard();
        
        if(checkBoxCollison(m, {1, 1}, position, s))
        {
            active = true;
    
            if(IsMouseButtonPressed(0))
            {
                onClick();
            }
        }
        else
            active = false;
    }
}

void RenderTextButtonAllowed1i(Font font, Vector2 position, float size, float spacing, std::string inactiveText, std::string activeText, void (*onClick)(int), int arg, bool& active, bool allowed)
{
    DrawTextEx(font, (active) ? activeText.c_str() : inactiveText.c_str(), position, size, spacing, (allowed) ? WHITE : GRAY);

    if(allowed)
    {
        Vector2 s = MeasureTextEx(font, (active) ? activeText.c_str() : inactiveText.c_str(), size, spacing);
        Vector2 m = MousePositionStandard();
        
        if(checkBoxCollison(m, {1, 1}, position, s))
        {
            active = true;
    
            if(IsMouseButtonPressed(0))
            {
                onClick(arg);
            }
        }
        else
            active = false;
    }
}

void RenderTextButtonCentered(Font font, Vector2 position, float size, float spacing, std::string inactiveText, std::string activeText, void (*onClick)(), bool& active)
{
    Vector2 s = MeasureTextEx(font, (active) ? activeText.c_str() : inactiveText.c_str(), size, spacing);

    DrawTextEx(font, (active) ? activeText.c_str() : inactiveText.c_str(), {position.x - s.x / 2, position.y - s.y / 2}, size, spacing, WHITE);

    Vector2 m = MousePositionStandard();
    
    if(checkBoxCollison(m, {1, 1}, {position.x - s.x / 2, position.y - s.y / 2}, s))
    {
        active = true;

        if(IsMouseButtonPressed(0))
        {
            onClick();
        }
    }
    else
        active = false;
}