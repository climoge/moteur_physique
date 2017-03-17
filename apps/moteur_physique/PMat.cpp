#include "PMat.hpp"
#include <glm/gtx/io.hpp>

PMat::PMat(glm::vec3 pos, float _m, bool _isFix) {
	_pos = glm::vec3(pos.x, pos.y, pos.z);
	m = _m;
	vit = glm::vec3(0.01f, 0, 0);
	frc = glm::vec3(0, 0, 0);
	isFix = _isFix;
}

void PMat::UpdateLeapFrog(double h)
{
	if(isFix) return;
	std::cout << "Vit : " << vit << std::endl;
	vit.x += h / m * frc.x;
	vit.y += h / m * frc.y;
	vit.z += h / m * frc.z;
	std::cout << "Vit updated: " << vit << std::endl << std::endl;
	std::cout << "Pos : " << _pos << std::endl;
	if(_pos.x + h * vit.x > 620 && _pos.x + h * vit.x < 0){
		vit.x *= -1;
	}
	_pos.x += h * vit.x;

	if(_pos.y + h * vit.y > 620 && _pos.y + h * vit.y < 0){
		_pos.y = 620;
		vit.y *= -1;
	}
	_pos.y += h * vit.y;
	//else _pos.y -= h * vit.y;
	if(_pos.z + h * vit.z > 50 && _pos.z + h * vit.z < -50){
		vit.z *= -1;
	}
	_pos.z += h * vit.z;
	//else _pos.z -= h * vit.z;
	
	std::cout << "Pos updated: " << _pos << std::endl << std::endl;
		
	frc = glm::vec3(0, 0, 0);
}

glm::vec3& PMat::getPos(){
	return _pos;
}

void PMat::setPos(glm::vec3 pos){
	_pos = pos;
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

std::ostream& operator<<(std::ostream& os, const PMat& pMat) 
{  
    os << pMat._pos;  
    return os;  
} 
