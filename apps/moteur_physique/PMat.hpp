#pragma once

#ifndef PMAT_H
#define PMAT_H

#include <glm/glm.hpp>
#include <iostream> 

class PMat {
	public:
		PMat(glm::vec3 pos, float _m, bool isFix);
		void UpdateLeapFrog(double h);
		glm::vec3 getPos();
		void setPos(glm::vec3 pos);
		glm::vec3 getFrc();
		void setFrc(glm::vec3 frc);
		glm::vec3& getVit();
		void setVit(glm::vec3 vit);
		
		friend std::ostream& operator<<(std::ostream& os, const PMat& pMat);
	
	private:
		glm::vec3 _pos;
		glm::vec3 vit;
		float m;
		glm::vec3 _frc;
		bool isFix;
};

#endif
