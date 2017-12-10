#include "stdafx.h"
#include "DetouredDBPF.h"

#include "Debugging.h"
#include "FilePFRecord.h"

#include <Spore\App\ICheatManager.h>
#include <Spore\Resource\cResourceManager.h>

DetourDeclaration(DetouredDBPF::GetFile);

long DetouredDBPF::AttachDetour()
{
	SetDetourAddress(GetFile, GetMethodAddress(DatabasePackedFile, GetFile));

	return AttachDetourFunction(GetFile, DetouredGetFile);
}

bool DetouredDBPF::DetouredGetFile(
	const ResourceKey &name, IPFRecord **ppDst, int nDesiredAccess, int nCreateDisposition, bool arg_10, DBPFItem *pDstInfo)
{

	DebugInformation* pDebugInformation = nullptr;
	string16 filePath;

	if (nDesiredAccess == kAccessFlagRead									// We only accept reading
		&& ppDst != nullptr													// No point on doing this if we are not going to get the file
		&& Debugging::Get().GetDebugInformation(this, &pDebugInformation)	// If we have debug information
		&& pDebugInformation->GetFilePath(name, &filePath))					// If this file has a path to debug
	{
		// This piece here prints a message in the console every time a debugged file is accessed.
		// In the release version I don't want to fill the console with garbage, but you can uncomment this if you want.
		//TODO: Create a cheat that enables this?
		/*App::ICheatManager* cm = App::ICheatManager::Get();
		if (cm != nullptr)
		{
			wstring dstName;
			IResourceManager::Get()->GetFileName(name, dstName);
			ArgScript::PrintF(cm->GetArgScript(), "File %ls accessed.", dstName);
		}*/

		// Until we say the opposite, this method hasn't succeed.
		bool result = false;

		FilePFRecord* pRecord = nullptr;

		// We lock the mutex (related with multithreading) before accessing the DBPF index.
		mIndexMutex.Lock(Mutex::kDefaultWait);

		// Get the DBPFItem info (we are not going to use it, but the caller might be requesting it).
		DBPFItem* info = mpIndex->GetFileInfo(name);
		if (info != nullptr)
		{
			// If the caller wants, give the DBPFItem info.
			if (pDstInfo != nullptr)
			{
				*pDstInfo = *info;
			}

			// The default behaviour for the create disposition (and remember that we ensured
			// the caller only wanted to read the file) is to only open the file if it exists.
			if (nCreateDisposition == kCDDefault)
			{
				nCreateDisposition = kCDOpenExisting;
			}

			// I'm not sure, but I think field_2E8 contains all the IPFRecords that are currently in use.
			// So first we check if there's one for the requested file.
			auto it = field_2E8.find(name);
			if (it != field_2E8.end())
			{
				//WARNING: This might not be the best idea. I think I copied it from what Spore does; anyways, it works.
				// If the current IPFRecord is our special one, we create a new one (why? I don't remember...).
				// If the current IPFRecord is not our type (for some reason) pRecord will be nullptr and nothing will
				// happen; this should never happen though.
				if (it->second->GetType() == FilePFRecord::kType)
				{
					pRecord = new (mpAllocator, "FilePFRecord") FilePFRecord((FilePFRecord*)it->second, name, this);
				}
			}
			else
			{
				// The cResourceManager class uses a special object for allocating memory, mpAllocator.
				// We use 'new' this way to use the special features of the ResourceManager memory allocator.
				pRecord = new (mpAllocator, "FilePFRecord") FilePFRecord(*info, name, this);
			}

			if (pRecord != nullptr)
			{
				// Our IPFRecord needs a path to a file in the disk, and we have it from the debug information!
				pRecord->SetPath(filePath.c_str());

				// We need to let our IPFRecord that it exists.
				// (not really, I don't know why we need this, but we will have memory leaks if we don't do this).
				pRecord->AddRef();

				// If the .package file is opened for writing...
				// Again, this is copied from Spore code.
				if (mnFileAccess & kAccessFlagWrite)
				{
					field_2E8[name] = pRecord;
				}
			}

			// Did the caller want the IPFRecord (we checked this before, so yes it does)?
			// Then give it.
			if (ppDst != nullptr)
			{
				*ppDst = pRecord;
			}

		}

		// We are done with accessing the index, so we unlock its mutex.
		mIndexMutex.Unlock();

		if (pRecord != nullptr)
		{
			// We need to let our IPFRecord that it exists (again?).
			// (not really, I don't know why we need this, but we will have memory leaks if we don't do this).
			pRecord->AddRef();

			// Everything went fine (we have an IPFRecord) so return true.
			result = true;
		}

		return result;
	}
	else
	{
		// If none of the conditions above are met, let Spore do its things.
		return CallOriginal(GetFile, name, ppDst, nDesiredAccess, nCreateDisposition, arg_10, pDstInfo);
	}
}
