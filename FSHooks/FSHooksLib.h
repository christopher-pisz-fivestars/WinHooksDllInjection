#pragma once

//--------------------------------------------------------------------------------------------------
#ifdef FSHOOKS_LIBRARY_EXPORTS
#define FSHOOKS_API __declspec(dllexport)
#else
#define FSHOOKS_API __declspec(dllimport)
// Specify the import library to link in for the DLL
#pragma comment(lib, "FSHooks.lib")
#endif
