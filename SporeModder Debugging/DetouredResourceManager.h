#pragma once

#include <Spore\Resource\cResourceManager.h>
#include <Spore\Detouring.h>

using namespace Resource;

class DetouredResourceManager : public cResourceManager
{
public:

	static long AttachDetour();

	/// The Resource::IResourceManager::GetResource() method tries to get files from the cache first.
	/// We want to disable that feature for the files that are being debugged, so they reflect the changes
	/// of the files in the disk.
#pragma DisableOptimization
	DetouredMethod(DetouredResourceManager, GetResource, bool, DetouredGetResource,
		PARAMS(const ResourceKey &name, ResourceObject **ppDst, int arg_8, DBPF *pDBPF, IResourceFactory *pFactory, const ResourceKey *pCacheName),
		PARAMS(name, ppDst, arg_8, pDBPF, pFactory, pCacheName));
};

