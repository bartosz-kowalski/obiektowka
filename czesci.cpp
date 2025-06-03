#include "czesci.hpp"


inline czesc::czesc(Model m, Vector3 pos, Vector3 axis, float angle, Vector3 s, std::string n, const bool mov, Vector3 rotationAx)
	: model(m), position(pos), rotationAxis(axis), rotationAngle(angle), scale(s), name(n), movable(mov) {
}

czesc::czesc() : model(), position({ 0, 0, 0 }), rotationAxis({ 0, 1, 0 }), rotationAngle(0.0f), scale({ 1, 1, 1 }), movable(false) {}

czesc::czesc(Model m, Vector3 pos, std::string n, const bool mov, Vector3 rotationAx, float angle)
	: model(m), position(pos), name(n), movable(mov) {
	rotationAxis = rotationAx;
	rotationAngle = angle;
	scale = { 0.01, 0.01, 0.01 };
}

czesc::~czesc() {

	//UnloadModel(model);
}

void czesc::setPosition(Vector3 pos) {
	if (movable)
		position = pos;
}

void czesc::setRotationAxis(Vector3 axis) {
	if (movable)
		rotationAxis = axis;
}

void czesc::setRotationAngle(float angle) {
	if (movable)
		rotationAngle = angle;
}

void czesc::setScale(Vector3 s) {
	scale = s;
}

void czesc::Draw() {
	DrawModelEx(model, position, rotationAxis, rotationAngle, scale, WHITE);
}

void czesc::Draw(Vector3 pos, Vector3 rot) {
	rot.x = DEG2RAD * rot.x;
	rot.y = DEG2RAD * rot.y;
	rot.z = DEG2RAD * rot.z;

	Matrix obrot = MatrixRotateXYZ(rot);
	Matrix posuw = MatrixTranslate(pos.x, pos.y, pos.z);

	model.transform = MatrixMultiply(obrot, posuw);

	DrawModel(model, Vector3Zero(), 0.01f, WHITE);
}

void czesc::Draw(Vector3 pos) {
	Matrix posuw = MatrixTranslate(pos.x, pos.y, pos.z);
	model.transform = posuw;

	DrawModel(model, Vector3Zero(), 0.01f, WHITE);
}

Vector3 czesc::getPosition() const {
	return position;
}

Model czesc::getModel() const {
	return model;
}
std::string czesc::getName() const {
	return name;
}
bool czesc::isMovable() const {
	return movable;
}

Vector3 czesc::getScale() const {
	return scale;
}

int czesc::getMeshCount() const {
	return model.meshCount;
}

void czesc::Update(float dt) {
	if (movable) {
		rotationAngle += 45.0f * dt;
	}
}

void czesc::deleteCzesc() {
	UnloadModel(model);
}
