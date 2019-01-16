#include "texture2D.h"

#include "device_context.h"
#include "image.h"
#include "logical_device.h"

#include <vulkan/vulkan.h>

using namespace rend;

Texture2D::Texture2D(DeviceContext* context, uint32_t width, uint32_t height, uint32_t mip_levels, const TextureType type)
    : _context(context), _image(nullptr), _type(type)
{
    VkImageUsageFlags usage;
    VkFormat format;

    switch(_type)
    {
        case TextureType::TRANSFER:
            usage  = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
            format = VK_FORMAT_R8G8B8A8_UNORM;
            break;
        case TextureType::DIFFUSE:
            usage  = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            format = VK_FORMAT_R8G8B8A8_UNORM;
            break;
    };

    _image = _context->get_device()->create_image(
            VkExtent3D{ width, height, 1},
            VK_IMAGE_TYPE_2D,
            format,
            mip_levels,
            1,
            VK_SAMPLE_COUNT_1_BIT,
            VK_IMAGE_TILING_OPTIMAL,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            usage,
            VK_IMAGE_VIEW_TYPE_2D
        );
}

Texture2D::~Texture2D(void)
{
    _context->get_device()->destroy_image(&_image);
}

Image* Texture2D::get_image(void) const
{
    return _image;
}

TextureType Texture2D::get_texture_type(void) const
{
    return _type;
}
