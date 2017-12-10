#pragma once

#include <Spore\Resource\DatabasePackedFile.h>
#include <Spore\Hash.h>

#include <EASTL\intrusive_ptr.h>
#include <EASTL\map.h>

#include "DebugInformation.h"

using namespace eastl;

/// This class will hold the debug information of all the packages that use the SporeModder
/// file debugging feature.
class Debugging
{
public:
	Debugging();
	~Debugging();

	/// Gets the debug information for the given DBPF.
	/// @param pDBPF The .package file to analize.
	/// @param[Out] pDst [Optional] Here the pointer to the package debug information will be written.
	/// @returns true if the DBPF has debug information, false otherwise.
	bool GetDebugInformation(const Resource::DBPF* pDBPF, DebugInformation** pDst = nullptr) const;

	///
	/// Detours all the functions related with file debugging.
	///
	static long AttachDetour();

	///
	/// A ModAPI Init function that detects all the .package files that must be debugged.
	///
	static bool Initialize();

	/// There's only one instance of this class active at a time; this method returns it.
	static Debugging& Get();

protected:
	void DetectDBPFs();

protected:
	/// Maps every package to the debug information
	map<const Resource::DBPF*, DebugInformation*> mPackageMap;


private:
	static Debugging sMain;
};

inline Debugging& Debugging::Get()
{
	return sMain;
}
