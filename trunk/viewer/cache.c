/*
 * $Id: cache.c,v 1.26 2007/02/12 14:08:22 prussar Exp $
 *
 * Viewer - a part of Plucker, the free off-line HTML viewer for PalmOS
 * Copyright (c) 1998-2002, Mark Ian Lillywhite and Michael Nordstrom
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "const.h"
#include "debug.h"
#include "util.h"
#include "list.h"
#include "os.h"
#include "ramfile.h"

#include "cache.h"


/***********************************************************************
 *
 *      Internal Constants
 *
 ***********************************************************************/
#define FTRNUM_START           100
#define MINIMUM_FREE_SPACE     262144
#define MAX_DATA_SIZE          64000

#ifndef memHeapFlagReadOnly
# define memHeapFlagReadOnly    0x0001
#endif

static const Char cacheDBName[] = "Plkr-Cache";

typedef struct {
    CacheType   type;
    Int32       refID;
    UInt16      size;
    MemHandle   recordHandle;
    void*       dataPtr;
    void        (*CleanerCallback)( void** dataPtr );
} CacheNode;


/***********************************************************************
 *
 *      Local Functions
 *
 ***********************************************************************/
static Err GetCacheNode( CacheType type, Int32 refID, CacheNode** refNode )
    CACHE_SECTION;
static void RemoveCacheNodeRecord( CacheNode* node )
    CACHE_SECTION;
static void RemoveCacheNode( CacheNode* node )
    CACHE_SECTION;
static UInt32 StorageHeapFree ( void )
    CACHE_SECTION;


/***********************************************************************
 *
 *      Private Variables
 *
 ***********************************************************************/
static LinkedList   cache = NULL;
static DmOpenRef    cacheDB;


/* Check if this refID has a cache node */
Boolean HasCacheNode
    (
    CacheType   type,
    Int32       refID   /* reference ID to search for */
    )
{
    CacheNode* refNode;

    return GetCacheNode( type, refID, &refNode ) == errNone;
}



/* check if this reference ID is cached, return it by the object pointer */
static Err GetCacheNode
    (
    CacheType   type,
    Int32       refID,  /* reference ID to search for */
    CacheNode** refNode /* return the reference node */
    )
{
    CacheNode* node;

    node = ListFirst( cache );
    while ( node != NULL )  {
        if ( node->type == type && node->refID == refID )
            break;
        node = ListNext( cache, node );
    }
    *refNode = node;

    if ( node == NULL )
        return errCacheNotFound;
    else
        return errNone;
}



/* Remove a record from the cache database identifying by node */
static void RemoveCacheNodeRecord
    (
    CacheNode* node
    )
{
    UInt16 index;

    if ( node->recordHandle == NULL )
        return;
        
    index = DmSearchRecord( node->recordHandle, &cacheDB );

    if ( index != 0xFFFF ) { /* ARP:This shouldn't be needed, but is! */
        CloseRecord( node->recordHandle, true );
        RemoveRecord( cacheDB, index );
    }
    node->recordHandle = NULL;
}



/* remove node from cache */
static void RemoveCacheNode
    (
    CacheNode* node
    )
{
    if ( node != NULL ) {
        if ( node->CleanerCallback != NULL ) {
            node->CleanerCallback( &(node->dataPtr) );
        }
        else {
            RemoveCacheNodeRecord( node );
        }
        ListTakeOut( cache, node );
        SafeMemPtrFree( node );
    }
}



/* recall a specific node out of cache by its reference ID */
MemPtr LoadFromCache
    (
    CacheType type,
    Int32     refID  /* reference ID to use */
    )
{
    Err         err;
    CacheNode*  node;

    MSG( _( "Loading from cache: refID %ld (type %d)\n", refID, type ) );

    /* Try to find this reference ID in our cache. If it's there,
       assign the memory pointer to its location */
    node    = NULL;
    err     = GetCacheNode( type, refID, &node );

    if ( err != errNone ) {
        MSG( _( "Not found\n" ) );
        return NULL;
    }

    MSG( _( "found list or ptr\n" ) );
    return node->dataPtr;
}



/* add pointer's address as a new entry into the cache */
Err AddPtrToCache
    (
    CacheType type,
    Int32     refID,    /* reference ID this data represents */
    void*     dataPtr,
    void*     cleanerCallbackFunc
    )
{
    CacheNode* node;

    while ( StorageHeapFree() < MINIMUM_FREE_SPACE && ! ListIsEmpty( cache ) )
        RemoveCacheNode( ListFirst( cache ) );

    if ( StorageHeapFree() < MINIMUM_FREE_SPACE )
        return errNone;

    node = SafeMemPtrNew( sizeof *node );
    MemSet( node, sizeof *node, 0 );

    node->type            = type;
    node->refID           = refID;
    node->dataPtr         = dataPtr;
    node->CleanerCallback = cleanerCallbackFunc;

    ListAppend( cache, node );

    MSG( _( "Adding into cache refID: %ld (type %d)\n", refID, type ) );
    return errNone;
}



/* Allocate space in the storage heap for cache-related data */
MemHandle AllocateCacheRecord
    (
    CacheType type,
    Int32     refID,
    UInt32    size
    )
{
    Err         err;
    CacheNode*  node;
    UInt16      index;

    node    = NULL;
    err     = GetCacheNode( type, refID, &node );

    if ( err != errNone )
        return NULL;

    if ( node->recordHandle != NULL ) {
        index = DmSearchRecord( node->recordHandle, &cacheDB );
        node->recordHandle = ResizeRecord( cacheDB, index, size );
    }
    else {
        index  = dmMaxRecordIndex;
        node->recordHandle = NewRecord( cacheDB, &index, size );
        if ( node->recordHandle == NULL ) {
            RemoveCacheNode( node );
            return NULL;
        }
    }

    return node->recordHandle;
}



/* Remove from the storage heap the cache-related data */
void RemoveCacheRecord
    (
    CacheType type,
    Int32     refID
    )
{
    CacheNode* node;
    Err        err;

    node = NULL;
    err  = GetCacheNode( type, refID, &node );

    if ( err == errNone )
        RemoveCacheNodeRecord( node );
}



/* Initialize the Cache. Needed to be called only when plucker starts */
Err InitializeCache( void )
{
    UInt16 cardNo;
    Err    err;

    /* create new cache */
    cacheDB = DmOpenDatabaseByTypeCreator( CacheDBType, ViewerAppID,
        dmModeReadWrite );
    if ( cacheDB != NULL )
        RemoveCache();

    cardNo = 0;
    err = DmCreateDatabase( cardNo, cacheDBName, ViewerAppID,
        CacheDBType, false );

    if ( err == errNone ) {
        cacheDB = DmOpenDatabaseByTypeCreator( CacheDBType, ViewerAppID,
            dmModeReadWrite );

        SafeSetRecyclable( cardNo, DmFindDatabase( cardNo, cacheDBName ) );

        err = DmGetLastErr();
        if ( err != errNone )
            RemoveCache();
        else
            cache = ListCreate();
    }
/*    DebugDatabaseSize( cacheDB ); */
    return err;
}



/* Clear the cache. Called either when Plucker is shutting down, or another
   document is loaded in memory, or a document is exited. */
void ResetCache( void )
{
    CacheNode* node;

    /* clear any data in the cache */
    node = ListFirst( cache );
    while ( node != NULL ) {
        CacheNode *next;

        next = ListNext( cache, node );
        RemoveCacheNode( node );
        node = next;
    }
}



/* Remove the cache. Called when Plucker is shutting down */
void RemoveCache( void )
{
    UInt16  cardNo;
    LocalID dbID;

    ResetCache();
    ListDelete( cache );
    cache = NULL;

    CloseDatabase( cacheDB );
    cacheDB = NULL;
    cardNo = 0;
    dbID   = DmFindDatabase( cardNo, cacheDBName );
    if ( dbID != 0 )
        DmDeleteDatabase( cardNo, dbID );
}


static UInt32 StorageHeapFree ( void )
{
    UInt16 numHeaps;
    UInt16 i;
/*    UInt32 value; */

/* Something like this code may be needed.  Still experimenting -- AR Pruss */
/*    if ( errNone == FtrGet( sysFtrCreator, sysFtrNumDmAutoBackup, &value ) &&
         value != 0 )
        DmSync(); */

    numHeaps = MemNumHeaps(0);

    for (i = 0; i < numHeaps; i++) {
        UInt16 id;

        id = MemHeapID(0, i);

        if ( ! MemHeapDynamic( id ) &&
             ! ( memHeapFlagReadOnly & MemHeapFlags( id ) ) ) {
            UInt32 freeBytes;
            UInt32 maxBytes;

            freeBytes = 0;

            MemHeapFreeBytes(id, &freeBytes, &maxBytes);

            MSG( _( "Cache storage heap: %ld free, %ld avail in one chunk\n",
                    freeBytes, maxBytes ) );

            return freeBytes;
        }
    }

    return 0;
}

