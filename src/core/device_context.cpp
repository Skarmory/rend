#include "core/device_context.h"

#include <cassert>

using namespace rend;

DeviceContext* DeviceContext::_service{ nullptr };

DeviceContext& DeviceContext::instance(void)
{
	assert(_service);
	return *_service;
}
