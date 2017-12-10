#include "stdafx.h"
#include "DebugInformation.h"

#include <Spore\App\Property.h>
#include <Spore\App\IPropManager.h>
#include <Spore\Hash.h>

DebugInformation::DebugInformation()
	: mDebugPath()
	, mFileNames()
{
}

DebugInformation::~DebugInformation()
{

}

bool DebugInformation::GetFilePath(const ResourceKey& key, string16* pDst) const
{
	auto it = mFileNames.find(key);

	// Is the file in the map?
	if (it != mFileNames.end())
	{
		// Did the caller ask for the path or just wanted to check if the file was in the map?
		if (pDst != nullptr)
		{
			// The path is just the path to the folder + the file name.
			*pDst = mDebugPath + *it->second;
		}
		return true;
	}
	else
	{
		// The file is not in the map.
		return false;
	}
}

bool DebugInformation::Read(uint32_t instanceID, uint32_t groupID)
{
	intrusive_ptr<App::PropertyList> pPropList;

	// If the .prop file exists, the code inside the if {} will be executed.
	if (App::GetPropManager()->GetPropertyList(instanceID, groupID, pPropList))
	{
		mpPropList = pPropList.get();

		// We declare the variables that store the values of the properties.
		size_t nFileCount;
		ResourceKey* pFileKeys;
		string16* pFileNames;

		// We need a <keys name="modFileKeys">... property; if it does not exist, we return false.
		if (!App::Property::GetArrayKey(mpPropList, Hash::FNV("modFileKeys"), nFileCount, pFileKeys)) return false;

		// We need a <string16s name="modFilePaths">... property; if it does not exist, we return false.
		if (!App::Property::GetArrayString16(mpPropList, Hash::FNV("modFilePaths"), nFileCount, pFileNames)) return false;

		// We need a <string16 name="modDebugPath">... property; if it does not exist, we return false.
		if (!App::Property::GetString16(mpPropList, Hash::FNV("modDebugPath"), mDebugPath)) return false;

		// We ensure the folder path ends with a '\', that way it's easier to get the path of a file (we will have to just join them).
		if (mDebugPath[mDebugPath.size() - 1] != '\\')
		{
			mDebugPath += '\\';
		}

		// Add every file in the properties into the map.
		for (size_t i = 0; i < nFileCount; i++)
		{
			mFileNames[pFileKeys[i]] = &pFileNames[i];
		}

		// The file existed and was successfully read, return true.
		return true;
	}
	else
	{
		// The .prop file does not exist, return false.
		return false;
	}
}
