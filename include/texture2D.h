#ifndef TEXTURE_2D_H
#define TEXTURE_2D_H

#include <cstdint>

namespace rend
{

class DeviceContext;
class Image;

enum class TextureType
{
    TRANSFER,
    DIFFUSE
};

class Texture2D
{
public:
    Texture2D(DeviceContext* context, uint32_t width, uint32_t height, uint32_t mip_levels, const TextureType type);
    ~Texture2D(void);

    Image*      get_image(void) const;
    TextureType get_texture_type(void) const;

private:
    DeviceContext* _context;
    Image*         _image;
    TextureType    _type;
};

}

#endif
