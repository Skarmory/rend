#ifndef REND_CORE_MESH_H
#define REND_CORE_MESH_H

#include "rend/rend_api.h"
#include "rend/core/gpu_resource.h"
#include "rend/core/rend_defs.h"
#include "rend/core/rend_object.h"

#include <string>

namespace rend
{

class GPUBuffer;

class REND_API Mesh : public GPUResource, public RendObject
{
    public:
        Mesh(const std::string& name, GPUBuffer* vertex_buffer, GPUBuffer* index_buffer);
        ~Mesh(void);

        GPUBuffer* get_vertex_buffer(void) const;
        GPUBuffer* get_index_buffer(void) const;

    private:
        GPUBuffer* _vertex_buffer{ nullptr };
        GPUBuffer* _index_buffer{ nullptr };
};

}

#endif
