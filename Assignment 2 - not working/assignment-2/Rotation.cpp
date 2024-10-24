#define _USE_MATH_DEFINES

#include "Rotation.h"
#include <cmath>

Rotation::Rotation() {
	value = 0;
}

Rotation::Rotation(float value) {
	this->value = clamp2Pi(value);
}

float Rotation::getValue() {
	return value;
}

Rotation Rotation::operator+(const Rotation& r) const {
	return Rotation(value + r.value);
}

Rotation Rotation::operator+(const float& r) const {
	return Rotation(value + r);
}

Rotation Rotation::operator-(const Rotation& r) const {
	return Rotation(value - r.value);
}

Rotation Rotation::operator-(const float& r) const {
	return Rotation(value - r);
}

Rotation& Rotation::operator=(const Rotation& r) {
	value = r.value;
	return *this;
}

Rotation& Rotation::operator=(const float& r) {
	value = r;
	return *this;
}

Rotation& Rotation::operator+=(const Rotation& r) {
	value += r.value;
	value = clamp2Pi(value);
	return *this;
}

float Rotation::clamp2Pi(float angle) {
	return fmod(angle, 2*M_PI);
}
