#include "texture2D.h"

#include "device_context.h"
#include "image.h"
#include "logical_device.h"

#include <vulkan.h>

using namespace rend;

Texture2D::Texture2D(DeviceContext* context, uint32_t width, uint32_t height, uint32_t mip_levels, const TextureType type)
    : _context(context), _image(nullptr), _type(type)
{
    VkImageUsageFlags usage;
    VkFormat format;
    VkImageAspectFlags aspects;

    switch(_type)
    {
        case TextureType::TRANSFER:
            usage  = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
            format = VK_FORMAT_R8G8B8A8_UNORM;
            aspects = VK_IMAGE_ASPECT_COLOR_BIT;
            break;
        case TextureType::DIFFUSE:
            usage  = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            format = VK_FORMAT_R8G8B8A8_UNORM;
            aspects = VK_IMAGE_ASPECT_COLOR_BIT;
            break;
        case TextureType::DEPTH_BUFFER:
            usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            format = VK_FORMAT_D24_UNORM_S8_UINT;
            aspects = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
            break;
    };

    _image = new Image(_context);
    _image->create_image(
        VkExtent3D{ width, height, 1},
        VK_IMAGE_TYPE_2D,
        format,
        mip_levels,
        1,
        VK_SAMPLE_COUNT_1_BIT,
        VK_IMAGE_TILING_OPTIMAL,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        usage,
        VK_IMAGE_VIEW_TYPE_2D,
        aspects
    );
}

Texture2D::~Texture2D(void)
{
    delete _image;
}

Image* Texture2D::get_image(void) const
{
    return _image;
}

TextureType Texture2D::get_texture_type(void) const
{
    return _type;
}
