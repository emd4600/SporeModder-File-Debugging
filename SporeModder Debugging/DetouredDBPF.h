#pragma once

#include <Spore\Internal.h>
#include <Spore\Detouring.h>

#include <Spore\Resource\DatabasePackedFile.h>

using namespace Resource;

class DetouredDBPF : public DatabasePackedFile
{
public:

	static long AttachDetour();


	/// We detour the Resource::DatabasePackedFile::GetFile() method, so we can check if it's going to use a
	/// file that we are debugging, and if it is, return a IPFRecord that reads from the disk instead of a .package.
#pragma DisableOptimization
	DetouredMethod(DetouredDBPF, GetFile, bool, DetouredGetFile,
		PARAMS(const ResourceKey &name, IPFRecord **pDst, int nDesiredAccess, int nCreateDisposition, bool arg_10, DBPFItem *pDstInfo),
		PARAMS(name, pDst, nDesiredAccess, nCreateDisposition, arg_10, pDstInfo));
};

