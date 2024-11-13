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
    Renderer::initialise(init_info);
    Renderer::get_instance().configure();
}

void rend::rend_uninitialise(void)
{
    Renderer::get_instance().shutdown();
    logging::LogManager::uninitialise();
    glfwTerminate();
}
