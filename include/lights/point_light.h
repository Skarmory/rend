//#ifndef REND_LIGHTS_POINT_LIGHT_H
//#define REND_LIGHTS_POINT_LIGHT_H
//
//#include <cstdint>
//
//namespace rend
//{
//
//static constexpr uint32_t POINT_LIGHT_MAX = 256;
//
//struct alignas(16) PointLight
//{
//    PointLight(void) = default;
//    PointLight(float r, float g, float b, float radius, float power);
//    ~PointLight(void) = default;
//
//    struct
//    {
//        float r{ 1.0f };
//        float g{ 1.0f };
//        float b{ 1.0f };
//    } colour;
//
//    float _pad{ 0.0f };
//    float radius{ 1.0f };
//    float power{ 50.0f };
//};
//
//}
//
//#endif
