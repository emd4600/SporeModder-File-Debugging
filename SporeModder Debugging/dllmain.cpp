// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include <ModAPI\MainUtilities.h>
#include <Spore\Detouring.h>

#include "Debugging.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	// Variables are declared outside of the switch to avoid compiler warnings.
	long error;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		// This line is always necessary
		ModAPI::ModAPIUtils::InitModAPI();

		/* Insert code here */

		/* 
		Note: You cannot use any Spore function inside this method. 
		If you want to add any initialization methods, do it with:

		bool myFunction() {...}

		And then, inside the DllMain function:

		ModAPI::ModAPIUtils::AddInitFunction(&myFunction);

		Things you can do in the DllMain method:
		 - Add init functions
		 - Add UI event listeners
		 - Detour functions
		*/

		// We have a method that must be run when the game is initialized.
		ModAPI::ModAPIUtils::AddInitFunction(Debugging::Initialize);

		/* -- DETOURING -- */
		PrepareDetours(hModule);

		Debugging::AttachDetour();

		error = SendDetours();
		/* --------------- */

		if (error != NO_ERROR)
		{
			MessageBoxA(NULL, "Error detouring. Check your version, it might not support the ModAPI.", "Error", MB_OK);
		}


	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

