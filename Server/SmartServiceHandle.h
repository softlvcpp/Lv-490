#pragma once

#include "pch.h"


using SmartServiceHandle = std::unique_ptr<SC_HANDLE__, void(*)(SC_HANDLE)>;
class HandleManager
{
	static void HandleDeleter(SC_HANDLE handle)
	{
		if (handle != nullptr)
		{
			CloseServiceHandle(handle);
		}
	}
public:
	static SmartServiceHandle MakeSmart(SC_HANDLE handle)
	{
		if (handle == INVALID_HANDLE_VALUE)
		{
			return SmartServiceHandle(nullptr, &HandleDeleter);
		}
		return SmartServiceHandle(handle, &HandleDeleter);
	}
};

