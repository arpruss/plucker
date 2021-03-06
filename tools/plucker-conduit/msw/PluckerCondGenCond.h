/*****************************************************************************
 *
 * Generic Conduit Export Functions Header File
 *
 ****************************************************************************/


#ifndef  __GENERIC_MAIN_CONDUIT__
#define  __GENERIC_MAIN_CONDUIT__
#include <condapi.h>

#define GENERIC_CONDUIT_VERSION 0x00000102

#ifndef ExportFunc
#ifdef _68K_
#define ExportFunc
#else
#define ExportFunc __declspec( dllexport )
#endif
#endif

extern "C" {

typedef  long (*PROGRESSFN) (char*);
ExportFunc long OpenConduit(PROGRESSFN, CSyncProperties&);
ExportFunc long GetConduitName(char*,WORD);
ExportFunc DWORD GetConduitVersion();
ExportFunc long ConfigureConduit(CSyncPreference& pref);
ExportFunc long GetConduitInfo(ConduitInfoEnum infoType, void *pInArgs, void *pOut, DWORD *dwOutSize);

};


#endif


