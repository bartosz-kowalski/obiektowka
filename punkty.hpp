#pragma once

#include "raylib.h"
#include "raymath.h"

class punkty {
private:
	Vector3 pozycja;
	float rotacja;
public:
	punkty();
	~punkty();
	void setPos(Vector3);
	void changeRot(float);
	void setRot(float);
	float getRot();
	Vector3 getPos();
};
