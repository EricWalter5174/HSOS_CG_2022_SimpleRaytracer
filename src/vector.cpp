#include "vector.h"
#include <assert.h>
#include <math.h>

Vector::Vector( float x, float y, float z):X(x), Y(y), Z(z)
{
	// TODO: add your code
    
}

Vector::Vector():X(0),Y(0),Z(0)
{
	// TODO: add your code
}

float Vector::dot(const Vector& v) const
{
    float res = this->X*v.X + this->Y*v.Y + this->Z*v.Z;
    
	return res;
}

Vector Vector::cross(const Vector& v) const
{
    Vector res;

res.X = this->Y * v.Z - this->Z * v.Y;
res.Y = this->Z * v.X - this->X * v.Z;
res.Z = this->X * v.Y - this->Y * v.X;

return res;
}

Vector Vector::operator+(const Vector& v) const
{
	Vector res;

	res.X = this->X + v.X;
	res.Y = this->Y + v.Y;
	res.Z = this->Z + v.Z;

	return res;
}

Vector Vector::operator-(const Vector& v) const
{
	Vector res;

	res.X = this->X - v.X;
	res.Y = this->Y - v.Y;
	res.Z = this->Z - v.Z;

	return res;
}

Vector Vector::operator*(float c) const
{
	Vector res;

	res.X = this->X * c;
	res.Y = this->Y * c;
	res.Z = this->Z * c;

	return res;
}

Vector Vector::operator-() const
{
	Vector res;

	res.X = this->X * (-1);
	res.Y = this->Y * (-1);
	res.Z = this->Z * (-1);

	return res;
}

Vector& Vector::operator+=(const Vector& v)
{
	this->X += v.X;
	this->Y += v.Y;
	this->Z += v.Z;

	return *this;
}

Vector& Vector::normalize()
{
	float factor = 1.0f / this->length();

	this->X *= factor;
	this->Y *= factor;
	this->Z *= factor;

	return *this;
}

float Vector::length() const
{
	float length = sqrt(this->lengthSquared());

	return length;
}

float Vector::lengthSquared() const
{
	float res = this->X * this->X + this->Y * this->Y + this->Z * this->Z;

	return res;
}

Vector Vector::reflection(const Vector& normal) const
{
	Vector reflection = *this - normal * (2 * (this->dot(normal)));
	return reflection;
}

bool Vector::triangleIntersection(const Vector& d, const Vector& a, const Vector& b, const Vector& c, float& s) const
{
	Vector nUp = (b - a).cross(c - a);
	float nDown = nUp.length();
	Vector normal = (nUp * (1.0f / nDown));

	float distance = normal.dot(a); // Berechnen von d
	s = (distance - (normal.dot(*this))) / (normal.dot(d)); // Berechnen von s

	if (s < 1e-5){
		return false;
	}

	Vector p = *this + (d * s); 
	float result = normal.dot(p) - distance;

		float areaABC = a.areaTriangle(b, c) + 0.00001f; // Aufgrund der Ungenauigkeit von Float
		float areaABP = a.areaTriangle(b, p);
		float areaACP = a.areaTriangle(c, p);
		float areaBCP = b.areaTriangle(c, p);
		return (areaABC >= (areaABP + areaACP + areaBCP));
}

float Vector::areaTriangle(const Vector& v, const Vector& w) const {
	return (((v - *this).cross(w - *this)).length() / 2);
}

bool Vector::operator==(const Vector& v) const {
	return (this->X == v.X && this->Y == v.Y && this->Z == v.Z);
}
