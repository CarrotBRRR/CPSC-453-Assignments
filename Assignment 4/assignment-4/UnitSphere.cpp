#define _USE_MATH_DEFINES

#include "UnitSphere.h"
#include <cmath>

void UnitSphere::generateGeometry(float radius){
    // Generate control points for a semicircle
    std::vector<glm::vec3> control_points = generateSemicircle(radius, 16);

    // Chaikin Subdivision
    std::vector<glm::vec3> curve_points = chaikinSubdivision(control_points, 3);

	std::reverse(curve_points.begin(), curve_points.end());

    // Generate the Sphere
    auto [surface_points, tex_coords] = generateSphere(curve_points, 30, radius);

    // Calculate Normals
    std::vector<glm::vec3> normals = getNormals(surface_points, tex_coords, 16, 16);

    // Set vertices and tex_coords for CPU_Geometry
    m_cpu_geom.verts = surface_points;
    m_cpu_geom.cols = std::vector<glm::vec3>(m_cpu_geom.verts.size(), glm::vec3(0.f, 0.f, 0.f));
    m_cpu_geom.normals = normals;
    m_cpu_geom.tex_coords = tex_coords;

	m_size = m_cpu_geom.verts.size();

    // Set vertices and tex_coords for GPU_Geometry
    m_gpu_geom.bind();
    m_gpu_geom.setVerts(m_cpu_geom.verts);
    m_gpu_geom.setCols(m_cpu_geom.cols);
    m_gpu_geom.setNormals(m_cpu_geom.normals);
    m_gpu_geom.setTexCoords(m_cpu_geom.tex_coords);
}

std::vector<glm::vec3> generateSemicircle(float radius, int segments){
    std::vector<glm::vec3> control_points;

    for (int i = 0; i < segments; i++){
        float theta = i * M_PI / (segments - 1); 

		control_points.push_back(glm::vec3(radius * sin(theta), radius * cos(theta), 0.f));
    }

    return control_points;
}

std::vector<glm::vec3> chaikinSubdivision(const std::vector<glm::vec3> control_points, int iterations){
    if (control_points.size() < 2){
        return std::vector<glm::vec3>{glm::vec3(0, 0, 0)};
    } else if (iterations == 0){
        return control_points;
    }

    std::vector<glm::vec3> curve_points;
    std::vector<glm::vec3> temp = control_points;

    // Beginning Mask
    curve_points.push_back(temp[0]);
    curve_points.push_back((0.5f * temp[0]) + (0.5f * temp[1]));

    for(int i =0; i < int(temp.size() - 2); i++){
        curve_points.push_back((0.75f * temp[i]) + (0.25f * temp[i + 1]));
        curve_points.push_back((0.25f * temp[i]) + (0.75f * temp[i + 1]));
    }

    // Ending Mask
    curve_points.push_back((0.5f * temp[temp.size() - 2]) + (0.5f * temp[temp.size() - 1]));
    curve_points.push_back(temp[temp.size() - 1]);

    return chaikinSubdivision(curve_points, iterations - 1);
}

std::pair<std::vector<glm::vec3>, std::vector<glm::vec2>> generateSphere(std::vector<glm::vec3>& curve_points, int n_slices, float radius){
    std::vector<glm::vec3> surface_points;
    std::vector<glm::vec2> tex_coords;

    float theta = 2.f * M_PI / n_slices;

    for(int i = 0; i < curve_points.size() - 1; i++){
        glm::vec3 p0 = curve_points[i];
        glm::vec3 p1 = curve_points[i + 1];

        for(int j = 0; j < n_slices; j++){
            float phi = j * theta;
            float phi_next = (j + 1) * theta;

            // Vertices
            glm::vec3 vert1(p0.x * cos(phi), p0.y, p0.x * sin(phi));
            glm::vec3 vert2(p1.x * cos(phi), p1.y, p1.x * sin(phi));
            glm::vec3 vert3(p1.x * cos(phi_next), p1.y, p1.x * sin(phi_next));
            glm::vec3 vert4(p0.x * cos(phi_next), p0.y, p0.x * sin(phi_next));

            surface_points.push_back(vert1);
            surface_points.push_back(vert2);
            surface_points.push_back(vert3);

            surface_points.push_back(vert1);
            surface_points.push_back(vert3);
            surface_points.push_back(vert4);

			// Texture Coordinates
			float u1 = 1.0f - (phi / (2.f * M_PI));
			float u2 = 1.0f - (phi_next / (2.f * M_PI));

			float v1 = (p0.y + radius) / (2 * radius);
			float v2 = (p1.y + radius) / (2 * radius);

			tex_coords.push_back(glm::vec2(u1, v1));
			tex_coords.push_back(glm::vec2(u1, v2));
			tex_coords.push_back(glm::vec2(u2, v2));

			tex_coords.push_back(glm::vec2(u1, v1));
			tex_coords.push_back(glm::vec2(u2, v2));
			tex_coords.push_back(glm::vec2(u2, v1));
        }
    }

    return std::make_pair(surface_points, tex_coords);
}

std::vector<glm::vec2> getTexCoords(const std::vector<glm::vec3>& verts, int n_slices, int n_stacks){
    std::vector<glm::vec2> tex_coords;

    for(float stack = 0.f; stack < n_stacks; stack++){
        float v = stack / n_stacks; // [0, 1] Latitude

        for(float slice = 0.f; slice < n_slices; slice++){
            float u = slice / n_slices; // [0, 1] Longitude

            tex_coords.push_back(glm::vec2(u, v));
        }
    }

    return tex_coords;
}

std::vector<glm::vec3> getNormals(const std::vector<glm::vec3>& verts, const std::vector<glm::vec2>& tex_coords, int n_slices, int n_stacks){
    std::vector<glm::vec3> normals;

    for(int i = 0; i < verts.size(); i++){
        glm::vec3 v0 = verts[i];
        glm::vec3 v1 = verts[(i + 1) % verts.size()];
        glm::vec3 v2 = verts[(i + 2) % verts.size()];

        glm::vec3 e1 = v1 - v0;
        glm::vec3 e2 = v2 - v0;

        glm::vec3 normal = glm::cross(e1, e2);
        normals.push_back(normal);
    }

    return normals;
}

std::vector<glm::vec3> getNormals(const std::vector<glm::vec3>& verts){
    std::vector<glm::vec3> normals;

    for(int i = 0; i < verts.size(); i += 3){
        glm::vec3 normal = glm::normalize(glm::cross(verts[i + 1] - verts[i], verts[i + 2] - verts[i]));
        normals.push_back(normal);
    }

    return normals;
}
