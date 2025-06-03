#pragma once

#include <raylib.h>
#include <raymath.h>
#include <string>

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
	czesc(Model m, Vector3 pos, Vector3 axis, float angle, Vector3 s, std::string n, const bool mov, Vector3 rotationAx);
	czesc();
	czesc(Model m, Vector3 pos, std::string n, const bool mov, Vector3 rotationAx, float angle);

	~czesc();

	void setPosition(Vector3 pos);
	void setRotationAxis(Vector3 axis);
	void setRotationAngle(float angle);
	void setScale(Vector3 s);
	void Draw();
	void Draw(Vector3 pos, Vector3 rot);
	void Draw(Vector3 pos);
	Vector3 getPosition() const;
	Model getModel() const;
	std::string getName() const;
	bool isMovable() const;

	Vector3 getScale() const;

	int getMeshCount() const;

	void Update(float dt);

	void deleteCzesc();
};
