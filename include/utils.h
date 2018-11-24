#ifndef UTILS_H
#define UTILS_H

#include <vulkan/vulkan.h>
#include <iostream>

#define VULKAN_DEATH_CHECK(expr, msg)\
{\
    {\
        VkResult res = expr;\
        if(res != VK_SUCCESS)\
        {\
            std::cerr << "Vulkan error code: " << res << std::endl;\
            std::cerr << msg << std::endl;\
            std::abort();\
        }\
    }\
}

#define DEATH_CHECK(expr, msg)\
{\
    if(expr)\
    {\
        std::cerr << msg << std::endl;\
        std::abort();\
    }\
}

#endif
