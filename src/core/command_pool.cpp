#include "core/command_pool.h"

#include "core/device_context.h"

#include <new>
#include <iostream>

using namespace rend;

CommandPool::CommandPool(void)
{
    auto& ctx = DeviceContext::instance();

    _handle = ctx.create_command_pool();
}

CommandPool::~CommandPool(void)
{
    auto& ctx = DeviceContext::instance();

    for(auto& command_buffer : _command_buffers)
    {
        ctx.destroy_command_buffer(command_buffer.handle(), _handle);
        delete &command_buffer;
    }

    ctx.destroy_command_pool(_handle);
}

CommandBuffer* CommandPool::create_command_buffer(void)
{
    auto& ctx = DeviceContext::instance();

    CommandBufferHandle command_buffer_handle = ctx.create_command_buffer(_handle);

    auto [pool_reference_handle, command_buffer] = _command_buffers.allocate();

    new (command_buffer) CommandBuffer(command_buffer_handle, pool_reference_handle);

    return command_buffer;
}

void CommandPool::destroy_command_buffer(CommandBuffer* command_buffer)
{
    auto& ctx = DeviceContext::instance();

    ctx.destroy_command_buffer(command_buffer->_handle, _handle);

    _command_buffers.deallocate(command_buffer->_pool_reference_handle);

    command_buffer->~CommandBuffer();
}
