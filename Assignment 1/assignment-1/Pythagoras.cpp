
#include "Pythagoras.h"
#include <iostream>
#include <glm/glm.hpp>
#include <cmath>

Pythagoras::Pythagoras(int depth) : depth(depth) {}

void Pythagoras::setDepth(int depth) {
	this->depth = depth;
}

int Pythagoras::getDepth() const {
	return depth;
}

void Pythagoras::generate(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colours) {
	vertices.clear();
	colours.clear();

	// Define the initial square with four corners
	glm::vec3 p1(-0.25f, -0.5f, 1.f);
	glm::vec3 p2(0.25f, -0.5f, 1.f);
	glm::vec3 p3(0.25f, 0.0f, 1.f);
	glm::vec3 p4(-0.25f, 0.0f, 1.f);

	// Define the base colour for the initial square
	glm::vec3 colour(0.5f, 0.5f, 0.5f);

	// Call the draw function to recursively generate the Pythagoras Tree
	draw(vertices, colours, this->depth, p1, p2, p3, p4, colour);
}

void Pythagoras::draw(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colours, int depth, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec3 colour) {
	if (depth == 0) {
		// Draw the square
		vertices.push_back(p1);
		vertices.push_back(p2);
		vertices.push_back(p3);
		vertices.push_back(p4);
		vertices.push_back(p1);

		colours.push_back(colour);
		colours.push_back(colour);
		colours.push_back(colour);
		colours.push_back(colour);
		colours.push_back(colour);
	}
	else {
		// Find the midpoints of the top edge of the square
		glm::vec3 mid = (p3 + p4) / 2.0f;

		// Calculate new points for the right isosceles triangle
		glm::vec3 direction = glm::normalize(p3 - p4);
		glm::vec3 perp(-direction.y, direction.x, 0.0f);

		// Height of the triangle
		float length = glm::length(p3 - p4);
		float height = length / sqrt(2.0f);

		// Calculate the new points for the right isosceles triangle
		glm::vec3 p5 = mid + (height / 2.0f) * perp;

		// normal vector of p4 to p5
		glm::vec3 normal = glm::normalize(glm::cross(p5 - p4, p5 - p3));

		glm::vec3 p6 = p5 + normal * height;
		glm::vec3 p7 = p4 + normal * height;

		draw(vertices, colours, depth - 1, p4, p5, p6, p7, colour);
	}
}

