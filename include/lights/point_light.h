#ifndef POINT_LIGHT_H
#define POInt_LIGHT_H

namespace rend
{

struct PointLight
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
    float _pad2[2];
};

}

#endif
