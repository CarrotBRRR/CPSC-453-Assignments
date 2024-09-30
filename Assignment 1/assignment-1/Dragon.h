#ifndef DRAGON_H
#define DRAGON_H

#include <vector>
#include <glm/glm.hpp>

class Dragon {
public:
	Dragon(int depth);

	void setDepth(int depth);
	int getDepth() const;

	void generate(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colours);

private:
	void draw(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colours, int depth, glm::vec3 p1, glm::vec3 p2, glm::vec3 colour);

	int depth;
};

#endif
