#include "lights/point_light.h"

using namespace rend;

PointLight::PointLight(void)
    : colour({ 1.0f, 1.0f, 1.0f, 1.0f }), radius(1.0f), constant(1.0f), linear(0.1f), quadratic(0.01f)
{
}

PointLight::PointLight(float r, float g, float b, float radius, float constant, float linear, float quadratic)
    : colour({ r, g, b, 1.0f }), radius(radius), constant(constant), linear(linear), quadratic(quadratic)
{
}

PointLight::~PointLight(void)
{
}
