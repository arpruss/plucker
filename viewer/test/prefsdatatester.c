/*
 * $Id: prefsdatatester.c,v 1.5 2004/07/25 16:41:28 nordstrom Exp $
 *
 * Viewer - a part of Plucker, the free off-line HTML viewer for PalmOS
 * Copyright (c) 1998-2003, Mark Ian Lillywhite and Michael Nordstrom
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

#include "TestDB.h"
#include "prefsdata.h"
#include "const.h"
#include "util.h"

#include "prefsdatatester.h"

#define ViewerPrefID                5
#define ViewerOldPrefID             4

#define ViewerVersion               1


static Boolean      hasStoredPrefs;
static Preferences  originalPref;


/* Remove preferences from the Preferences database */
static void RemovePref
    (
    UInt16 prefID   /* preference to remove */
    )
{
    DmOpenRef           ref;
    DmSearchStateType   state;
    LocalID             dbID;
    UInt16              cardNo;
    Int16               index;

    DmGetNextDatabaseByTypeCreator( true, &state, 'sprf', 'psys', false,
        &cardNo, &dbID );
    ref = DmOpenDatabase( cardNo, dbID, dmModeReadWrite );
    if ( ref == NULL )
        return;

    index = DmFindResource( ref, ViewerAppID, prefID, NULL );
    if ( index != -1 )
        DmRemoveResource( ref, index );

     DmCloseDatabase( ref );
}



int PrefsDataSuiteSetUp( void )
{
    Int16   version;
    UInt16  prefSize;

    /* Store original preference structure */
    prefSize    = 0;
    version     = PrefGetAppPreferences( (UInt32) ViewerAppID,
                    (UInt16) ViewerPrefID, NULL, &prefSize, true);

    if ( version != noPreferenceFound && prefSize == sizeof( Preferences ) ) {
        PrefGetAppPreferences( (UInt32) ViewerAppID,
            (UInt16) ViewerPrefID, &originalPref, &prefSize, true);
        hasStoredPrefs = true;
    }
    else {
        hasStoredPrefs = false;
    }

    return 0;
}



int PrefsDataSuiteTearDown( void )
{
    /* Restore original preference structure */
    if ( hasStoredPrefs )
    {
        PrefSetAppPreferences( (UInt32) ViewerAppID, (UInt16) ViewerPrefID,
            (Int16) ViewerVersion, &originalPref, sizeof( Preferences ), true );
    }
    MemSet( &originalPref, sizeof( Preferences ), 0 );

    return 0;
}



int PrefsDataInitTest( void )
{
    RemovePref( ViewerPrefID );
    return 0;
}



int PrefsDataCleanupTest( void )
{
    return 0;
}



void TestPrefsDataInit( void )
{
    /* Change a known item to make sure it is updated */
    Prefs()->controlMode = MODE3;

    /* Since we don't have any preference structure calling ReadPrefs
       should create a new structure, i.e. the controlMode item should
       be set to its default value (MODE1) */
    ReadPrefs();

    ASSERT( Prefs()->controlMode == MODE1 );
}



void TestPrefsDataRead( void )
{
    Preferences testPref;

    /* Add known preference structure */
    MemSet( &testPref, sizeof( Preferences ), 0 );
    testPref.controlMode = MODE2;
    PrefSetAppPreferences( (UInt32) ViewerAppID, (UInt16) ViewerPrefID,
        (Int16) ViewerVersion, &testPref, sizeof( Preferences ), true );

    /* Read it back and check that the item has the correct value */
    ReadPrefs();
    ASSERT( Prefs()->controlMode == MODE2 );
}



void TestPrefsDataReadOld( void )
{
    OldPreferences  oldPref;
    Int16           version;
    UInt16          prefSize;

    /* Add known old preference structure */
    MemSet( &oldPref, sizeof( OldPreferences ), 0 );
    oldPref.rotate          = true;
    oldPref.controlMode     = MODE2;
    oldPref.useDateTime     = true;
    PrefSetAppPreferences( (UInt32) ViewerAppID, (UInt16) ViewerOldPrefID,
        (Int16) ViewerVersion, &oldPref, sizeof( OldPreferences ), true );

    /* When the old preference structure is found it should be converted
       to the new format and then the old structure should be removed */
    ReadPrefs();
    ASSERT( Prefs()->rotate );
    ASSERT( Prefs()->controlMode == MODE2 );
    ASSERT( Prefs()->useDateTime );

    /* Check that the old preference structure is removed */
    prefSize    = 0;
    version     = PrefGetAppPreferences( (UInt32) ViewerAppID,
                    (UInt16) ViewerOldPrefID, NULL, &prefSize, true);

    ASSERT( version == noPreferenceFound );
}



void TestPrefsDataWrite( void )
{
    Preferences testPref;
    Int16       version;
    UInt16      prefSize;

    /* Write known preference structure */
    MemSet( Prefs(), sizeof( Preferences ), 0 );
    Prefs()->controlMode = MODE2;
    WritePrefs();

    /* Check that the correct preference structure was stored */
    prefSize    = 0;
    version     = PrefGetAppPreferences( (UInt32) ViewerAppID,
                    (UInt16) ViewerPrefID, NULL, &prefSize, true);

    if ( version != noPreferenceFound && prefSize == sizeof( Preferences ) ) {
        PrefGetAppPreferences( (UInt32) ViewerAppID,
            (UInt16) ViewerPrefID, &testPref, &prefSize, true);
    }

    ASSERT( testPref.controlMode == MODE2 );
}



void InitializePrefsDataSuite( void )
{
    TestGroup* group;

    /* Preference Data Tester */
    group = add_test_group( "PrefsData", PrefsDataSuiteSetUp,
                PrefsDataSuiteTearDown, PrefsDataInitTest,
                PrefsDataCleanupTest );
    add_test_case( group, "Init", TestPrefsDataInit );
    add_test_case( group, "Read", TestPrefsDataRead );
    add_test_case( group, "Read (old)", TestPrefsDataReadOld );
    add_test_case( group, "Write", TestPrefsDataWrite );
}



void TerminatePrefsDataSuite( void )
{
}

