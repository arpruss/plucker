/*
 * $Id: data.c,v 1.1 2003/01/07 23:17:21 adamm Exp $
 *
 * Cleaner - a part of Plucker, the free off-line HTML viewer for PalmOS
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
 *
 * Originally based on code by Andrew Howlett from his Tex2Hex demo. (Thanks!)
 */

#include "data.h"


/***********************************************************************
 *
 *      Internal Types
 *
 ***********************************************************************/
typedef struct DBNode *DBNodePtr;

struct DBNode {
    UInt32    creator;
    UInt32    type;
    UInt16    cardNo;
    LocalID   dbID;
    DBNodePtr next;
};


/***********************************************************************
 *
 *      Internal Constants
 *
 ***********************************************************************/
#define NOT_FOUND 0xFFFF


/***********************************************************************
 *
 *      Private Variables
 *
 ***********************************************************************/
static DBNodePtr dbHead = NULL;



/* Find the databases represented by this creator/type, and calculate sizes */
void EnumerateData
    (
    UInt32  creator,
    UInt32  type,
    UInt32* totalRecords,
    UInt32* totalSize
    )
{
    Err              err;
    DmSearchStatePtr stateInfoP;
    Boolean          newSearch;

    stateInfoP    = MemPtrNew( sizeof( DmSearchStateType ) );
    newSearch     = true;
    *totalRecords = 0;
    *totalSize    = 0;

    do {
        UInt16    cardNo;
        LocalID   dbID;
        UInt32    records;
        UInt32    size;

        err = DmGetNextDatabaseByTypeCreator( newSearch, stateInfoP,
            type, creator, false, &cardNo, &dbID );
        newSearch = false;

        size    = 0;
        records = 0;
        if ( err == errNone ) {
            DBNodePtr newDBNode;

            /* Append the database information into the linked list */
            newDBNode = MemPtrNew( sizeof( struct DBNode ) );
            newDBNode->creator = creator;
            newDBNode->type    = type;
            newDBNode->cardNo  = cardNo;
            newDBNode->dbID    = dbID;
            newDBNode->next    = dbHead;
            dbHead             = newDBNode;

            /* If we find a database that is not owned by plucker directly,
               crack it open and count the resources with our name on it */
            if ( creator != ViewerAppID ) {
                DmOpenRef ref;
                UInt16    index;
                MemHandle data;

                ref = DmOpenDatabase( cardNo, dbID, dmModeReadOnly );

                records = 0;
                index = DmFindResourceType( ref, ViewerAppID, records++ );
                while ( index != NOT_FOUND ) {
                    if ( ( data = DmGetResourceIndex( ref, index ) ) != NULL ) {
                        size += MemHandleSize( data );
                        DmReleaseResource( data );
                    }
                    index = DmFindResourceType( ref, ViewerAppID, records++ );
                }
                /* record count is actually one level too high */
                records--;

                DmCloseDatabase( ref );
            }
            else {
                DmDatabaseSize( cardNo, dbID, &records, &size, NULL );
            }
            *totalRecords += records;
            *totalSize    += size;
        }
    } while ( err == errNone );

    MemPtrFree( stateInfoP );
}



/* Delete the data associated with this creator/type */
Err ClearData
    (
    UInt32 creator,
    UInt32 type
    )
{
    Err       err;
    DBNodePtr dbNode;

    err = dmErrCantFind;

    for ( dbNode = dbHead; dbNode; dbNode = dbNode->next ) {
        if ( ! ( dbNode->creator == creator && dbNode->type == type ) )
            continue;

        /* Obviously since we don't want to delete any databases that we don't
           own, we'll have to open up the requested db to look for resources
           labelled with our own creatorID */
        if ( creator != ViewerAppID ) {
            DmOpenRef ref;
            UInt16    index;
            UInt32    typeIndex;

            ref = DmOpenDatabase( dbNode->cardNo, dbNode->dbID,
                dmModeReadWrite );

            typeIndex = 0;
            index = DmFindResourceType( ref, ViewerAppID, typeIndex++ );
            while ( index != NOT_FOUND ) {
                err = DmRemoveResource( ref, index );
                index = DmFindResourceType( ref, ViewerAppID, typeIndex++ );
            }

            DmCloseDatabase( ref );
        } else {
            err = DmDeleteDatabase( dbNode->cardNo, dbNode->dbID );
        }
    }

    return err;
}



/* Free the memory allocated by the linked list */
void FreeDataList( void )
{
    DBNodePtr dbNode, nextDBNode;

    for ( dbNode = dbHead; dbNode; dbNode = nextDBNode ) {
        nextDBNode = dbNode->next;
        MemPtrFree( dbNode );
        dbNode = NULL;
        dbHead = nextDBNode;
    }
}
