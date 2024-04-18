#ifndef REND_CORE_MESH_H
#define REND_CORE_MESH_H

#include "core/gpu_resource.h"
#include "core/rend_defs.h"
#include "core/rend_object.h"

#include <string>

namespace rend
{

class GPUBuffer;

class Mesh : public GPUResource, public RendObject
{
    public:
        Mesh(const std::string& name, GPUBuffer* vertex_buffer, GPUBuffer* index_buffer, RendHandle rend_handle);
        ~Mesh(void);

        GPUBuffer* get_vertex_buffer(void) const;
        GPUBuffer* get_index_buffer(void) const;

    private:
        GPUBuffer* _vertex_buffer{ nullptr };
        GPUBuffer* _index_buffer{ nullptr };
};

}

#endif
