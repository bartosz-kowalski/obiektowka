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
		//if(!movable)
		DrawModelEx(model, position, rotationAxis, rotationAngle, scale, WHITE);
	}

	void Draw(Vector3 pos, Vector3 rot) {
		rot.x = DEG2RAD * rot.x;
		rot.y = DEG2RAD * rot.y;
		rot.z = DEG2RAD * rot.z;

		Matrix obrot = MatrixRotateXYZ(rot);
		Matrix posuw = MatrixTranslate(pos.x, pos.y, pos.z);

		model.transform = MatrixMultiply(posuw, obrot);

		DrawModel(model, Vector3Zero(), 0.01f, WHITE);
	}
	void Draw2(Vector3 pos, Vector3 rot) {
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

	Vector3 getPosition() const{
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

int main() {
	//std::cout << "Working directory: " << std::filesystem::current_path() << "\n";
	bool automat = false;
	float rotationTR = 0.0;

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
				//part.setRotationAxis({ 1, 0, 0 });
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

	double yC, yM, yT, yS, yMax, yMin;
	for (czesc& part : czesci)
	{
		part.getModel().materials[0].shader = shader;
		if (part.getName() == "Carrage.obj") {
			yMin = yC = part.getPosition().z;
			std::cout << part.getPosition().x << ' ' << part.getPosition().y << ' ' << part.getPosition().z << '\n';
		}
		if (part.getName() == "Motor Gear.obj") {
			yM = part.getPosition().x;
			std::cout << part.getPosition().x << ' ' << part.getPosition().y << ' ' << part.getPosition().z << '\n';
		}

		if (part.getName() == "Tool roller.obj") {
			yT = part.getPosition().z;
			std::cout << part.getPosition().x << ' ' << part.getPosition().y << ' ' << part.getPosition().z << '\n';
		}

		if (part.getName() == "Spool hookers.obj") {
			yS = part.getPosition().z;
			std::cout << part.getPosition().x << ' ' << part.getPosition().y << ' ' << part.getPosition().z << '\n';
		}

	}

	yMax = 800;

	while (!WindowShouldClose()) {

		SetShaderValue(shader, lightPosLoc, &lightPos, SHADER_UNIFORM_VEC3);
		SetShaderValue(shader, viewPosLoc, &camera.position, SHADER_UNIFORM_VEC3);
		SetShaderValue(shader, lightColorLoc, &lightColor, SHADER_UNIFORM_VEC4);
		SetShaderValue(shader, objectColorLoc, &objectColor, SHADER_UNIFORM_VEC4);

		Vector2 mousePosition = GetMousePosition();
		bool mousePressed = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
		bool mouseReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
		if (DWUR.Wcisniety(mousePosition, mouseReleased) or DWUA.Wcisniety(mousePosition, mouseReleased))
		{
			automat = !automat;
			std::cout << "Zmiana trybu" << std::endl;
		}
		if (automat == false)
		{
		if (Xplus.Wcisniety(mousePosition, mousePressed) && yC < yMax)
			{
				yC += 1;
				yT += 1;
				yM -= 1;
				yS += 1;
				std::cout << yC << '\n';
			}
			if (Xmin.Wcisniety(mousePosition, mousePressed) && yC > yMin)
			{
				yC -= 1;
				yT -= 1;
				yM += 1;
				yS -= 1;
			}
			if (TRplus.Wcisniety(mousePosition, mousePressed))
			{
				rotationTR += 2.0f;
				std::cout << "Obrót TR+: " << rotationTR << " stopni\n";
			}

			if (TRmin.Wcisniety(mousePosition, mousePressed))
			{
				rotationTR -= 2.0f;
				std::cout << "Obrót TR-: " << rotationTR << " stopni\n";
			}
		}
		

		//UpdateCamera(&camera, CAMERA_THIRD_PERSON);

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
		int gearMod = 5;

		for (czesc& part : czesci) {
			Vector3 pos = part.getPosition();
			if (!part.isMovable())
				part.Draw();
			else
			{   //carrage, motor gear, tool roller
				if (part.getName() == "Motor Gear.obj") {
					pos.x = yM;
					part.setPosition(pos);
					part.Draw(pos, { rotation * gearMod, 90.0f, 0 });

				}
				else if (part.getName() == "Tool roller.obj") {
					//rlPushMatrix();
					//rlTranslatef(pos.x,pos.y,pos.z);
					//rlRotatef(rotationTR, pos.x, pos.y, pos.z);
					//rlTranslatef(-pos.x, -pos.y, -pos.z);
					pos.z = yT;
					part.setPosition(pos);
					part.Draw2(pos, { rotationTR, 0, 0 });
					//DrawModelEx(part.getModel(), {0,0,0}, {1, 0, 0}, rotationTR, {0.01,0.01,0.01}, WHITE);
					//rlPopMatrix();
				}
				else if (part.getName() == "Carrage.obj") {
					pos.z = yC;
					part.setPosition(pos);
					part.Draw(pos);
				}
				else if (part.getName() == "Spool hookers.obj") {
					pos.z = yS;
					part.setPosition(pos);
					part.Draw(pos, { rotation, 0, 0 });
				}
				else if(part.getName() == "Mandrel.obj") {
					part.Draw(pos);
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
