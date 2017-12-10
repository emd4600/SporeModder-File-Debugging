#include "stdafx.h"
#include "DetouredResourceManager.h"
#include "Debugging.h"

DetourDeclaration(DetouredResourceManager::GetResource);

long DetouredResourceManager::AttachDetour()
{
	SetDetourAddress(GetResource, GetMethodAddress(cResourceManager, GetResource));

	return AttachDetourFunction(GetResource, DetouredGetResource);
}

bool DetouredResourceManager::DetouredGetResource(
	const ResourceKey &name, ResourceObject **ppDst, int arg_8, DBPF * pDBPF, IResourceFactory *pFactory, const ResourceKey * pCacheName) {

	DebugInformation* pDebugInformation = nullptr;

	// We only want to do the debugging thing if we are going to return a resource
	// Therefore, if no dst for the resource is given, we just call the original method.
	if (ppDst == nullptr) return CallOriginal(GetResource, name, ppDst, arg_8, pDBPF, pFactory, pCacheName);


	// For now we assume the given name is the real one, if it's necessary it will get replaced in GetRealFileKey().
	ResourceKey realKey = name;

	// If no specific DBPF has been given, we must get it (this is done in the original method as well).
	if (pDBPF == nullptr)
	{
		if (pCacheName == nullptr) pDBPF = this->GetRealFileKey(name, &realKey);
		else pDBPF = this->GetDBPF(name);
	}

	// If the requested file exists in any package
	if (pDBPF != nullptr)
	{
		// Do we have debug information of this package? If so, do we have the original path of this file?
		if (Debugging::Get().GetDebugInformation(pDBPF, &pDebugInformation)
			&& pDebugInformation->GetFilePath(realKey))
		{
			// ReadResource does almost the same as GetResource, but it does not read from the cache,
			// which is what we are interested in.
			// Notice that we have done all this way because we are okay with Spore using the cache
			// in every other file; we only want to disable it for the files being debugged.
			return this->ReadResource(name, ppDst, arg_8, pDBPF, pFactory, pCacheName);

		}
		else
		{
			// If debugging does not apply to this file, do whatever Spore wants to do.
			return CallOriginal(GetResource, name, ppDst, arg_8, pDBPF, pFactory, pCacheName);
		}

	}
	else
	{
		// If the file doesn't exist we call the original method, just in case in needs to do something.
		return CallOriginal(GetResource, name, ppDst, arg_8, pDBPF, pFactory, pCacheName);
	}
}
