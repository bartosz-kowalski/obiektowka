#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <string>
#include <filesystem>
#include <iostream>
#include <vector>

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

int main() {
    //std::cout << "Working directory: " << std::filesystem::current_path() << "\n";
    InitWindow(800, 600, "Wizualizacja nawijarki");
    SetTargetFPS(60);

    Camera3D camera = { 0 };
    camera.position = { 0.0f, 10.0f, 10.0f };  // Camera position
    camera.target = { 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;

    // Za³aduj model STL (upewnij siê, ¿e plik istnieje w katalogu projektu)

    std::vector <Model> model;
	std::vector<std::string> fileNames = listFilesInDirectory("model/");
    for(int i = 0; i < fileNames.size(); i++) {
        std::string filePath = "model/" + fileNames[i];
        Model loadedModel = ImportSTLModel(filePath.c_str());
        if (loadedModel.meshCount > 0) {
            model.push_back(loadedModel);
        } else {
            std::cerr << "Nie uda³o siê za³adowaæ modelu: " << filePath << std::endl;
        }
	}
    

    Vector3 position = { 0.0f, 0.0f, 0.0f };

    while (!WindowShouldClose()) {
        UpdateCamera(&camera, );

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        for(int i = 0; i < model.size(); i++) {
            DrawModel(model[i], position, 1.0f, GRAY);
		}
        DrawGrid(10, 1.0f);
        EndMode3D();

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