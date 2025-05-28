#include "guzik.hpp"
#include "raylib.h"

Guzik::Guzik(const char* imagePath, Vector2 imagePosition)
{
    Image image = LoadImage(imagePath);

    int originalWidth = image.width;
    int originalHeight = image.height;

    texture = LoadTextureFromImage(image);
    UnloadImage(image);
    position = imagePosition;
}

Guzik::~Guzik()
{
    UnloadTexture(texture);
}

void Guzik::Draw()
{
    DrawTextureV(texture, position, WHITE);
}

bool Guzik::Wcisniety(Vector2 mousePos, bool mousePressed)
{
    Rectangle rect = { position.x, position.y, static_cast<float>(texture.width), static_cast<float>(texture.height) };
    if (CheckCollisionPointRec(mousePos, rect) && mousePressed)
    {
        return true;
    }
    return false;
}
