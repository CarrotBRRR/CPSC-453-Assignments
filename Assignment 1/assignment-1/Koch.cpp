#include "Koch.h"
#include <iostream>
#include <glm/glm.hpp>

Koch::Koch(int depth) : depth(depth) {}

void Koch::setDepth(int depth) {
	this->depth = depth;
}

int Koch::getDepth() const {
	return depth;
}

void Koch::generate(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colours) {
	vertices.clear();
	colours.clear();

	// Define the initial triangle vertices for the Koch snowflake
	glm::vec3 p1(-0.5f, -0.5f, 0.f);
	glm::vec3 p2(0.5f, -0.5f, 0.f);
	glm::vec3 p3(0.f, 0.5f, 0.f);

	// Base colour for the Koch snowflake
	glm::vec3 colour(1.f, 1.f, 1.f);

	// Start generating the Koch Snowflake from each side of the triangle
	draw(vertices, colours, depth, p1, p2, colour);	// Base side
	draw(vertices, colours, depth, p2, p3, colour);	// Right side
	draw(vertices, colours, depth, p3, p1, colour);	// Left side
}


void Koch::draw(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colours, int depth, glm::vec3 p1, glm::vec3 p2, glm::vec3 colour) {
	if (depth == 0) {
		// Base case: add the line segment to the vertices
		vertices.push_back(p1);
		vertices.push_back(p2);

		// Assign colour to the vertices
		colours.push_back(colour);
		colours.push_back(colour);
	}
	else {
		// Calculate points for the Koch curve
		glm::vec3 segment = p2 - p1;
		glm::vec3 mid = (p1 + p2) / 2.0f;			// Midpoint of the segment
		glm::vec3 A = p1 + segment / 3.0f;			// Point A at 1/3 of the segment
		glm::vec3 B = p1 + segment * 2.0f / 3.0f;	// Point B at 2/3 of the segment

		// length of the new segment (1/3 of the original segment) ie magnitude of the new segment
		float length = glm::length(segment) / 3.0f;

		// cross product of the segment and the z-axis to get the normal vector
		glm::vec3 normal = glm::cross(segment, glm::vec3(0.0f, 0.0f, 1.0f));

		// get the new point C
		glm::vec3 C = mid + glm::normalize(normal) * length * sqrt(3.0f) / 2.0f;

		// Recursively draw the Koch curve
		draw(vertices, colours, depth - 1, p1, A, colour);
		draw(vertices, colours, depth - 1, A, C, colour);
		draw(vertices, colours, depth - 1, C, B, colour);
		draw(vertices, colours, depth - 1, B, p2, colour);
	}
}

