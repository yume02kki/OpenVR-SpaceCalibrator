#pragma once

#include "Logging.h"
#include <funchook.h>
#include <map>
#include <string>

class IHook
{
public:
	const std::string name;

	IHook(const std::string &name) : name(name) { }
	virtual ~IHook() { }

	virtual void Destroy() = 0;

	static bool Exists(const std::string &name);
	static void Register(IHook *hook);
	static void Unregister(IHook *hook);
	static void DestroyAll();

	static funchook_t *funchook;

private:
	static std::map<std::string, IHook *> hooks;
};

template<class FuncType> class Hook : public IHook
{
public:
	FuncType originalFunc = nullptr;
	Hook(const std::string &name) : IHook(name) { }

	bool CreateHookInObjectVTable(void *object, int vtableOffset, void *detourFunction)
	{
		void **vtable = *((void ***)object);
		originalFunc = (FuncType)vtable[vtableOffset];

		if (!funchook)
			funchook = funchook_create();

		int rv = funchook_prepare(funchook, (void **)&originalFunc, detourFunction);
		if (rv != 0)
		{
			LOG("Failed to prepare hook for %s, error: %d", name.c_str(), rv);
			return false;
		}

		rv = funchook_install(funchook, 0);
		if (rv != 0)
		{
			LOG("Failed to install hook for %s, error: %d", name.c_str(), rv);
			return false;
		}

		LOG("Enabled hook for %s", name.c_str());
		enabled = true;
		return true;
	}

	void Destroy()
	{
		if (enabled && funchook)
		{
			funchook_uninstall(funchook, 0);
			enabled = false;
		}
	}

private:
	bool enabled = false;
};
