#include "PMat.hpp"

PMat::PMat(glm::vec3 _pos, float _m, bool _isFix) {
	pos = glm::vec3(_pos.x, _pos.y, _pos.z);
	m = _m;
	vit = glm::vec3(0, 0, 0);
	frc = glm::vec3(0, 0, 0);
	isFix = _isFix;
}

void PMat::UpdateLeapFrog(double h)
{
	if(isFix) return;
	
	vit.x += h / m * frc.x;
	vit.y += h / m * frc.y;
	vit.z += h / m * frc.z;
	if(pos.x + h * vit.x > 620 && pos.x + h * vit.x < 0){
		vit.x *= -1;
	}
	pos.x += h * vit.x;

	if(pos.y + h * vit.y > 620 && pos.y + h * vit.y < 0){
		pos.y = 620;
		vit.y *= -1;
	}
	pos.y += h * vit.y;
	//else pos.y -= h * vit.y;
	if(pos.z + h * vit.z > 50 && pos.z + h * vit.z < -50){
		vit.z *= -1;
	}
	pos.z += h * vit.z;
	//else pos.z -= h * vit.z;
		
	frc = glm::vec3(0, 0, 0);
}

glm::vec3& PMat::getPos(){
	return pos;
}

void PMat::setPos(glm::vec3 pos){
	pos = pos;
}

glm::vec3& PMat::getFrc(){
	return frc;
}

void PMat::setFrc(glm::vec3 frc){
	frc = frc;
}

glm::vec3& PMat::getVit(){
	return vit;
}

void PMat::setVit(glm::vec3 vit){
	vit = vit;
}
