#ifndef REND_COMMAND_POOL_H
#define REND_COMMAND_POOL_H

#include "core/command_buffer.h"
#include "core/containers/data_array.h"

namespace rend
{

class CommandPool
{
    public:
        CommandPool(void);
        ~CommandPool(void);
        CommandPool(const CommandPool&)            = delete;
        CommandPool(CommandPool&&)                 = delete;
        CommandPool& operator=(const CommandPool&) = delete;
        CommandPool& operator=(CommandPool&&)      = delete;

        CommandPoolHandle handle(void) const;

        CommandBuffer* create_command_buffer(void);
        void           destroy_command_buffer(CommandBuffer* command_buffer);
        void           reset(void);

    private:
        CommandPoolHandle        _handle{ NULL_HANDLE };
        DataArray<CommandBuffer> _command_buffers;
};

}

#endif
