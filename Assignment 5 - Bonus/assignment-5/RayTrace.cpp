#include <iostream>
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "RayTrace.h"


using namespace std;
using namespace glm;

Sphere::Sphere(vec3 c, float r, int ID){
	centre = c;
	radius = r;
	id = ID;
}

//------------------------------------------------------------------------------
// This is part 2.1 of your assignment. At the moment, the spheres are not showing
// up. Implement this method to make them show up.
//
// Make sure you set all of the appropriate fields in the Intersection object.
//------------------------------------------------------------------------------
Intersection Sphere::getIntersection(Ray ray){
	Intersection i{};
	i.id = id;
	i.material = material;

	// You are required to implement this intersection.
	//
	// NOTE: You _must_ set these values appropriately for each case:
	//
	// No Intersection:
	// i.numberOfIntersections = 0;
	//
	// Intersection:
	// i.normal = **the normal at the point of intersection **
	// i.point = **the point of intersection**
	// i.numberOfIntersections = 1; // for a single intersection
	//
	// If you get fancy and implement things like refraction, you may actually
	// want to track more than one intersection. You'll need to change
	// The intersection struct in that case.

	vec3 L = ray.origin - centre;

	// Quadratic formula
	float a = 1.f; // dot(ray.direction, ray.direction) // 1
	float b = 2.f * dot(ray.direction, L);
	float c = dot(L, L) - radius * radius; // ||D||^2 - r^2

	float discriminant = b * b - 4 * a * c; // b^2 - 4ac

	// No real intersections (this is unreal)
	if (discriminant < 0) {
		i.numberOfIntersections = 0;
		return i;

	} else {
		// Real intersections (S**t's getting real)
		float tp = (-b + sqrt(discriminant)) / (2.f * a);
		float tn = (-b - sqrt(discriminant)) / (2.f * a);

		float t = 0.f;

		if (tp > 0.f && tp < tn) {
			t = tp;

		} else if (tn > 0.f && tn < tp) {
			t = tn;

		} else {
			t = -1.f;
		}

		if (t > 0.f) {
			i.point = ray.origin + (t * ray.direction);
			i.normal = normalize(i.point - centre);
			i.numberOfIntersections = 1;

		} else {
			i.numberOfIntersections = 0;
		}
	}

	return i;
}

//------------------------------------------------------------------------------
// This is part 2.2 of your assignment. At the moment, the cylinders are not showing
// up. Implement this method to make them show up.
//
// Make sure you set all of the appropriate fields in the Intersection object.
//------------------------------------------------------------------------------
Cylinder::Cylinder(vec3 c, float r, float h, vec3 ax, int ID)
{
	center = c;
	radius = r;
	height = h;
	axis = ax;
	id = ID;
}

Intersection Cylinder::getIntersection(Ray ray)
{
	Intersection i{};
	i.id = id;
	i.material = material;

	// You are required to implement this intersection.
	//
	// NOTE: You _must_ set these values appropriately for each case:
	//
	// No Intersection:
	// i.numberOfIntersections = 0;
	//
	// Intersection:
	// i.normal = **the normal at the point of intersection **
	// i.point = **the point of intersection**
	// i.numberOfIntersections = 1; // for a single intersection
	//
	// If you get fancy and implement things like refraction, you may actually
	// want to track more than one intersection. You'll need to change
	// The intersection struct in that case.

			// Create a coordinate system based on the cylinder's axis
	vec3 localZ = axis;
	vec3 localX, localY;

	// Create orthonormal basis
	if (std::abs(localZ.x) > std::abs(localZ.y)) {
		localY = vec3(0, 1, 0);
		localX = normalize(cross(localZ, localY));
		localY = cross(localX, localZ);
	}
	else {
		localX = vec3(1, 0, 0);
		localY = normalize(cross(localZ, localX));
		localX = cross(localY, localZ);
	}

	// Transform ray to cylinder's local coordinate system
	vec3 localOrigin = vec3(
		dot(ray.origin - center, localX),
		dot(ray.origin - center, localY),
		dot(ray.origin - center, localZ)
	);
	vec3 localDirection = vec3(
		dot(ray.direction, localX),
		dot(ray.direction, localY),
		dot(ray.direction, localZ)
	);

	// Closest intersection time
	float closestIntersection = std::numeric_limits<float>::max();
	bool hasIntersection = false;

	// Cylinder side intersection
	// Project ray direction and origin onto plane perpendicular to cylinder axis
	vec3 rayDirXY = vec3(localDirection.x, localDirection.y, 0);
	vec3 originXY = vec3(localOrigin.x, localOrigin.y, 0);

	// Quadratic formula coefficients for xy-plane intersection
	float a = dot(rayDirXY, rayDirXY);
	float b = 2.0f * dot(rayDirXY, originXY);
	float c = dot(originXY, originXY) - radius * radius;

	// Calculate discriminant
	float discriminant = b * b - 4 * a * c;

	// Side surface intersection
	if (discriminant >= 0) {
		float tp = (-b + sqrt(discriminant)) / (2.f * a);
		float tn = (-b - sqrt(discriminant)) / (2.f * a);

		// Find valid intersection times
		float t = (tp > 0 && (tp < tn || tn < 0)) ? tp : tn;

		if (t > 0) {
			vec3 localIntersectionPoint = localOrigin + (t * localDirection);

			// Check if intersection is within cylinder's height
			if (localIntersectionPoint.z >= -height / 2.0f && localIntersectionPoint.z <= height / 2.0f) {
				closestIntersection = t;
				hasIntersection = true;

				// Transform intersection point back to world coordinates
				i.point = center +
					(localIntersectionPoint.x * localX) +
					(localIntersectionPoint.y * localY) +
					(localIntersectionPoint.z * localZ);

				// Compute normal for side surface in world coordinates
				vec3 localNormal = vec3(localIntersectionPoint.x, localIntersectionPoint.y, 0);
				i.normal = normalize(
					localNormal.x * localX +
					localNormal.y * localY
				);
			}
		}
	}

	// Top cap intersection
	{
		float t = (-height / 2.0f - localOrigin.z) / localDirection.z;
		if (t > 0) {
			vec3 intersectionPoint = localOrigin + (t * localDirection);
			float distanceFromAxis = length(vec2(intersectionPoint.x, intersectionPoint.y));

			if (distanceFromAxis <= radius) {
				if (t < closestIntersection) {
					closestIntersection = t;
					hasIntersection = true;

					// Transform intersection point back to world coordinates
					i.point = center +
						(intersectionPoint.x * localX) +
						(intersectionPoint.y * localY) +
						(intersectionPoint.z * localZ);

					i.normal = localZ;
				}
			}
		}
	}

	// Bottom cap intersection
	{
		float t = (height / 2.0f - localOrigin.z) / localDirection.z;
		if (t > 0) {
			vec3 intersectionPoint = localOrigin + (t * localDirection);
			float distanceFromAxis = length(vec2(intersectionPoint.x, intersectionPoint.y));

			if (distanceFromAxis <= radius) {
				if (t < closestIntersection) {
					closestIntersection = t;
					hasIntersection = true;

					// Transform intersection point back to world coordinates
					i.point = center +
						(intersectionPoint.x * localX) +
						(intersectionPoint.y * localY) +
						(intersectionPoint.z * localZ);

					i.normal = -localZ;
				}
			}
		}
	}

	// Set intersection result
	if (hasIntersection) {
		i.numberOfIntersections = 1;
	}
	else {
		i.numberOfIntersections = 0;
	}

	return i;
};

Plane::Plane(vec3 p, vec3 n, int ID){
	point = p;
	normal = n;
	id = ID;
}


float dot_normalized(vec3 v1, vec3 v2){
	return glm::dot(glm::normalize(v1), glm::normalize(v2));
}

void debug(char* str, vec3 a){
	cout << "debug:" << str << ": " << a.x <<", " << a.y <<", " << a.z << endl;
}
// --------------------------------------------------------------------------
void Triangles::initTriangles(int num, vec3 * t, int ID){
	id = ID;
	for(int i = 0; i< num; i++){
		triangles.push_back(Triangle(*t, *(t+1), *(t+2)));
		t+=3;
	}
}

Intersection Triangles::intersectTriangle(Ray ray, Triangle triangle){
	// From https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
	const float EPSILON = 0.0000001;
	auto vertex0 = triangle.p1;
	auto vertex1 = triangle.p2;
	auto vertex2 = triangle.p3;

	glm::vec3 edge1, edge2, h, s, q;
	float a,f,u,v;
	edge1 = vertex1 - vertex0;
	edge2 = vertex2 - vertex0;
	h = glm::cross(ray.direction, edge2);
	a = glm::dot(edge1, h);
	if (a > -EPSILON && a < EPSILON) {
		return Intersection{}; // no intersection
	}
	f = 1.0/a;
	s = ray.origin - vertex0;
	u = f * glm::dot(s, h);
	if (u < 0.0 || u > 1.0) {
		return Intersection{}; // no intersection
	}
	q = glm::cross(s, edge1);
	v = f * glm::dot(ray.direction, q);
	if (v < 0.0 || u + v > 1.0) {
		return Intersection{}; // no intersection
	}
	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = f * glm::dot(edge2, q);
	// ray intersection
	if (t > EPSILON) {
		Intersection p;
		p.point = ray.origin + ray.direction * t;
		p.normal = glm::normalize(glm::cross(edge1, edge2));
		p.material = material;
		p.numberOfIntersections = 1;
		p.id = id;
		return p;
	} else {
		// This means that there is a line intersection but not a ray intersection.
		return Intersection{}; // no intersection
	}
}


Intersection Triangles::getIntersection(Ray ray){
	Intersection result{};
	result.material = material;
	result.id = id;
	float min = 9999;
	result = intersectTriangle(ray, triangles.at(0));
	if(result.numberOfIntersections!=0)min = glm::distance(result.point, ray.origin);
	for(int i = 1; i<triangles.size() ;i++){
		Intersection p = intersectTriangle(ray, triangles.at(i));
		if(p.numberOfIntersections !=0 && glm::distance(p.point, ray.origin) < min){
			min = glm::distance(p.point, ray.origin);
			result = p;
		}
	}

	result.material = material;
	result.id = id;
	return result;
}

Intersection Plane::getIntersection(Ray ray){
	Intersection result;
	result.material = material;
	result.id = id;
	result.normal = normal;
	if(dot(normal, ray.direction)>=0)return result;
	float s = dot(point - ray.origin, normal)/dot(ray.direction, normal);
	//if(s<0.00001)return result;
	result.numberOfIntersections = 1;
	result.point = ray.origin + s*ray.direction;
	return result;
}
