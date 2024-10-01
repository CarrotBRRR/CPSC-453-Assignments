#include "Hilbert.h"
#include <iostream>

Hilbert::Hilbert(int depth) : depth(depth) {}

void Hilbert::setDepth(int depth) {
	this->depth = depth;
}

int Hilbert::getDepth() const {
	return depth;
}

void Hilbert::generate(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colours) {
	vertices.clear();
	colours.clear();

	// Starting points for the Hilbert Curve
	glm::vec3 p1(-0.5f, -0.5f, 0.0f);
	glm::vec3 p2(-0.5f, 0.5f, 0.0f);
	glm::vec3 p3(0.5f, 0.5f, 0.0f);
	glm::vec3 p4(0.5f, -0.5f, 0.0f);

	// Base color for the Hilbert Curve
	glm::vec3 colour(0.0f, 0.5f, 1.0f); // Blue color

	// Generate the Hilbert Curve
	draw(vertices, colours, depth, p1, p2, p3, p4, colour);
}

void Hilbert::draw(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colours, int depth, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec3 color) {
	if (depth == 0) {
		// Connect the points as a sequence of line segments (for GL_LINES)
		vertices.push_back(p1); vertices.push_back(p2); // Line segment from p1 to p2
		vertices.push_back(p2); vertices.push_back(p3); // Line segment from p2 to p3
		vertices.push_back(p3); vertices.push_back(p4); // Line segment from p3 to p4

		// Add color for each vertex (same color for both points in a segment)
		colours.push_back(color); colours.push_back(color);
		colours.push_back(color); colours.push_back(color);
		colours.push_back(color); colours.push_back(color);
	}
	else {
		// Midpoints of each edge
		glm::vec3 mid12 = (p1 + p2) / 2.0f;
		glm::vec3 mid23 = (p2 + p3) / 2.0f;
		glm::vec3 mid34 = (p3 + p4) / 2.0f;
		glm::vec3 mid41 = (p4 + p1) / 2.0f;
		glm::vec3 center = (p1 + p3) / 2.0f;

		// Recursively draw the Hilbert curve in the correct order
		draw(vertices, colours, depth - 1, p1, mid12, center, mid41, color);     // Lower left (rotated 90° clockwise)
		draw(vertices, colours, depth - 1, mid12, p2, mid23, center, color);     // Upper left
		draw(vertices, colours, depth - 1, center, mid23, p3, mid34, color);     // Upper right
		draw(vertices, colours, depth - 1, mid41, center, mid34, p4, color);     // Lower right (rotated 90° counterclockwise)
	}
}
