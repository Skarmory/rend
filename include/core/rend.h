#ifndef REND_REND_H
#define REND_REND_H

#include <cstdint>

namespace rend
{
	class Window;

    enum class API
    {
        API_VULKAN
    };

    struct VulkanInfo
    {
        const char** extensions{ nullptr };
        uint32_t     extensions_count{ 0 };
        const char** layers{ nullptr };
        uint32_t     layers_count{ 0 };
    };

    struct RendInfo
    {
        API     api{ API::API_VULKAN };
        void*   api_info{ nullptr };
        Window* window;
    };

	void init_rend(const RendInfo& info);
	void destroy_rend(void);
}

#endif
