#include "color.h"
#include <assert.h>

Color::Color()
{
	R = 0.0f;
	G = 0.0f;
	B = 0.0f;
}

Color::Color( float r, float g, float b)
{
	R = r;
	G = g;
	B = b;
}

Color Color::operator*(const Color& c) const
{
    return Color(R*c.R, G*c.G, B*c.B);
}

Color Color::operator*(const float Factor) const
{
	return Color(R*Factor, G*Factor, B*Factor);
}

Color Color::operator+(const Color& c) const
{
	return Color(R+c.R, G+c.G, B+c.B);
}

Color& Color::operator+=(const Color& c)
{
	this->R += c.R;
	this->G += c.G;
	this->B += c.B;
	return *this;
}