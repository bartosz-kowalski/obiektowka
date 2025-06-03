#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <string>
#include <filesystem>
#include <iostream>
#include <vector>
#include <fstream>

#include "guzik.hpp"
#include "guzik.cpp"

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
	czesc(Model m, Vector3 pos, Vector3 axis, float angle, Vector3 s, std::string n, const bool mov, Vector3 rotationAx)
		: model(m), position(pos), rotationAxis(axis), rotationAngle(angle), scale(s), name(n), movable(mov) {
	}

	czesc() : model(), position({ 0, 0, 0 }), rotationAxis({ 0, 1, 0 }), rotationAngle(0.0f), scale({ 1, 1, 1 }), movable(false) {}

	czesc(Model m, Vector3 pos, std::string n, const bool mov, Vector3 rotationAx, float angle)
		: model(m), position(pos), name(n), movable(mov) {
		rotationAxis = rotationAx;
		rotationAngle = angle;
		scale = { 0.01, 0.01, 0.01 };
	}

	~czesc() {

		//UnloadModel(model);
	}

	void setPosition(Vector3 pos) {
		if (movable)
			position = pos;
	}

	void setRotationAxis(Vector3 axis) {
		if (movable)
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

	void Draw(Vector3 pos, Vector3 rot) {
		rot.x = DEG2RAD * rot.x;
		rot.y = DEG2RAD * rot.y;
		rot.z = DEG2RAD * rot.z;

		Matrix obrot = MatrixRotateXYZ(rot);
		Matrix posuw = MatrixTranslate(pos.x, pos.y, pos.z);

		model.transform = MatrixMultiply(obrot, posuw);

		DrawModel(model, Vector3Zero(), 0.01f, WHITE);
	}

	void Draw(Vector3 pos) {
		Matrix posuw = MatrixTranslate(pos.x, pos.y, pos.z);
		model.transform = posuw;

		DrawModel(model, Vector3Zero(), 0.01f, WHITE);
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

	void deleteCzesc() {
		UnloadModel(model);
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

		if (entry.is_regular_file() && entry.path().extension() == ".txt") {
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

void deleteModels(std::vector <czesc> czesci)
{
	for (czesc& part : czesci)
	{
		part.deleteCzesc();
	}
}

float normalize(float min, float value, float max) {
	float min_x = 0;
	float max_x = 100;
	if (max_x - min_x == 0) return min;
	return min + ((value - min_x) / (max_x - min_x)) * (max - min);
}

int main() {
	//std::cout << "Working directory: " << std::filesystem::current_path() << "\n";
	bool automat = false;
	float rotationTR = 0.0;
	float rotationMA = 0.0;

	InitWindow(1000, 800, "Wizualizacja nawijarki");
	SetTargetFPS(60);

	Guzik Xplus{ "Menu/Xplus.png", {64, 32} };
	Guzik Xmin{ "Menu/Xmin.png", {138, 32} };
	Guzik TRplus{ "Menu/TRplus.png", {212, 32} };
	Guzik TRmin{ "Menu/TRmin.png", {284, 32} };
	Guzik MAplus{ "Menu/MAplus.png", {358, 32} };
	Guzik MAmin{ "Menu/MAmin.png", {432, 32} };
	Guzik DWUR{ "Menu/DwuR.png", {730, 32} };
	Guzik DWUA{ "Menu/DwuA.png", {730, 32} };

	Camera3D camera = { 0 };
	camera.position = { 1.0f, 10.0f, 0.0f };  // Camera position
	camera.target = { 0.0f, 0.0f, 0.0f };      // Camera looking at point
	camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
	camera.fovy = 60.0f;

	camera.projection = CAMERA_PERSPECTIVE;

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
			if (fileName != "Motor Gear.obj") {
				czesc part(loadedModel, pos, fileName, move, { 0, 1, 0 }, 0.0f);
				czesci.push_back(part);
			}
			else {
				czesc part(loadedModel, pos, fileName, move, { 0, 1, 0 }, 90.0f);
				czesci.push_back(part);
			}
		}
		else {
			std::cerr << "Nie udało się załadować modelu: " << filePath << std::endl;
		}
	}

	Shader shader = LoadShader("lighting.vs", "lighting.fs");

	int lightPosLoc = GetShaderLocation(shader, "lightPos");
	int viewPosLoc = GetShaderLocation(shader, "viewPos");
	int lightColorLoc = GetShaderLocation(shader, "lightColor");
	int objectColorLoc = GetShaderLocation(shader, "objectColor");

	Vector3 lightPos = { 0.0f, 4.0f, 0.0f };
	Vector4 lightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	Vector4 objectColor = { 0.5f, 0.5f, 0.5f, 1.0f };

	double yC, yM, yT, yS, yCm, yMm, yTm, ySm, yCM, yMM, yTM, ySM;
	for (czesc& part : czesci)
	{
		part.getModel().materials[0].shader = shader;
		if (part.getName() == "Carrage.obj") {
			yC = yCm = part.getPosition().z;
			//std::cout << part.getPosition().x << ' ' << part.getPosition().y << ' ' << part.getPosition().z << '\n';
		}
		if (part.getName() == "Motor Gear.obj") {
			yM = yMm = part.getPosition().z;
			//std::cout << part.getPosition().x << ' ' << part.getPosition().y << ' ' << part.getPosition().z << '\n';
		}

		if (part.getName() == "Tool roller.obj") {
			yT = yTm = part.getPosition().z;
			//std::cout << part.getPosition().x << ' ' << part.getPosition().y << ' ' << part.getPosition().z << '\n';
		}

		if (part.getName() == "Spool hookers.obj") {
			yS = ySm = part.getPosition().z;
			//std::cout << part.getPosition().x << ' ' << part.getPosition().y << ' ' << part.getPosition().z << '\n';
		}

	}
	double gC, gM, gS, gT, gMA, gTR;
	double kC = 0, kM = 0, kS = 0, kT = 0, kMA = 0, kTR = 0;

	yCM = 800;
	yMM = ySM = 400;
	yTM = 401;
	bool working = false;
	std::ifstream gcode;
	std::string linia;
	FilePathList droppedFiles;
	int iterator = 0;

	while (!WindowShouldClose()) {

		SetShaderValue(shader, lightPosLoc, &lightPos, SHADER_UNIFORM_VEC3);
		SetShaderValue(shader, viewPosLoc, &camera.position, SHADER_UNIFORM_VEC3);
		SetShaderValue(shader, lightColorLoc, &lightColor, SHADER_UNIFORM_VEC4);
		SetShaderValue(shader, objectColorLoc, &objectColor, SHADER_UNIFORM_VEC4);

		Vector2 mousePosition = GetMousePosition();
		bool mousePressed = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
		bool mouseReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
		if ((DWUR.Wcisniety(mousePosition, mouseReleased) or DWUA.Wcisniety(mousePosition, mouseReleased)) && !working)
		{
			automat = !automat;
		}
		if (!automat)
		{
			if ((IsKeyDown(KEY_LEFT) || Xplus.Wcisniety(mousePosition, mousePressed)) && yC < yCM)
			{
				yC += 1;
				yT += 1;
				yM += 1;
				yS += 1;
				//std::cout << yC << '\n';
			}
			if ((IsKeyDown(KEY_RIGHT) || Xmin.Wcisniety(mousePosition, mousePressed)) && yC > yCm)
			{
				yC -= 1;
				yT -= 1;
				yM -= 1;
				yS -= 1;
			}
			if (IsKeyDown(KEY_W) || TRplus.Wcisniety(mousePosition, mousePressed))
			{
				rotationTR += 2.0f;
				//std::cout << "Obrót TR+: " << rotationTR << " stopni\n";
			}

			if (IsKeyDown(KEY_S) || TRmin.Wcisniety(mousePosition, mousePressed))
			{
				rotationTR -= 2.0f;
				//std::cout << "Obrót TR-: " << rotationTR << " stopni\n";
			}
			if (IsKeyDown(KEY_A) || MAplus.Wcisniety(mousePosition, mousePressed))
			{
				rotationMA += 2.0f;
				//std::cout << "Obrót MA+: " << rotationTR << " stopni\n";
			}

			if (IsKeyDown(KEY_D) || MAmin.Wcisniety(mousePosition, mousePressed))
			{
				rotationMA -= 2.0f;
				//std::cout << "Obrót MA-: " << rotationTR << " stopni\n";
			}
		}
		else if (!working) {
			if (IsFileDropped()) {
				droppedFiles = LoadDroppedFiles();
				if (droppedFiles.count > 1 || droppedFiles.count == 0) {
					UnloadDroppedFiles(droppedFiles);
				}
				else {
					std::filesystem::path plik = std::string(droppedFiles.paths[0]);

					if (!std::filesystem::exists(plik) || plik.extension() != ".gcode") {
						//kill yourself??????????
					}
					else {
						iterator = 0;
						gcode.open(plik);
						working = true;
					}
				}
			}
		}
		if (working && iterator % 11 == 0) {

			std::getline(gcode, linia);
			if (linia.find(';') == std::string::npos)
			{
				kC = kM = kS = kT = 0;
				std::stringstream ss(linia);
				char separator = ' ';
				std::string token;
				while (ss >> token) {
					if (token[0] == 'X') {
						gC = normalize(yCm, std::stod(token.substr(1)), yCM);
						gM = normalize(yMm, std::stod(token.substr(1)), yMM);
						gT = normalize(yTm, std::stod(token.substr(1)), yTM);
						gS = normalize(ySm, std::stod(token.substr(1)), ySM);

						kC = (gC - yC) / 10;
						kM = (gM - yM) / 10;
						kS = (gS - yS) / 10;
						kT = (gT - yT) / 10;
					}
					else if (token[0] == 'Y') {
						gMA = std::stod(token.substr(1));
						kMA = (gMA - rotationMA) / 10;
					}
					else if (token[0] == 'Z') {
						gTR = std::stod(token.substr(1));
						kTR = (gTR - rotationTR) / 10;
					}
				}
			}
			if (gcode.eof()) { gcode.close(); UnloadDroppedFiles(droppedFiles); automat = !automat; working = false; iterator = 0; }
		}

		if (working)
		{
			yC += kC;
			yM += kM;
			yS += kS;
			yT += kT;
			rotationMA += kMA;
			rotationTR += kTR;
			iterator += 1;
		}

		//Ruch kamery bo mnie już wkurzało że latała cały czas
		if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
			Vector2 delta = GetMouseDelta();
			camera.target = Vector3Zero();
			camera.position = Vector3RotateByAxisAngle(camera.position, { 0, 1, 0 }, -delta.x * 0.003f);

			Vector3 right = Vector3Normalize(Vector3CrossProduct(Vector3Subtract(camera.target, camera.position), camera.up));
			camera.position = Vector3RotateByAxisAngle(camera.position, right, -delta.y * 0.003f);

			UpdateCamera(&camera, CAMERA_FIRST_PERSON);
		}
		//przybliżanie środkowym myszy
		float wheel = GetMouseWheelMove();
		if (wheel != 0) {
			Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
			camera.position = Vector3Add(camera.position, Vector3Scale(forward, wheel * 0.5f));
		}
		BeginDrawing();
		ClearBackground(RAYWHITE);

		BeginMode3D(camera);


		float rotation = 0;
		int gearMod = 2;

		for (czesc& part : czesci) {
			Vector3 pos = part.getPosition();
			if (!part.isMovable())
				part.Draw();
			else
			{
				if (part.getName() == "Motor Gear.obj") {
					pos.z = yM;
					part.setPosition(pos);
					part.Draw(pos, { -rotationTR * gearMod, 90.0f, 0 });

				}
				else if (part.getName() == "Tool roller.obj") {

					pos.z = yT;
					part.setPosition(pos);
					part.Draw(pos, { rotationTR, 0, 0 });
				}
				else if (part.getName() == "Carrage.obj") {
					pos.z = yC;
					part.setPosition(pos);
					part.Draw(pos);
				}
				else if (part.getName() == "Spool hookers.obj") {
					pos.z = yS;
					part.setPosition(pos);
					part.Draw(pos, { 0, 0, rotationMA * 0.9270072f });
				}
				else if (part.getName() == "Mandrel.obj") {
					part.setPosition(pos);
					part.Draw(pos, { 0, 0, rotationMA });
				}
			}
		}

		DrawGrid(10, 1.0f);
		EndMode3D();

		Xplus.Draw();
		Xmin.Draw();
		TRplus.Draw();
		TRmin.Draw();
		MAmin.Draw();
		MAplus.Draw();
		if (automat == false)
		{
			DWUR.Draw();
		}
		else
		{
			DWUA.Draw();
		}


		EndDrawing();
	}

	UnloadShader(shader);
	deleteModels(czesci);
	CloseWindow();
	return 0;
}
