#ifndef POINT_LIGHT_H
#define POInt_LIGHT_H

namespace rend
{

struct PointLight
{
    PointLight(void);
    PointLight(float r, float g, float b, float radius, float constant, float linear, float quadratic);
    ~PointLight(void);

    struct
    {
        float r;
        float g;
        float b;
        float _;
    } colour;

    float radius;
    float constant;
    float linear;
    float quadratic;
};

}

#endif
