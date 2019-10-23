#include "sampled_texture.h"
#include "rend_defs.h"

using namespace rend;

SampledTexture::SampledTexture(DeviceContext& context)
    :
#ifdef USE_VULKAN
        VulkanSampledTexture(context)
#endif
{
}

SampledTexture::~SampledTexture(void)
{
}

bool SampledTexture::create_sampled_texture(uint32_t width, uint32_t height, uint32_t depth, uint32_t mip_levels, TextureFormat format)
{
    create_texture_base(width, height, depth, format);

    if(create_sampled_texture_api(width, height, depth, mip_levels, format) != StatusCode::SUCCESS)
    {
        return false;
    }

    return true;
}