#include "Link.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtx/io.hpp>

Link::Link(float _k, float _z){
	k = _k;
	z = _z;
	l0 = 0;
	_p1 = nullptr;
	_p2 = nullptr;
}

void Link::LinkConnect(PMat &p1, PMat &p2){
	setP1(p1);
	setP2(p2);
	l0 = glm::distance(_p1->getPos(), _p2->getPos());
}

void Link::LinkRessort(){
	//std::cout << "Ressort" << std::endl;
	float d = glm::distance(_p1->getPos(), _p2->getPos());
	float f = k * (1 -l0 / d);
	
	std::cout << "d : " << d << std::endl;
	//_p1->setFrc(glm::vec3(0, 0, 0));
	_p1->setFrc(glm::vec3(f, f, 0.f) + _p1->getFrc());
	_p2->setFrc(glm::vec3(f, f, 0.f) - _p2->getFrc());
	//_p1->getFrc() += glm::vec3(f, f, 0);
	//_p2->getFrc() -= glm::vec3(f, f, 0);

}

void Link::LinkFrein(){
	//std::cout << "Frein" << std::endl;
	glm::vec3 dv = glm::vec3(_p2->getVit().x - _p1->getVit().x, _p2->getVit().y - _p1->getVit().y, _p2->getVit().z - _p1->getVit().z);
	dv.x *= z;
	dv.y *= z;
	dv.z *= z;
	_p1->setFrc(_p1->getFrc() + dv);
	_p2->setFrc(_p2->getFrc() - dv);
}

void Link::LinkGravite(glm::vec3 gravity){
	std::cout << "Gravity : " << gravity << std::endl;
	std::cout << "Force in Link : " << _p1->getFrc() << std::endl;
	_p1->setFrc(_p1->getFrc() + gravity);
	_p1->setFrc(gravity);
	std::cout << "Force in Link updated : " << _p1->getFrc() << std::endl << std::endl;
	_p2->setFrc(_p2->getFrc() + gravity);
	_p2->setFrc(gravity);
}

PMat& Link::getP1(){
	return *_p1;
}

void Link::setP1(PMat& p1){
	_p1 = &p1;
}

PMat& Link::getP2(){
	return *_p2;
}

void Link::setP2(PMat& p2){
	_p2 = &p2;
}
