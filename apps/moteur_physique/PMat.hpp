#pragma once

#include <glm/glm.hpp>

class PMat {
	public:
		PMat(glm::vec3 _pos, float _m, bool isFix);
		void UpdateLeapFrog(double h);
		glm::vec3& getPos();
		void setPos(glm::vec3 pos);
		glm::vec3& getFrc();
		void setFrc(glm::vec3 frc);
		glm::vec3& getVit();
		void setVit(glm::vec3 vit);
	
	private:
		glm::vec3 pos;
		glm::vec3 vit;
		float m;
		glm::vec3 frc;
		bool isFix;
};
