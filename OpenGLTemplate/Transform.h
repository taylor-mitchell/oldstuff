#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Transform
{
public:
	Transform(const glm::vec3& p = glm::vec3(), const glm::vec3& r = glm::vec3(), const glm::vec3& s = glm::vec3(1.0f, 1.0f, 1.0f))
	{
		pos = p;
		rot = r;
		scale = s;
	}

	inline glm::mat4 getModel() const
	{
		glm::mat4 posMat = glm::translate(pos);
		glm::mat4 scaleMat = glm::scale(scale);
		glm::mat4 rotX = glm::rotate(rot.x, glm::vec3(1.0, 0.0, 0.0));
		glm::mat4 rotY = glm::rotate(rot.y, glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 rotZ = glm::rotate(rot.z, glm::vec3(0.0, 0.0, 1.0));
		glm::mat4 rotMat = rotZ * rotY * rotX;

		return posMat * rotMat * scaleMat;
	}

	/*inline glm::mat4 getMVP(const Camera& camera) const
	{
		glm::mat4 VP = camera.getViewProjection();
		glm::mat4 M = getModel();

		return VP * m;
	}*/

	inline glm::vec3& getPos() { return pos; }
	inline glm::vec3& getRot() { return rot; }
	inline glm::vec3& getScale() { return scale; }

	inline void setPos(glm::vec3& p) { pos = p; }
	inline void setRot(glm::vec3& r) { rot = r; }
	inline void setScale(glm::vec3& s) { scale = s; }

private:
	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 scale;
};


#endif