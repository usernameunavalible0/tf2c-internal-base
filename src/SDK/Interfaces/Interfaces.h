#pragma once
#include <Windows.h>

//GetInterface
template <typename T>
static T* GetInterface(const char* name, const char* library)
{
	const HINSTANCE handle = GetModuleHandle(library);

	if (!handle)
		return nullptr;

	using FN = T * (*)(const char*, int*);
	const FN CreateInterface = reinterpret_cast<FN>(GetProcAddress(handle, "CreateInterface"));

	return CreateInterface(name, nullptr);
}

namespace I {
	inline void* Client = nullptr;
	inline void* ClientMode = nullptr;
}