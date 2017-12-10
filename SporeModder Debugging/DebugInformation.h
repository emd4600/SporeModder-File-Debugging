#pragma once

#include <EASTL\string.h>
#include <EASTL\hash_map.h>

#include <Spore\ResourceKey.h>
#include <Spore\App\PropertyList.h>

using namespace eastl;

/// This class holds all the debug information in a .package file packed using the
/// SporeModder file debugging feature. It contains a map that assigns the real filepath 
/// (such as "editor_rigblocks~\model.prop.xml") to every ResourceKey in the package.
/// It also contains the file path to the folder where the project files are stored.
///
class DebugInformation
{
public:
	DebugInformation();
	~DebugInformation();

	/// Gets the path to the file in the disk that will be used for the specified ResourceKey.
	/// @param key The ResourceKey of the file.
	/// @param[Out] pDst [Optional] The pointer where the path will be written.
	/// @returns True if this ResourceKey has a path assigned in this object, false otherwise.
	bool GetFilePath(const ResourceKey& key, string16* pDst = nullptr) const;

	/// Reads the .prop file with the given instance and group ID and loads all the debug
	/// information.
	/// @returns True if the file existed and was loaded properly, false otherwise.
	bool Read(uint32_t instanceID, uint32_t groupID);

protected:
	/// The path to the disk folder where the project files are stored.
	string16 mDebugPath;
	/// Maps the real filename (such as "editor_rigblocks~\model.prop.xml") to its ResourceKey.
	hash_map<ResourceKey, string16*> mFileNames;
	/// Debug information is hold in a .prop file, this member points to it.
	App::PropertyList* mpPropList;
};