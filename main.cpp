#include <raylib.h>
#include "raymath.h"
#include "rlgl.h"
#include <string>
#include <filesystem>
#include <iostream>
#include <vector>
#include "guzik.hpp"
#include "guzik.cpp"
// JakubQ test
Model ImportSTLModel(const char* filename) {
    if (!std::filesystem::exists(filename)) {
        std::cerr << "Plik STL nie istnieje: " << filename << std::endl;
        return Model();
	}

    return LoadModel(filename);;
}

std::vector<std::string> listFilesInDirectory(const std::string& folderPath) {
    std::vector<std::string> objFiles;

    if (!std::filesystem::exists(folderPath)) {
        std::cerr << "Folder nie istnieje: " << folderPath << std::endl;
        return {};
    }

    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".obj") {
            objFiles.push_back(entry.path().filename().string()); // tylko nazwa pliku
        }
    }

    return objFiles;
}

void RysujGuzik()
{

}
int main() {
    //std::cout << "Working directory: " << std::filesystem::current_path() << "\n";
    
    InitWindow(800, 600, "Wizualizacja nawijarki");
    SetTargetFPS(60);
    Guzik Xplus{ "Menu/Xplus.png", {64, 32} };
    Guzik Xmin{ "Menu/Xmin.png", {138, 32} };
    Guzik TRplus{ "Menu/TRplus.png", {212, 32} };
    Guzik TRmin{ "Menu/TRmin.png", {284, 32} };
    Guzik MAplus{ "Menu/MAplus.png", {358, 32} };
    Guzik MAmin{ "Menu/MAmin.png", {432, 32} };
    Guzik STOP{ "Menu/STOP.png", {600, 32} };
    Guzik START{ "Menu/START.png", {674, 32} };
    Camera3D camera = { 0 };
    camera.position = { 0.0f, 10.0f, 10.0f };  // Camera position
    camera.target = { 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;
    //SetCameraMode(camera, CAMERA_FREE);

    // Za�aduj model STL (upewnij si�, �e plik istnieje w katalogu projektu)

    std::vector <Model> model;
	std::vector<std::string> fileNames = listFilesInDirectory("model/");
    for(int i = 0; i < fileNames.size(); i++) {
        std::string filePath = "model/" + fileNames[i];
        Model loadedModel = ImportSTLModel(filePath.c_str());
        if (loadedModel.meshCount > 0) {
            model.push_back(loadedModel);
        } else {
            std::cerr << "Nie uda�o si� za�adowa� modelu: " << filePath << std::endl;
        }
	}
    

    Vector3 position = { 0.0f, 0.0f, 0.0f };

    while (!WindowShouldClose()) {
        Vector2 mousePosition = GetMousePosition();
        bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if (Xplus.Wcisniety(mousePosition, mousePressed))
        {
            std::cout << "Guzik1 wcisniety" << std::endl;
        }
        if (Xmin.Wcisniety(mousePosition, mousePressed))
        {
            std::cout << "Guzik2 wcisniety" << std::endl;
        }

        UpdateCamera(&camera, CAMERA_THIRD_PERSON);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        for(int i = 0; i < model.size(); i++) {
            DrawModel(model[i], position, 1.0f, GRAY);
		}

        DrawGrid(10, 1.0f);
        EndMode3D();

        Xplus.Draw();
        Xmin.Draw();
        TRplus.Draw();
        TRmin.Draw();
        MAmin.Draw();
        MAplus.Draw();
        STOP.Draw();
        START.Draw();
        //DrawText("Wczytano model STL", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }

    for(int i = 0; i < model.size(); i++) {
        if (model[i].meshCount > 0) {
            UnloadModel(model[i]);
        }
	}
    CloseWindow();
    return 0;
}