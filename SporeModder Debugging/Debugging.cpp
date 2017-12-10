#include "stdafx.h"
#include "Debugging.h"
#include "DetouredDBPF.h"
#include "DetouredResourceManager.h"

#include <Spore\Hash.h>
#include <Spore\App\IPropManager.h>
#include <Spore\Resource\IResourceManager.h>
#include <Spore\Resource\DatabasePackedFile.h>
#include <Spore\Resource\PFIndexModifiable.h>

#include <EASTL\string.h>

using namespace IO;

Debugging Debugging::sMain;

Debugging::Debugging()
	: mPackageMap()
{

}
Debugging::~Debugging()
{
	try
	{
		for (auto it = mPackageMap.cbegin(); it != mPackageMap.cend(); ++it)
		{
			if (it->second != nullptr)
			{
				delete it->second;
			}
		}
	}
	catch (int) {};
}

long Debugging::AttachDetour()
{
	long result = 0;

	result |= DetouredDBPF::AttachDetour();
	result |= DetouredResourceManager::AttachDetour();

	return result;
}

bool Debugging::GetDebugInformation(const Resource::DBPF* pDBPF, DebugInformation** ppDst) const
{
	auto it = mPackageMap.find(pDBPF);
	if (it != mPackageMap.end())
	{
		if (ppDst != nullptr) {
			*ppDst = it->second;
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool Debugging::Initialize()
{
	// Inside our initialization function, we only need to detect the .package files.
	sMain.DetectDBPFs();

	return true;
}

void Debugging::DetectDBPFs()
{
	// On startup, we get a list of all .package files and check which ones we are debugging

	// The GetAllDBPFs method requires this type of list.
	cResourceManager::DBPFList dbpfList = cResourceManager::DBPFList(ICoreAllocatorAdapter(nullptr, IO::GetAllocator()));

	// We get all the package (that method returns the number of packages).
	size_t numDBPFs = IResourceManager::Get()->GetAllDBPFs(dbpfList);


	vector<uint32_t> propNames;

	uint32_t nGroupID = Hash::FNV("_SporeModder");

	// We get all the .prop files contained in the folder "_SporeModder".
	if (App::GetPropManager()->GetAllListIDs(nGroupID, propNames))
	{
		for (size_t i = 0; i < propNames.size(); i++)
		{
			// For every .prop file in the "_SporeModder" folder, create a new DebugInformation and read it.
			DebugInformation* pInfo = new DebugInformation();
			if (pInfo->Read(propNames[i], nGroupID))
			{
				DBPF* pDBPF = IResourceManager::Get()->GetDBPF(ResourceKey(propNames[i], App::PropertyList::kPropTypeID, nGroupID));
				// We assign the debug information to this package.
				mPackageMap[pDBPF] = pInfo;
			}
		}
	}
}
