#pragma once

class Rotation {
public:
	Rotation();
	Rotation(float value);
	float getValue();

	Rotation operator+(const Rotation& r) const;
	Rotation operator+(const float& r) const;
	Rotation operator-(const Rotation& r) const;
	Rotation operator-(const float& r) const;
	Rotation& operator=(const Rotation& r);
	Rotation& operator=(const float& r);
	Rotation& operator+=(const Rotation& r);

private:
	float value;

	static float clamp2Pi(float angle);
};
