#include "punkty.hpp"

punkty::punkty() {
	rotacja = 0;
	pozycja = { 0,0,0 };
}
punkty::~punkty() {
	
}
void punkty::setPos(Vector3 pos) {
	pozycja = pos;
}
void punkty::changeRot(float rot) {
	rotacja += rot;
}
void punkty::setRot(float rot) {
	rotacja = rot;
}
float punkty::getRot() {
	return rotacja;
}
Vector3 punkty::getPos() {
	return pozycja;
}
