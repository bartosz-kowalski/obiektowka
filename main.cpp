#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <string>
#include <filesystem>
#include <iostream>
#include <vector>
#include <fstream>

class czesc {
private:
    Model model;
    Vector3 position = { 0, 0, 0 };
    Vector3 rotationAxis = { 0, 1, 0 };
    float rotationAngle = 0.0f;
    Vector3 scale = { 0.0001, 0.0001, 0.0001 };
    const bool movable = false;
    std::string name; 

public:
    czesc(Model m, Vector3 pos, Vector3 axis, float angle, Vector3 s, std::string n, const bool mov)
		: model(m), position(pos), rotationAxis(axis), rotationAngle(angle), scale(s), name(n), movable(mov) {
    }

	czesc() : model(), position({ 0, 0, 0 }), rotationAxis({ 0, 1, 0 }), rotationAngle(0.0f), scale({ 1, 1, 1 }), movable(false) {} 

    czesc(Model m, Vector3 pos, std::string n, const bool mov)
        : model(m), position(pos), name(n), movable(mov) {
        rotationAxis = { 0, 1, 0 };
        rotationAngle = 0.0f;
        scale = { 0.01, 0.01, 0.01 };
	}

    ~czesc() {

        //UnloadModel(model);
    }

    void setPosition(Vector3 pos) {
        if(movable)
            position = pos;
	}

    void setRotationAxis(Vector3 axis) {
        if(movable)
            rotationAxis = axis;
    }

    void setRotationAngle(float angle) {
        if (movable)
            rotationAngle = angle;
	}

    void setScale(Vector3 s) {
        scale = s;
    }

    void Draw() {
        DrawModelEx(model, position, rotationAxis, rotationAngle, scale, WHITE);
    }

    Vector3 getPosition() const {
        return position;
	}

    Model getModel() const {
		return model;   
    }
    std::string getName() const {
        return name;
    }
    bool isMovable() const {
        return movable;
    }

    Vector3 getScale() const {
        return scale;
	}

    int getMeshCount() const {
        return model.meshCount;
    }

    void Update(float dt) {
        if (movable) {
            rotationAngle += 45.0f * dt; 
        }
    }
};

Model ImportSTLModel(const char* filename) {
    if (!std::filesystem::exists(filename)) {
        std::cerr << "Plik STL nie istnieje: " << filename << std::endl;
        return Model();
	}

    return LoadModel(filename);;
}

std::vector<std::string> listFilesInDirectory(const std::string& folderPath, std::vector<std::string>& txtFiles) {
    std::vector<std::string> objFiles;

    if (!std::filesystem::exists(folderPath)) {
        std::cerr << "Folder nie istnieje: " << folderPath << std::endl;
        return {};
    }

    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".obj") {
            objFiles.push_back(entry.path().filename().string()); // tylko nazwa pliku 
        }

        if( entry.is_regular_file() && entry.path().extension() == ".txt") {
            txtFiles.push_back(entry.path().filename().string()); // tylko nazwa pliku 
		}
    }

    return objFiles;
}

std::ifstream openFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Nie można otworzyć pliku: " << filePath << std::endl;
    }
    return file;
}

bool movableLine(const std::string& line) {
    return line.find("true") != std::string::npos;
}

Vector3 positionLine(const std::string& line) {
    Vector3 result = { 0 };

    // Znajdź początek nawiasu
    size_t start = line.find('{');
    size_t end = line.find('}');
    if (start == std::string::npos || end == std::string::npos) return result;

    std::string vecString = line.substr(start + 1, end - start - 1); // "-10, -10, 0"

    std::istringstream iss(vecString);
    std::string val;
    std::vector<float> values;

    while (std::getline(iss, val, ',')) {
        values.push_back(std::stof(val));
    }

    if (values.size() == 3) {
        result.x = values[0];
        result.z = values[1];
        result.y = values[2];
    }

    return result;
}

int main() {
    //std::cout << "Working directory: " << std::filesystem::current_path() << "\n";
    InitWindow(800, 600, "Wizualizacja nawijarki");
    SetTargetFPS(60);

    Camera3D camera = { 0 };
    camera.position = { 10.0f, 2.0f, 10.0f };  // Camera position
    camera.target = { 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;
    //SetCameraMode(camera, CAMERA_FREE);


    std::vector <czesc> czesci;
	std::vector <std::string> txtFileNames;
	std::vector<std::string> fileNames = listFilesInDirectory("model/", txtFileNames);

    for (int i = 0; i < fileNames.size(); i++) {
        std::string fileName = fileNames[i];
        std::string txtFileName = txtFileNames[i];
        std::string filePath = "model/" + fileName;
        std::string txtFilePath = "model/" + txtFileName;

        Model loadedModel = ImportSTLModel(filePath.c_str());

        if (loadedModel.meshCount > 0) {
            std::ifstream file(txtFilePath);
            std::string line;
            Vector3 pos = { 0 };
            bool move = false;

            while (std::getline(file, line)) {
                if (line.find("position:") != std::string::npos) {
                    pos = positionLine(line);
                }
                else if (line.find("movable:") != std::string::npos) {
                    move = movableLine(line);
                }
            }

            file.close();

            czesc part(loadedModel, pos, fileName, move);
            czesci.push_back(part);
        }
        else {
            std::cerr << "Nie udało się załadować modelu: " << filePath << std::endl;
        }
    }
    

    Vector3 position = { 0.0f, 0.0f, 0.0f };

    while (!WindowShouldClose()) {
        UpdateCamera(&camera, CAMERA_THIRD_PERSON);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        for (czesc& part : czesci) {
            part.Draw();
        }
        DrawGrid(10, 1.0f);
        EndMode3D();

        //DrawText("Wczytano model STL", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}