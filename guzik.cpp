#include "guzik.hpp"

Guzik::Guzik(const char* imagePath, Vector2 imagePosition)
{
    texture = LoadTexture(imagePath);
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