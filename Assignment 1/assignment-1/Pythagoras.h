#ifndef PYTHAGORAS_H
#define PYTHAGORAS_H

#include <vector>
#include <glm/glm.hpp>

class Pythagoras {
public:
	Pythagoras(int depth);

	void setDepth(int depth);
	int getDepth() const;

	void generate(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colours);

private:
	void draw(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colours, glm::vec2 position, float size, float angle, int depth);

	int depth;
};

#endif
