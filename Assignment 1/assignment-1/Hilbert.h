#ifndef HILBERT_H
#define HILBERT_H

#include <vector>
#include <glm/glm.hpp>

class Hilbert {
private:
	int depth = 0;
	void draw(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colours, int depth, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, glm::vec3 colour);
public:
	// Constructor
	Hilbert(int depth = 0);

	// Set the depth of the Hilbert curve
	void setDepth(int depth);
	int getDepth() const;

	// generate the Hilbert curve and colours
	void generate(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colours);
};

#endif
