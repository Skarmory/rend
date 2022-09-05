#ifndef REND_CORE_CAMERA_H
#define REND_CORE_CAMERA_H

#include <glm/glm.hpp>

namespace rend
{

struct CameraData
{
    glm::vec4 position{};
    glm::mat4 projection{};
    glm::mat4 view{};
};

}

#endif
