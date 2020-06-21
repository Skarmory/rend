#include "vertex_buffer.h"

#include "device_context.h"

using namespace rend;

bool VertexBuffer::create_vertex_buffer(uint32_t vertices_count, size_t vertex_size)
{
	auto& ctx = DeviceContext::instance();

	_handle = ctx.create_vertex_buffer(vertices_count, vertex_size);

	if (_handle != NULL_HANDLE)
	{
		_bytes = vertices_count * vertex_size;

		return true;
	}

	return false;
}
