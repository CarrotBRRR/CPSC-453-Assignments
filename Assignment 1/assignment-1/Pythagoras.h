#ifndef PYTHAGORAS_H
#define PYTHAGORAS_H

#include <vector>
#include <glm/glm.hpp>

class Pythagoras {
public:
	Pythagoras(int depth);

	void setDepth(int depth);
	int getDepth() const;

	void generate(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors);

private:
	void draw(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors, int depth, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec3 color);

	int depth;
};

#endif
