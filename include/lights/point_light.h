#ifndef POINT_LIGHT_H
#define POInt_LIGHT_H

namespace rend
{

struct alignas(16) PointLight
{
    PointLight(void);
    PointLight(float r, float g, float b, float radius, float power);
    ~PointLight(void);

    struct
    {
        float r;
        float g;
        float b;
    } colour;

    float _pad;
    float radius;
    float power;
};

}

#endif
