#include "raylib.h"

#pragma once

int renderWidth;
int renderHeight;

void SetRenderSize(int rW, int rH)
{
    renderWidth = rW;
    renderHeight = rH;
}

Vector2 MousePositionStandard()
{
    Vector2 m = GetMousePosition();
    float xr = (float)renderWidth / (float)GetScreenWidth();
    float yr = (float)renderHeight / (float)GetScreenHeight();

    return { m.x * xr, m.y * yr };
}