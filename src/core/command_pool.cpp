#include "core/command_pool.h"

#include "core/device_context.h"
#include "core/rend_service.h"

#include <new>
#include <iostream>

using namespace rend;

CommandPool::CommandPool(void)
{
    auto& ctx = *RendService::device_context();

    _handle = ctx.create_command_pool();
}

CommandPool::~CommandPool(void)
{
    auto& ctx = *RendService::device_context();

    for(auto& command_buffer : _command_buffers)
    {
        ctx.destroy_command_buffer(command_buffer.handle(), _handle);
        delete &command_buffer;
    }

    ctx.destroy_command_pool(_handle);
}

CommandBuffer* CommandPool::create_command_buffer(void)
{
    auto& ctx = *RendService::device_context();

    CommandBufferHandle pool_ref_handle = _command_buffers.allocate();

    CommandBuffer* new_buffer = _command_buffers.get(pool_ref_handle);
    new_buffer->_handle = ctx.create_command_buffer(_handle);
    new_buffer->_pool_reference_handle = pool_ref_handle;

    return new_buffer;
}

void CommandPool::destroy_command_buffer(CommandBuffer* command_buffer)
{
    auto& ctx = *RendService::device_context();

    ctx.destroy_command_buffer(command_buffer->_handle, _handle);

    _command_buffers.deallocate(command_buffer->_pool_reference_handle);
}
