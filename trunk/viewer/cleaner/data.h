/* $Id: data.h,v 1.2 2003/08/01 11:57:17 prussar Exp $ */

#ifndef CLEANER_DATA_H
#define CLEANER_DATA_H

#include "cleaner.h"

extern void EnumerateData( UInt32 creator, UInt32 type, UInt32* totalRecords,
    UInt32* totalSize );
extern Err ClearData( UInt32 creator, UInt32 type );
extern void FreeDataList( void );

#endif
