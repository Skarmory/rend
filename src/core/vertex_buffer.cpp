#include "vertex_buffer.h"

#include "device_context.h"

using namespace rend;

VertexBufferHandle VertexBuffer::get_handle() const
{
	return _handle;
}

bool VertexBuffer::create_vertex_buffer(uint32_t vertices_count, size_t vertex_size)
{
	auto& ctx = DeviceContext::instance();

	_handle = ctx.create_vertex_buffer(vertices_count, vertex_size);

	return _handle != NULL_HANDLE;
}
