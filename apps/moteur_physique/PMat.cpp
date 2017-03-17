#include "PMat.hpp"
#include <glm/gtx/io.hpp>

PMat::PMat(glm::vec3 pos, float _m, bool _isFix) {
	_pos = glm::vec3(pos.x, pos.y, pos.z);
	m = _m;
	vit = glm::vec3(0, 0, 0);
	_frc = glm::vec3(0, 0, 0);
	isFix = _isFix;
}

void PMat::UpdateLeapFrog(double h)
{
	if(isFix) return;
	//std::cout << "Vit : " << vit << std::endl;
	//std::cout << "Force : " << _frc << std::endl;
	vit.x += h / m * _frc.x;
	vit.y += h / m * _frc.y;
	vit.z += h / m * _frc.z;
	//std::cout << "Vit updated: " << vit << std::endl << std::endl;
	//std::cout << "Pos : " << _pos << std::endl;
	
	_pos.x += h * vit.x;
	_pos.y += h * vit.y;
	_pos.z += h * vit.z;
	
	//std::cout << "Pos updated: " << _pos << std::endl << std::endl;
	_frc = glm::vec3(0, 0, 0);
}

glm::vec3& PMat::getPos(){
	return _pos;
}

void PMat::setPos(glm::vec3 pos){
	_pos = pos;
}

glm::vec3 PMat::getFrc(){
	return _frc;
}

void PMat::setFrc(glm::vec3 frc){
	_frc = frc;
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
