#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <typeinfo>

#define DUGL_NAMESPACE_BEGIN  namespace dugl {
#define DUGL_NAMESPACE_END    }

DUGL_NAMESPACE_BEGIN

using uint = unsigned int;
using uint32 = uint32_t;

template <typename T>
inline T* requireNonNull(T* ptr)
{
	if (!ptr) {
		throw std::invalid_argument(
			std::string("null pointer found of type ") + typeid(T).name());
	}
	return ptr;
}

DUGL_NAMESPACE_END