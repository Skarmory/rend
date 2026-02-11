#include "core/rend.h"

#include "core/renderer.h"
#include "core/logging/log_manager.h"

#include <GLFW/glfw3.h>

using namespace rend;
using namespace rend::core;

void rend::rend_initialise(const RendInitInfo& init_info)
{
    glfwInit();
    logging::LogManager::initialise();
    Renderer::create(init_info);
    Renderer::get_instance().initialise(init_info);
}

void rend::rend_uninitialise(void)
{
    Renderer::get_instance().uninitialise();
    Renderer::destroy();
    logging::LogManager::uninitialise();
    glfwTerminate();
}
