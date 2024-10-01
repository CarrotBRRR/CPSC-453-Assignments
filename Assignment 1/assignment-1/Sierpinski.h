#ifndef SIERPINSKI_H
#define SIERPINSKI_H

#include <vector>
#include <glm/glm.hpp>

class Sierpinski {
private:
	int depth = 0;
	float step = depth;
	void draw(std::vector<glm::vec3>& vertices, int depth, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 colour);
	void generateColours(std::vector<glm::vec3>& colours, int depth);

public:
	// Constructor
	Sierpinski(int depth = 0);

	// Set the depth of the Sierpinski triangle
	void setDepth(int depth);
	int getDepth() const;

	// generate the Sierpinski triangle and colours
	void generate(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colours);
};

#endif
