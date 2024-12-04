#pragma once

#include "Geometry.h"
#include <glm/gtx/transform.hpp>

// Based on given UnitCube.h
class UnitSphere {
public:
    CPU_Geometry m_cpu_geom;
    GPU_Geometry m_gpu_geom;
    glm::mat4 m_model;
    GLsizei m_size;
    void generateGeometry(float radius);
};

// Function Prototypes
std::vector<glm::vec3> generateSemicircle(float radius, int segments);
std::vector<glm::vec3> chaikinSubdivision(const std::vector<glm::vec3> control_points, int iterations);
std::pair<std::vector<glm::vec3>, std::vector<glm::vec2>> generateSphere(std::vector<glm::vec3>& curve_points, int n_slices, float radius);
std::vector<glm::vec2> getTexCoords(const std::vector<glm::vec3>& verts, int n_slices, int n_stacks);
std::vector<glm::vec3> getNormals(const std::vector<glm::vec3>& verts, const std::vector<glm::vec2>& tex_coords, int n_slices, int n_stacks);
std::vector<glm::vec3> getNormals(const std::vector<glm::vec3>& verts);
