#include "rend/core/rend.h"

#include "rend/core/renderer.h"
#include "rend/core/logging/log_manager.h"

#include <GLFW/glfw3.h>

using namespace rend;
using namespace rend::core;

void rend::rend_initialise(const RendInitInfo& init_info)
{
    glfwInit();
    logging::LogManager::initialise();
    Renderer::create(init_info);
}

void rend::rend_uninitialise(void)
{
    Renderer::destroy();
    logging::LogManager::uninitialise();
    glfwTerminate();
}
