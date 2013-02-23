{********************************************************************}
{                                                                    }
{  Some Const used for Palm stuff                                    }
{                                                                    }
{********************************************************************}
UNIT PalmConst;

INTERFACE

CONST

    {********************************************************************}
    {                                                                    }
    {  Some Const used for Palm stuff, known from the Docs               }
    {                                                                    }
    {********************************************************************}

    mydmValidDBNameChars       = [' '..'~'];// dmDBNameLength Maximum length of a database’s name.
                                            //  Currently, this is 32 bytes, which include the
                                            //  null terminator. Note that database names must
                                            //  use only 7-bit ASCII characters (0x20 through
                                            //  0x7E).
    mydmMaxRecSize             = 65505;     // Maximum Size of a DB Record in Byte.
    mydmNullTime               =  1462;     // Delphi TDateTime Value to Give a Palm null Time.

    {********************************************************************}
    {                                                                    }
    {  The following are from "DataMgr.h" SDK 3.5                        }
    {                                                                    }
    {********************************************************************}

    //************************************************************
    //* Category equates
    //************************************************************
    dmRecAttrCategoryMask      =   $0F;     // mask for category #
    dmRecNumCategories         =    16;     // number of categories
    dmCategoryLength           =    16;     // 15 chars + 1 null terminator

    dmAllCategories            =   $ff;
    dmUnfiledCategory          =     0;

    dmMaxRecordIndex           = $ffff;

    // Record Attributes
    dmRecAttrDelete            =   $80;     // delete this record next sync
    dmRecAttrDirty             =   $40;     // archive this record next sync
    dmRecAttrBusy              =   $20;     // record currently in use
    dmRecAttrSecret            =   $10;     // "secret" record - password protected

    //************************************************************
    //* Database Header equates
    //************************************************************
    dmDBNameLength             =    32;     // 31 chars + 1 null terminator

    // Attributes of a Database
    dmHdrAttrResDB             = $0001;     // Resource database
    dmHdrAttrReadOnly          = $0002;     // Read Only database
    dmHdrAttrAppInfoDirty      = $0004;     // Set if Application Info block is dirty
                                            // Optionally supported by an App's conduit
    dmHdrAttrBackup            = $0008;     // Set if database should be backed up to PC if
                                            //  no app-specific synchronization conduit has
                                            //  been supplied.
    dmHdrAttrOKToInstallNewer  = $0010;     // This tells the backup conduit that it's OK
                                            //  for it to install a newer version of this database
                                            //  with a different name if the current database is
                                            //  open. This mechanism is used to update the 
                                            //  Graffiti Shortcuts database, for example. 
    dmHdrAttrResetAfterInstall = $0020;     // Device requires a reset after this database is 
                                            // installed.
    dmHdrAttrCopyPrevention    = $0040;     // This database should not be copied to 
    dmHdrAttrStream            = $0080;     // This database is used for file stream implementation.
    dmHdrAttrHidden            = $0100;     // This database should generally be hidden from view
                                            //  used to hide some apps from the main view of the
                                            //  launcher for example.
                                            // For data (non-resource) databases, this hides the record
                                            //  count within the launcher info screen.
    dmHdrAttrLaunchableData    = $0200;     // This data database (not applicable for executables)
                                            //  can be "launched" by passing it's name to it's owner
                                            //  app ('appl' database with same creator) using
                                            //  the sysAppLaunchCmdOpenNamedDB action code. 
    dmHdrAttrOpen              = $8000;     // Database not closed properly

    //************************************************************
    //* Unique ID equates
    //************************************************************
    dmRecordIDReservedRange    =     1;     // The range of upper bits in the database's
                                            //  uniqueIDSeed from 0 to this number are
                                            //  reserved and not randomly picked when a
                                            //  database is created.
    dmDefaultRecordsID         =     0;     // Records in a default database are copied
                                            //  with their uniqueIDSeeds set in this range.
    dmUnusedRecordID           =     0;     // Record ID not allowed on the device

IMPLEMENTATION

END.
