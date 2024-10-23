#ifndef ROTATION_H
#define ROTATION_H

class Rotation {
public:
	Rotation();
	Rotation(float value);
	float GetRawValue();

	Rotation operator+(const Rotation& r) const;
	Rotation operator+(const float& r) const;
	Rotation operator-(const Rotation& r) const;
	Rotation operator-(const float& r) const;
	Rotation& operator=(const Rotation& r);
	Rotation& operator=(const float& r);

private:
	float value;

	static float clamp2Pi(float angle);
};

#endif
