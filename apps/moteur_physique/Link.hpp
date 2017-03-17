#pragma once
 
#ifndef LINK_H
#define LINK_H
 
#include "PMat.hpp"

class PMat;
 
class Link {
	public:
		Link(float _k, float _z);
		void LinkConnect(PMat &p1, PMat &p2);
		void LinkRessort();
		void LinkFrein();
		void LinkGravite(glm::vec3 gravity);
		PMat& getP1();
		void setP1(PMat &p1);
		PMat& getP2();
		void setP2(PMat &p2);

	private:
		PMat *_p1;
		PMat *_p2;
		float k, z, l0;
};

#endif
