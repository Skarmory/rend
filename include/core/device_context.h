#pragma once

#ifndef REND_DEVICE_CONTEXT_H
#define REND_DEVICE_CONTEXT_H

#include "rend_defs.h"
#include "resource.h"

namespace rend
{

class DeviceContext
{
public:
    static DeviceContext& instance(void);

    virtual StatusCode create(void) = 0;
    virtual void       destroy(void) = 0;

    [[nodiscard]] virtual VertexBufferHandle create_vertex_buffer(uint32_t vertices_count, size_t vertex_size) = 0;
    [[nodiscard]] virtual IndexBufferHandle  create_index_buffer(uint32_t indices_count, size_t index_size) = 0;

protected:
    static DeviceContext* _service;
};

}

#endif