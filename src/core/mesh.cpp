#include "core/mesh.h"

#include "core/gpu_buffer.h"

using namespace rend;

Mesh::Mesh(const std::string& name, GPUBuffer* vertex_buffer, GPUBuffer* index_buffer)
    :
        GPUResource(name),
        _vertex_buffer(vertex_buffer),
        _index_buffer(index_buffer)
{
}

Mesh::~Mesh(void)
{
}

GPUBuffer* Mesh::get_vertex_buffer(void) const
{
    return _vertex_buffer;
}

GPUBuffer* Mesh::get_index_buffer(void) const
{
    return _index_buffer;
}
