{********************************************************************}
{                                                                    }
{           Program Name:     Strings Unit for Conduit.dpr           }
{           Source Language:  Delphi 3                               }
{           Author:           Dirk Heiser <conduit@Dirk-Heiser.de>   }
{                                                                    }
{********************************************************************}
{  This unit is part of the Plucker conduit for Win32, see in        }
{  Conduit.dpr for more Infos                                        }
{                                                                    }
{********************************************************************}
UNIT ConstStrings;

INTERFACE

USES PalmConst;

CONST
 Empty              = '';

 DBName             = 'PluckerDB';
 DBType             = 'Data';
 DBCreator          = 'Plkr';
 DBVersion          = '1';
 RecMax             = mydmMaxRecSize;
 CacheDir           = 'cache\';
 DBFileName         = 'PluckerDB.pdb';
 Verbose            = '1';
 DBReservedLength   = 0;
 DBNameSubstitute   = '_';
 Record_attrib      = $0000;
 
 DBMinRecSize       = 8;
 DBCTPOS            = 8;
 DBMinIRecSize      = 6;
 Index_Record       = 1;
 Category_Record_ID = 3;
 Category_Record_CT = 9;
 RecName            = 'DATA';

 PythonNullTime     = 25569;

 PQASignature       = 'lnch';
 PQAhdrVersion      = 3;
 PQAencVersion      = 0;
 BIGICON            = 1;
 SMALICON           = 2;
 VersionStr         = '%s.0';
 BigIconName        = '%s_b.tbmp';
 SmallIconName      = '%s_s.tbmp';
 STR_NonIcon        = '<none>';

 ValidChars         = ['0'..'9'];
 FileMask           = '*.';
 PathSep            = '\';
 
 Switchcount        = 16;
 Switches           : ARRAY [ 1..Switchcount ] OF STRING = (
  '-user=', '-dir=', '-cache=', '-id=', '-dbname=', '-fname=', '-fix', '-dbversion=', '-install=', '-verbose=', '-copyprev', '-backup', '-launchable', '-icon', '-icon=', '-category='
                      );

 CompressionTypes   : ARRAY [ 0..2 ] OF STRING = (
  'UNKNOWN', 'DOC', 'ZLib'
                      );

 RESERVED_NAMES     : ARRAY [ -2..4 ] OF STRING = (
  'UNKNOWN', 'Index Record', 'home.html', 'external bookmarks', 'URL handling', 'Category Record', 'Metadata Record'
                      );

 CT                 : ARRAY [ -2..10 ] OF STRING = (
  'UNKNOWN', 'none', 'PHTML', 'PHTML_COMPRESSED', 'TBMP', 'TBMP_COMPRESSED', 'MAILTO', 'LINK_INDEX', 'LINKS', 'LINKS_COMPRESSED', 'BOOKMARKS', 'CATEGORY', 'METADATA'
                      );

 S                  = '          ';
 L                  = #13#10;
 
 MSG_Welcome        = L + S +
  'ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป' + L + S +
  'บ          Plucker conduit for Win32 Version 1.72          บ' + L + S +
  'บฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤบ' + L + S +
  'บ             (c) 1999-2002 by Dirk Heiser                 บ' + L + S +
  'บ  ---o-o---              <conduit@Dirk-Heiser.de>         บ' + L + S +
  'บ  ---o--o--                                               บ' + L + S +
  'บ  ---o-o---  using TPalmUserData Class by Art Dahm        บ' + L + S +
  'บ  ---o-----                                               บ' + L + S +
  'บ  ---o-----  Distributable under the GNU General Public   บ' + L + S +
  'บ             License Version 2 or newer.                  บ' + L + S +
  'ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ' + L + L;
 
 MSG_Usage          =
  'Usage: conduit.exe -dir=<Plucker DIR> [-user=<User Name>] [-cache=<Cache Dir>]' + L +
  '                   [-id=<Creator ID>] [-dbversion=<DB Version Nr>]' + L +
  '                   [-dbname=<DB Name>] [-fname=<Filename>] [-fix]' + L +
  '                   [-install=<Path&Filename>] [-verbose=<Verbose Level>]' + L +
  '                   [-copyprev] [-backup] [-launchable] [-icon[=<IconFile>]]' + L +
  '                   [-category=<Category Name>]' + L + L +
  '   -dir        : Dir where your Plucker Data are stored' + L +
  '   -user       : Pilot user Name, if set DB will installed for Hotsync.' + L +
  '                 Sepperate more users with ";".' + L +
  '   -cache      : Dirname for you cache Dir. Default "' + CacheDir + '"' + L +
  '   -id         : Creator ID for the Database. Default "' + DBCreator + '"' + L +
  '   -dbversion  : Versions number for the Database. Default "' + DBVersion + '"' + L +
  '   -dbname     : Database Name. Dafault "' + DBName + '"' + L +
  '   -fname      : Filename for the Database. Default "' + DBFileName + '"' + L +
  '   -fix        : 0x0D0A Sequenses are converted to 0x0A' + L +
  '   -install    : Install a Pilot File. -User must be set, -dir not' + L +
  '   -verbose    : Set the Verbose Level. Default are ' + Verbose + L +
  '                 0 = Verbosity level 0 is silent except for errors' + L +
  '                 1 = Verbosity level 1 gives progress status' + L +
  '                 2 = Verbosity level 2 is used for debugging' + L +
  '   -category   : Preset the Category the DB belong to, enter the cat, Name' + L +
  '   -copyprev   : Set the CopyPrevention Flag' + L +
  '   -backup     : Set the Backup Flag' + L +
  '   -launchable : Set the Launchable Flag' + L +
  '   -icon       : Add an Icon to the DB.' + L +
  '               : You can specify the Path&Filename to an Tbmp Bitmap.' + L +
  '               : E.g. -icon=myicon Points to a Big Icon called "myicon_b.tbmp"' + L +
  '               : and a Small Icon called "myicon_s.tbmp"' + L;
 
 MSG_Dir            = 'Plucker Data Directory: %s';
 MSG_User           = 'Datababse is for User: %s';
 MSG_Read_File      = '  %s: Reading File "%s" [Record UID %s Size %s Bytes]';
 MSG_Reading        = 'Reading: %s';
 MSG_DBType         = 'Database compression type: %s';
 MSG_Installing     = 'Installing "%s" for: "%s"';
 MSG_Done           = ' --> Done.';
 MSG_FileInfo       = '        Data Type: %s       Contens Type: %s';
 MSG_Error_FileN    = ' --> Fail.' + L + 'Wrong Filename: %s';
 MSG_Error_FileR    = ' --> Fail.' + L + 'Error Reading File: %s';
 MSG_Error_Dir      = 'No Files Found';
 MSG_Error_Save     = ' --> Fail.' + L + 'Error Saving "%s"';
 MSG_Error_User     = 'Error Installing Database for "%s"';
 MSG_Error_UserNF   = ' --> Fail.' + L + 'User "%s" not Exist';
 MSG_Write_DB       = 'Writing "%s" (%s kB)';
 MSG_WriteDB_Info   = '           ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออป' + L +
                      '           บ Documents   ณ %s kB ณ %s Byte ณ %s%% บ' + L +
                      '           บ Images      ณ %s kB ณ %s Byte ณ %s%% บ' + L +
                      '           บ Mailto      ณ %s kB ณ %s Byte ณ %s%% บ' + L +
                      '           บ Links       ณ %s kB ณ %s Byte ณ %s%% บ' + L +
                      '           บ Bookmarks   ณ %s kB ณ %s Byte ณ %s%% บ' + L +
                      '           บ Unknown     ณ %s kB ณ %s Byte ณ %s%% บ' + L +
                      '           บ Header      ณ %s kB ณ %s Byte ณ %s%% บ' + L +
                      '           บฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤบ' + L +
                      '           บ Summary     ณ %s kB ณ %s Byte ณ %s%% บ' + L +
                      '           ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออผ';
 MSG_Error_Size     = ' --> Fail.' + L + 'Record to lage (max. is %s Bytes)';
 MSG_Error_FNF      = 'Error: File "%s" not Found';
 MSG_Error_Index    = 'Error: Index_Record (Record 1) not Found';
 MSG_Error_Record   = L + 'Error: Record too small (%s bytes)';
 MSG_Error_IRecord  = L + 'Error: Index Record too small (%s bytes)';
 MSG_Error_IRecordS = L + 'Error: Index Record has the wrong Size';
 MSG_Error_UID      = L + 'Error: UID different to Filename (File: %s)';
 MSG_Error_FindFile = 'Error: Canot find a Free UID';
 MSG_Rem_Cat        = 'INFO: Category Info found, Removing';
 MSG_New_Cat        = 'Set Category to "%s"';

IMPLEMENTATION

END.
