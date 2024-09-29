#include "Pythagoras.h"

#include <iostream>

using namespace std;

// Constructor
Pythagoras::Pythagoras(int depth) : depth(depth) {}

void Pythagoras::setDepth(int depth) {
	this->depth = depth;
}

int Pythagoras::getDepth() const {
	return depth;
}

// Generate verticies and colours for the Pythagoras tree

void Pythagoras::generate(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colours) {
	// Clear the vectors, in case they are not empty
	vertices.clear();
	colours.clear();

	// Define the initial square
	glm::vec3 v1(-0.5f, -0.5f, 1.f);
	glm::vec3 v2(0.5f, -0.5f, 1.f);
	glm::vec3 v3(0.5f, 0.5f, 1.f);
	glm::vec3 v4(-0.5f, 0.5f, 1.f);

	glm::vec3 c(1.f, 1.f, 1.f);

	draw(vertices, colours, this->depth, v1, v2, v3, v4, c);
}

// Recursively generate the smaller squares
void Pythagoras::draw(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colours, int depth, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, glm::vec3 colour) {
	if (depth == 0) {
		// Base case: add the vertices and colours to the vectors
		vertices.push_back(v1);
		vertices.push_back(v2);
		vertices.push_back(v3);
		vertices.push_back(v1);
		vertices.push_back(v3);
		vertices.push_back(v4);

		// Push colors based on calculated colors
		colours.push_back(colour);
		colours.push_back(colour);
		colours.push_back(colour);
		colours.push_back(colour);
		colours.push_back(colour);
		colours.push_back(colour);
	}
	else {
		// Recursive case: find the midpoints of the sides of the square
		glm::vec3 m1 = (v1 + v2) / 2.0f;
		glm::vec3 m2 = (v2 + v3) / 2.0f;
		glm::vec3 m3 = (v3 + v4) / 2.0f;
		glm::vec3 m4 = (v4 + v1) / 2.0f;

		glm::vec3 c1(0.5f, 0.2f, 0.2f);
		glm::vec3 c2(0.2f, 0.5f, 0.2f);
		glm::vec3 c3(0.2f, 0.2f, 0.5f);
		glm::vec3 c4(0.5f, 0.5f, 0.5f);

		// Recursively draw the smaller squares
		draw(vertices, colours, depth - 1, v1, m1, m2, m4, c1);
		draw(vertices, colours, depth - 1, m1, v2, m2, m3, c2);
		draw(vertices, colours, depth - 1, m4, m2, v3, m3, c3);
		draw(vertices, colours, depth - 1, m4, m3, v4, v1, c4);
	}
}
