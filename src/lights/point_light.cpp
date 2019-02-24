#include "lights/point_light.h"

using namespace rend;

PointLight::PointLight(void)
    : colour({ 1.0f, 1.0f, 1.0f }), radius(1.0f), power(50.0f)
{
}

PointLight::PointLight(float r, float g, float b, float radius, float power)
    : colour({ r, g, b }), radius(radius), power(power)
{
}

PointLight::~PointLight(void)
{
}
