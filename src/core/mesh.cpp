#include "core/mesh.h"

using namespace rend;

Mesh::Mesh(const std::string& name, BufferHandle vertex_buffer, BufferHandle index_buffer)
    :
        GPUResource(name),
        _vertex_buffer(vertex_buffer),
        _index_buffer(index_buffer)
{
}

Mesh::~Mesh(void)
{
}
