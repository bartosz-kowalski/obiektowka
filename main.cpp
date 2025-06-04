#include "guzik.hpp"
#include "czesci.hpp"
#include "rozne.hpp"

#define MAX_SEGMENTS 10000
Vector3 path[MAX_SEGMENTS];
int pathCount = 0;
Mesh filamentMesh = { 0 };
Model filamentModel = { 0 };
bool filamentDirty = true;
float filamentWidth = 0.05f;

bool CheckCollisionRayTriangle(Ray ray, Vector3 p1, Vector3 p2, Vector3 p3, Vector3* out)
{

	Vector3 edge1 = Vector3Subtract(p2, p1);
	Vector3 edge2 = Vector3Subtract(p3, p1);
	Vector3 h = Vector3CrossProduct(ray.direction, edge2);
	float a = Vector3DotProduct(edge1, h);
	if (a > -EPSILON && a < EPSILON) return false;

	float f = 1.0f / a;
	Vector3 s = Vector3Subtract(ray.position, p1);
	float u = f * Vector3DotProduct(s, h);
	if (u < 0.0f || u > 1.0f) return false;

	Vector3 q = Vector3CrossProduct(s, edge1);
	float v = f * Vector3DotProduct(ray.direction, q);
	if (v < 0.0f || u + v > 1.0f) return false;

	float t = f * Vector3DotProduct(edge2, q);
	if (t > EPSILON) {
		if (out != NULL) {
			*out = Vector3Add(ray.position, Vector3Scale(ray.direction, t));
		}
		return true;
	}

	return false;
}

int main() {

	InitWindow(1000, 800, "Wizualizacja nawijarki");
	SetTargetFPS(60);
	bool automat = false;
	bool stop = false;
	float rotationTR = 0.0;
	float rotationMA = 0.0;
	double kC = 0, kM = 0, kS = 0, kT = 0, kMA = 0, kTR = 0;
	double yC, yM, yT, yS, yCm, yMm, yTm, ySm, yCM = 800, yMM = 400, yTM = 401, ySM = 400;
	bool working = false;
	int iterator = 0, rotationSH = 0;
	float wheel;
	int gearMod = 2;
	std::ifstream gcode;
	std::string linia;

	Shader shader = LoadShader("lighting.vs", "lighting.fs");
	int lightPosLoc = GetShaderLocation(shader, "lightPos");
	int viewPosLoc = GetShaderLocation(shader, "viewPos");
	int lightColorLoc = GetShaderLocation(shader, "lightColor");
	int objectColorLoc = GetShaderLocation(shader, "objectColor");
	Vector3 lightPos = { 0.0f, 4.0f, 0.0f };
	Vector4 lightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	Vector4 objectColor = { 0.5f, 0.5f, 0.5f, 1.0f };
	Vector3 tool_pos = { 0 };
	Vector3 draw_pos = { 261, 0, 183 };

	FilePathList droppedFiles;

	Guzik Xplus{ "Menu/Xplus.png", {64, 32} };
	Guzik Xmin{ "Menu/Xmin.png", {138, 32} };
	Guzik TRplus{ "Menu/TRplus.png", {212, 32} };
	Guzik TRmin{ "Menu/TRmin.png", {284, 32} };
	Guzik MAplus{ "Menu/MAplus.png", {358, 32} };
	Guzik MAmin{ "Menu/MAmin.png", {432, 32} };
	Guzik DWUR{ "Menu/DwuR.png", {730, 32} };
	Guzik DWUA{ "Menu/DwuA.png", {730, 32} };
	Guzik Estop{ "Menu/Estop.png", {804, 32} };


	Camera3D camera;;
	camera.position = { 1.0f, 10.0f, 0.0f };
	camera.target = { 0.0f, 0.0f, 0.0f };
	camera.up = { 0.0f, 1.0f, 0.0f };
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

	for (czesc& part : czesci)
	{
		part.getModel().materials[0].shader = shader;
		if (part.getName() == "Carrage.obj") {
			yC = yCm = part.getPosition().z;
		}
		if (part.getName() == "Motor Gear.obj") {
			yM = yMm = part.getPosition().z;
		}

		if (part.getName() == "Tool roller.obj") {
			yT = yTm = part.getPosition().z;
		}

		if (part.getName() == "Spool hookers.obj") {
			yS = ySm = part.getPosition().z;
		}

	}

	Ray filament_r;
	filament_r.direction = { 1,0,0 };
	filament_r.position = { 0,0,0 };

	while (!WindowShouldClose()) {
		rotationSH %= 360;

		SetShaderValue(shader, lightPosLoc, &lightPos, SHADER_UNIFORM_VEC3);
		SetShaderValue(shader, viewPosLoc, &camera.position, SHADER_UNIFORM_VEC3);
		SetShaderValue(shader, lightColorLoc, &lightColor, SHADER_UNIFORM_VEC4);
		SetShaderValue(shader, objectColorLoc, &objectColor, SHADER_UNIFORM_VEC4);

		Vector2 mousePosition = GetMousePosition();
		bool mousePressed = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
		bool mouseReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
		if (((DWUR.Wcisniety(mousePosition, mouseReleased) or DWUA.Wcisniety(mousePosition, mouseReleased)) || IsKeyPressed(KEY_R)) && !working)
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
				rotationSH += 2;
			}
			if ((IsKeyDown(KEY_RIGHT) || Xmin.Wcisniety(mousePosition, mousePressed)) && yC > yCm)
			{
				yC -= 1;
				yT -= 1;
				yM -= 1;
				yS -= 1;
				rotationSH += 2;
			}
			if (IsKeyDown(KEY_W) || TRplus.Wcisniety(mousePosition, mousePressed))
			{
				rotationTR += 2.0f;
				rotationSH += 1;
			}

			if (IsKeyDown(KEY_S) || TRmin.Wcisniety(mousePosition, mousePressed))
			{
				rotationTR -= 2.0f;
				rotationSH += 1;
			}
			if (IsKeyDown(KEY_A) || MAplus.Wcisniety(mousePosition, mousePressed))
			{
				rotationMA += 2.0f;
				rotationSH += 2;
			}

			if (IsKeyDown(KEY_D) || MAmin.Wcisniety(mousePosition, mousePressed))
			{
				rotationMA -= 2.0f;
				rotationSH += 2;
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
						std::cerr << "Nie ma takiego pliku albo jest to plik nieodpowiedniego typu \n";
					}
					else {
						rotationMA = static_cast<int>(rotationMA) % 360;
						rotationTR = static_cast<int>(rotationTR) % 360;
						iterator = 0;
						gcode.open(plik);
						working = true;
					}
				}
			}
		}
		if (working && iterator % 6 == 0) {

			if (gcode.eof()) { gcode.close(); UnloadDroppedFiles(droppedFiles); automat = !automat; working = false; iterator = 0; continue; }
			std::getline(gcode, linia);
			if (linia.find(';') == std::string::npos)
			{
				kC = kM = kS = kT = 0;
				std::stringstream ss(linia);
				char separator = ' ';
				std::string token;
				while (ss >> token) {

					if (token[0] == 'X') {
						kC = (normalize(yCm, std::stod(token.substr(1)), yCM) - yC) / 5;
						kM = (normalize(yMm, std::stod(token.substr(1)), yMM) - yM) / 5;
						kT = (normalize(yTm, std::stod(token.substr(1)), yTM) - yT) / 5;
						kS = (normalize(ySm, std::stod(token.substr(1)), ySM) - yS) / 5;
					}
					else if (token[0] == 'Y') {
						kMA = (std::stod(token.substr(1)) - rotationMA) / 5;
					}
					else if (token[0] == 'Z') {
						kTR = (std::stod(token.substr(1)) - rotationTR) / 5;
					}
				}
			}
		}

		if (working)
		{
			if ((Estop.Wcisniety(mousePosition, mousePressed)) || IsKeyPressed(KEY_E))
			{
				stop = true;
			}
			if (stop)
			{
				rotationMA = static_cast<int>(rotationMA) % 360;
				rotationTR = static_cast<int>(rotationTR) % 360;
				gcode.close();
				UnloadDroppedFiles(droppedFiles);
				automat = !automat;
				working = false;
				iterator = 0;
				stop = false;
				continue;
			}
			yC += kC;
			yM += kM;
			yS += kS;
			yT += kT;
			rotationMA += kMA;
			rotationTR += kTR;
			rotationSH += 2;
			iterator += 1;
		}

		if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
			Vector2 delta = GetMouseDelta();
			camera.target = Vector3Zero();
			camera.position = Vector3RotateByAxisAngle(camera.position, { 0, 1, 0 }, -delta.x * 0.003f);

			Vector3 right = Vector3Normalize(Vector3CrossProduct(Vector3Subtract(camera.target, camera.position), camera.up));
			camera.position = Vector3RotateByAxisAngle(camera.position, right, -delta.y * 0.003f);

			UpdateCamera(&camera, CAMERA_FIRST_PERSON);
		}
		wheel = GetMouseWheelMove();
		if (wheel != 0) {
			Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
			camera.position = Vector3Add(camera.position, Vector3Scale(forward, wheel * 0.5f));
		}
		BeginDrawing();
		ClearBackground(RAYWHITE);

		BeginMode3D(camera);
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
					draw_pos.z = pos.z = yT;
					part.setPosition(pos);
					tool_pos = pos;
					part.Draw(pos, { rotationTR, 0, 0 });
					filament_r.position = pos;
					filament_r.direction = { 1, 0, 0 };
				}
				else if (part.getName() == "Carrage.obj") {
					pos.z = yC;
					part.setPosition(pos);
					part.Draw(pos);
				}
				else if (part.getName() == "Spool hookers.obj") {
					pos.z = yS;
					part.setPosition(pos);
					part.Draw(pos, { 0, 0, static_cast<float>(rotationSH) });
				}
				else if (part.getName() == "Mandrel.obj") {
					part.setPosition(pos);
					part.Draw(pos, { 0, 0, rotationMA });
				}
			}
		}

		//279 y z 
		DrawSphere(draw_pos, 1, BLUE);

		DrawGrid(10, 1.0f);
		EndMode3D();

		Xplus.Draw();
		Xmin.Draw();
		TRplus.Draw();
		TRmin.Draw();
		MAmin.Draw();
		MAplus.Draw();
		Estop.Draw();

		if (!automat)
			DWUR.Draw();
		else
			DWUA.Draw();

		EndDrawing();
	}

	UnloadShader(shader);
	deleteModels(czesci);
	CloseWindow();
	return 0;
}
