#define _USE_MATH_DEFINES
#include <cmath>

#define RAYTRACING_THREADS 16

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include <glm/gtx/vector_query.hpp>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "imagebuffer.h"
#include "RayTrace.h"
#include "Scene.h"
#include "Lighting.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <omp.h>

struct Parameters {
	bool refractions = true;
};

static Parameters params;

vec3 refract(vec3 I, vec3 N, float refractiveIndex) {
	float cosi = glm::dot(I, N);
	float etai = 1, etat = refractiveIndex;

	vec3 n = N;
	if (cosi < 0) {
		cosi = -cosi;
		std::swap(etai, etat);
		n = -N;
	}

	float eta = etai / etat;
	float k = 1 - eta * eta * (1 - cosi * cosi);

	return k < 0 ? glm::reflect(I, N) : glm::normalize(eta * I - (eta * cosi + sqrtf(k)) * n);
}

int hasIntersection(Scene const &scene, Ray ray, int skipID){
	for (auto &shape : scene.shapesInScene) {
		Intersection tmp = shape->getIntersection(ray);
		if(
			shape->id != skipID
			&& tmp.numberOfIntersections!=0
			&& glm::distance(tmp.point, ray.origin) > 0.00001
			&& glm::distance(tmp.point, ray.origin) < glm::distance(ray.origin, scene.lightPosition) - 0.01
		){
			return tmp.id;
		}
	}
	return -1;
}

Intersection getClosestIntersection(Scene const &scene, Ray ray, int skipID){ //get the nearest
	Intersection closestIntersection;
	float min = std::numeric_limits<float>::max();
	for(auto &shape : scene.shapesInScene) {
		if(skipID == shape->id) {
			// Sometimes you need to skip certain shapes. Useful to
			// avoid self-intersection. ;)
			continue;
		}
		Intersection p = shape->getIntersection(ray);
		float distance = glm::distance(p.point, ray.origin);
		if(p.numberOfIntersections !=0 && distance < min){
			min = distance;
			closestIntersection = p;
		}
	}
	return closestIntersection;
}

glm::vec3 raytraceSingleRay(Scene const &scene, Ray const &ray, int level, int source_id) {
	// TODO: Part 3: Somewhere in this function you will need to add the code to determine
	//               if a given point is in shadow or not. Think carefully about what parts
	//               of the lighting equation should be used when a point is in shadow.
	// TODO: Part 4: Somewhere in this function you will need to add the code that does reflections and refractions.
	//               NOTE: The ObjectMaterial class already has a parameter to store the object's
	//               reflective properties. Use this parameter + the color coming back from the
	//               reflected array and the color from the phong shading equation.
	Intersection result = getClosestIntersection(scene, ray, source_id); //find intersection

	PhongReflection phong;
	phong.ray = ray;
	phong.scene = scene;
	phong.material = result.material;
	phong.intersection = result;

	glm::vec3 local_color = phong.I();

	if(result.numberOfIntersections == 0) return glm::vec3(0, 0, 0); // black;

	if (level < 1) {
		phong.material.reflectionStrength = glm::vec3(0);
	}

	// Max Depth
	if (level > 10) {
		return phong.I();
	}

	// Part 3
	Ray shadow_ray(result.point + (phong.l() * 0.001f), phong.l()); // Add offset to avoid grains

	for (const auto& shape : scene.shapesInScene) {
		Intersection shadow_result = shape->getIntersection(shadow_ray);

		if (shadow_result.numberOfIntersections > 0) {
			float d_shadow = glm::distance(result.point, shadow_result.point);
			float d_light = glm::distance(result.point, scene.lightPosition);

			if (d_light >= d_shadow) {
				local_color = phong.Ia();
			}
		}
	}

	// Part 4
	// Reflection
	glm::vec3 reflection_dir = glm::normalize(glm::reflect(ray.direction, result.normal));
	Ray reflection_ray(result.point + (reflection_dir * 0.001f), reflection_dir);

	glm::vec3 reflected = raytraceSingleRay(scene, reflection_ray, level + 1, result.id) * phong.material.reflectionStrength;

	// Refraction
	glm::vec3 refracted(0.f);

	if (phong.material.reflectionStrength != glm::vec3(0.f) && params.refractions) {
		glm::vec3 refraction_dir = refract(ray.direction, result.normal, phong.material.refractiveIndex);
		Ray refraction_ray(result.point + (refraction_dir * 0.001f), refraction_dir);

		refracted = raytraceSingleRay(scene, refraction_ray, level + 1, result.id) * 0.4f;
	}

	return local_color + reflected + refracted;
}

struct RayAndPixel {
	Ray ray;
	int x;
	int y;
};

std::vector<RayAndPixel> getRaysForViewpoint(Scene const &scene, ImageBuffer &image, glm::vec3 viewPoint) {
	// This is the function you must implement for part 1
	//
	// This function is responsible for creating the rays that go
	// from the viewpoint out into the scene with the appropriate direction
	// and angles to produce a perspective image.

	// Part 1
	std::vector<RayAndPixel> rays;

	float w = image.Width();
	float h = image.Height();
	float aspect_ratio = w / h;

	constexpr float FoV = glm::radians(45.0f);
	float view_dist = 10.f;

	for (int x = 1; x < w; x++) {
		float u = (2.f * x) / w - 1.f;

		for (int y = 1; y < h; y++) {
			float v =  (2.f * y) / h - 1.f;

			glm::vec3 direction = glm::normalize(
				glm::vec3(
					u * aspect_ratio * glm::tan(FoV/2.f),
					v * glm::tan(FoV / 2.f),
					view_dist / -10.f
				)
			);

			Ray ray(viewPoint, direction);
			rays.push_back({ ray, x, y });
		}
	}

	return rays;
}

void raytraceImage(Scene const &scene, ImageBuffer &image, glm::vec3 viewPoint) {
	// Reset the image to the current size of the screen.
	image.Initialize();

	// Get the set of rays to cast for this given image / viewpoint
	std::vector<RayAndPixel> rays = getRaysForViewpoint(scene, image, viewPoint);

	// This loops processes each ray and stores the resulting pixel in the image.
	// final color into the image at the appropriate location.
	//
	// I've written it this way, because if you're keen on this, you can
	// try and parallelize this loop to ensure that your ray tracer makes use
	// of all of your CPU cores
	//
	// Note, if you do this, you will need to be careful about how you render
	// things below too
	// std::for_each(std::begin(rays), std::end(rays), [&] (auto const &r) {

	omp_set_num_threads(RAYTRACING_THREADS);
	#pragma omp parallel for

	for (int i = 0; i < rays.size(); i++) {
		glm::vec3 color = raytraceSingleRay(scene, rays[i].ray, 1, -1);
		image.SetPixel(rays[i].x, rays[i].y, color);
	}
}

// EXAMPLE CALLBACKS
class Assignment5 : public CallbackInterface {

public:
	Assignment5() {
		viewPoint = glm::vec3(0, 0, 0);
		scene = initScene1();
		raytraceImage(scene, outputImage, viewPoint);
	}

	virtual void keyCallback(int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
			shouldQuit = true;
		}

		if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
			scene = initScene1();
			raytraceImage(scene, outputImage, viewPoint);
		}

		if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
			scene = initScene2();
			raytraceImage(scene, outputImage, viewPoint);
		}

		if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
			scene = initScene3();
			raytraceImage(scene, outputImage, viewPoint);
		}

		if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
			scene = initScene4();
			raytraceImage(scene, outputImage, viewPoint);
		}

		if (key == GLFW_KEY_F && action == GLFW_PRESS) {
			params.refractions = !params.refractions;
			raytraceImage(scene, outputImage, viewPoint);
		}
	}

	bool shouldQuit = false;

	ImageBuffer outputImage;
	Scene scene;
	glm::vec3 viewPoint;

};
// END EXAMPLES


int main() {
	Log::debug("Starting main");

	// WINDOW
	glfwInit();

	// Change your image/screensize here.
	int width = 800;
	int height = 800;
	Window window(width, height, "CPSC 453: Assignment 5");

	GLDebug::enable();

	// CALLBACKS
	std::shared_ptr<Assignment5> a5 = std::make_shared<Assignment5>(); // can also update callbacks to new ones
	window.setCallbacks(a5); // can also update callbacks to new ones

	// RENDER LOOP
	while (!window.shouldClose() && !a5->shouldQuit) {
		glfwPollEvents();

		glEnable(GL_FRAMEBUFFER_SRGB);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		a5->outputImage.Render();

		window.swapBuffers();
	}


	// Save image to file:
	// outpuImage.SaveToFile("foo.png")

	glfwTerminate();
	return 0;
}
