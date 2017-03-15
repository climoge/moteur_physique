#include "Link.hpp"
#include <glm/glm.hpp>

Link::Link(float _k, float _z){
	k = _k;
	z = _z;
	l0 = 0;
	p1 = nullptr;
	p2 = nullptr;
}

void Link::LinkConnect(PMat &_p1, PMat &_p2){
	setP1(_p1);
	setP2(_p2);
	l0 = glm::distance(p1->getPos(), p2->getPos());
}

void Link::LinkRessort(){
	float d = glm::distance(p1->getPos(), p2->getPos());
	float f = k * (1 -l0 / d);
	
	p1->getFrc() += glm::vec3(f, f, 0);
	p2->getFrc() -= glm::vec3(f, f, 0);

}

void Link::LinkFrein(){
	glm::vec3 dv = glm::vec3(p2->getVit().x - p1->getVit().x, p2->getVit().y - p1->getVit().y, p2->getVit().z - p1->getVit().z);
	dv.x *= z;
	dv.y *= z;
	dv.z *= z;
	p1->getFrc() += dv;
	p2->getFrc() -= dv;
}

void Link::LinkGravite(glm::vec3 gravity){
	p1->getFrc() += gravity;
	p2->getFrc() += gravity;
}

PMat& Link::getP1(){
	return *p1;
}

void Link::setP1(PMat& p1){
	p1 = p1;
}

PMat& Link::getP2(){
	return *p2;
}

void Link::setP2(PMat& p2){
	p2 = p2;
}
