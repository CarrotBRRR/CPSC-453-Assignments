#include "Sierpinski.h"

#include <iostream>

using namespace std;

// Constructor
Sierpinski::Sierpinski(int depth) : depth(depth) {}

void Sierpinski::setDepth(int depth) {
	this->depth = depth;
}

int Sierpinski::getDepth() const {
	return depth;
}

// Generate verticies and colours for the Sierpinski triangle
void Sierpinski::generate(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colours) {
	// Clear the vectors, in case they are not empty
	vertices.clear();
	colours.clear();

	// Define the initial triangle
	glm::vec3 v1(-0.5f, -0.5f, 1.f);
	glm::vec3 v2(0.5f, -0.5f, 1.f);
	glm::vec3 v3(0.f, 0.5f, 1.f);

	glm::vec3 c(1.f, 1.f, 1.f);

	draw(vertices, this->depth, v1, v2, v3, c);
	generateColours(colours, this->depth);
}

// Recursively generate the smaller triangles
void Sierpinski::draw(std::vector<glm::vec3>& vertices, int depth, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 colour) {
	if (depth == 0) {
		// Base case: add the vertices and colours to the vectors
		vertices.push_back(v1);
		vertices.push_back(v2);
		vertices.push_back(v3);
	}
	else {
		// Recursive case: find the midpoints of the sides of the triangle
		glm::vec3 m1 = (v1 + v2) / 2.0f;
		glm::vec3 m2 = (v2 + v3) / 2.0f;
		glm::vec3 m3 = (v3 + v1) / 2.0f;

		glm::vec3 c1(0.5f, 0.2f, 0.2f);
		glm::vec3 c2(0.2f, 0.5f, 0.2f);
		glm::vec3 c3(0.2f, 0.2f, 0.5f);

		// Recursively draw the smaller triangles
		draw(vertices, depth - 1, v1, m1, m3, c1);
		draw(vertices, depth - 1, m1, v2, m2, c2);
		draw(vertices, depth - 1, m3, m2, v3, c3);
	}
}

void Sierpinski::generateColours(std::vector<glm::vec3>& colours, int depth) {
	// Clear the vectors, in case they are not empty
	colours.clear();

	// Push colors based on calculated colors
	float step = 1.0f / float(std::pow(3, depth));
	float steps = 0.0f;

	for (int i = 0; i < std::pow(3, depth); i++) {
		colours.push_back(glm::vec3(steps, steps, 1.0f - steps));
		colours.push_back(glm::vec3(steps, steps, 1.0f - steps));
		colours.push_back(glm::vec3(steps, steps, 1.0f - steps));
		steps += step;
	}
}
