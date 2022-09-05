#ifndef REND_CORE_MESH_H
#define REND_CORE_MESH_H

#include "core/gpu_resource.h"
#include "core/rend_defs.h"

#include <string>

namespace rend
{

class Mesh : public GPUResource
{
    public:
        Mesh(const std::string& name, BufferHandle vertex_buffer, BufferHandle index_buffer);
        ~Mesh(void);

        BufferHandle get_vertex_buffer(void) const { return _vertex_buffer; }
        BufferHandle get_index_buffer(void) const { return _index_buffer; }

    private:
        BufferHandle _vertex_buffer{ NULL_HANDLE };
        BufferHandle _index_buffer{ NULL_HANDLE };
};

}

#endif
