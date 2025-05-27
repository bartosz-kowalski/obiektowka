#pragma once
#include <raylib.h>

class Guzik
{
    public:
        Guzik(const char* imagePath, Vector2 imagePosition);
        ~Guzik();
        void Draw();
        bool Wcisniety(Vector2 mousePos, bool mousePressed);
    private:
        Texture2D texture;
        Vector2 position;
};
