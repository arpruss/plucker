{********************************************************************}
{                                                                    }
{           Program Name:     Plucker conduit for Win32 Version 1.72 }
{           Source Language:  Delphi 3                               }
{           Author:           Dirk Heiser <conduit@Dirk-Heiser.de>   }
{                                                                    }
{********************************************************************}
{  If you find Spelling Errors or Bugs feel free to report this to   }
{  me.                                                               }
{                                                                    }
{  Distributable under the GNU General Public License                }
{  Version 2 or newer.                                               }
{                                                                    }
{  You need TPalmUserData Class for Delphi:                          }
{  Copyright © 1997 by Art Dahm Web: http://palm.dahm.com            }
{                                                                    }
{  Description:                                                      }
{   Conduit for Plucker running under Win32.                         }
{                                                                    }
{ Usage:                                                             }
{  -dir       : Dir where your Plucker Data are stored               }
{  -user      : Pilot user Name, if set DB will installed for Hotsync}
{  -cache     : Dirname for you cache Dir. Default "Cache\"          }
{  -id        : Creator ID for the Database. Default "Plkr"          }
{  -dbversion : Versions number for the Database. Default "7"        }
{  -dbname    : Database Name. Dafault "PluckerDB"                   }
{  -fname     : Filename for the Database. Default "PluckerDB.pdb"   }
{  -fix       : 0x0D0A Sequenses are converted to 0x0A               }
{  -install   : Install a Pilot File. -User must be set, -dir not    }
{  -verbose   : Set the Verbose Level. Default are 1                 }
{               0 = Verbosity level 0 is silent except for errors    }
{               1 = Verbosity level 1 gives progress status          }
{               2 = Verbosity level 2 is used for debugging          }
{  -copyprev  : Set the CopyPrevention Flag                          }
{  -backup    : Set the Backup Flag                                  }
{  -launchable: Set the Launchable Flag                              }
{                                                                    }
{  Credits:                                                          }
{   Art Dahm for the TPalmUserData Class for Delphi.                 }
{   See the Files in .\PalmUserData\ for more infos                  }
{                                                                    }
{********************************************************************}
{                                                                    }
{  Revision History                                                  }
{  v#    ,Date       ,Author Name   ,Description                     }
{  1.0   ,10/21/1999 ,Dirk Heiser   ,First Version                   }
{  1.1   ,11/03/1999 ,Dirk Heiser   ,Add new Command Line Parameters }
{  1.2   ,11/08/1999 ,Dirk Heiser   ,Add DBVersion Command Line Para.}
{  1.3   ,11/09/1999 ,Dirk Heiser   ,Add -install Command Line Param.}
{  1.4   ,12/18/1999 ,Dirk Heiser   ,Add Max Size Check.             }
{                                    Set Default DB Version to "6"   }
{                                    Change -nofix param. to -fix    }
{  1.41  ,12/23/1999 ,Dirk Heiser   ,Fixed a little Bug on the max.  }
{                                    Rec Size Calculation            }
{  1.42  ,03/04/2000 ,Dirk Heiser   ,Recompiled with a new Version of}
{                                    the PalmUserData Class          }
{                                    Set the Default DBVersion to "7"}
{  1.5   ,05/10/2000 ,Dirk Heiser    Added Verbose Switch            }
{                                    Switch DB Attr CopyPrevention   }
{                                    Switch DB Attrib Backup         }
{                                    Switch DB Attrib Launchable     }
{                                    Mod Time are set to Create Time }
{                                    Records are sorted by Number    }
{                                    Writing icons in PQA App info   }
{  1.51  ,06/05/2000 ,Dirk Heiser    Check Presence of Rec 1 & 2     }
{                                    Check Record size >=2           }
{                                    -icon now public                }
{                                    Debug view change for new format}
{                                    PQA Appinfo Block versionformat }
{                                    RecName enhanced (Debug view)   }
{                                    Set DB Version to 8             }
{  1.52  ,06/06/2000 ,Dirk Heiser    Correct the About screen        }
{  1.53  ,06/07/2000 ,Dirk Heiser    Set the UID to Zero             }
{  1.54  ,06/28/2000 ,Dirk Heiser    Set DB Version to 9             }
{                                    Max Rec Size to 64KB            }
{                                    Max DBName Length to 26         }
{                                    Add PalmConst.pas for Palm const}
{  1.55  ,07/03/2000 ,Dirk Heiser    Changes for the new DB Format   }
{  1.56  ,07/05/2000 ,Dirk Heiser    Changes for the new DB Format   }
{                                    More Detiled DB Statistik       }
{  1.6   ,08/01/2000 ,Dirk Heiser    Changes for the new DB Format   }
{                                    DB Version to 1                 }
{                                    Check for Filename == UID       }
{  1.61  ,08/22/2000 ,Dirk Heiser    Change Max DBName length to 31  }
{                                    Support for Categorys           }
{  1.7   ,10/16/2000 ,Dirk Heiser    Changes to be compatible with   }
{                                    Virtual Pascal and copiled with }
{                                    Virtual Pacal                   }
{                                    Max. Record size set to 65505 b.}
{  1.71  ,03/03/2002 ,Dirk Heiser    Fix for install DB for the visor}
{                                    Desktop Software.               }
{                                    Support for the Metadata Record.}
{  1.72  ,03/11/2002 ,Dirk Heiser    More than one user with ";"     }
{                                                                    }
{********************************************************************}
PROGRAM Conduit;

{$IFNDEF VIRTUALPASCAL}
    {$APPTYPE CONSOLE}
{$ENDIF}

USES SysUtils, Classes, ConstStrings, DBTools, Tools, PalmConst;

VAR
 Data               : TMemoryStream;
 Header             : TMemoryStream;
 AppInfo            : TMemoryStream;
 
 Sizes              : TStringList;
 Names              : TStringList;
 Req_NameID         : TStringList;
 Req_Name           : TStringList;
 Req_ID             : TStringList;
 
 TempData           : PByteArray;
 Config             : TConfig;
 Input              : FILE;
 
 Pos                : LongInt;
 Counter            : LongInt;
 Groesse            : Longint;
 I, II, III         : Longint;
 Padding            : LongInt;
 AppInfoSize        : LongInt;
 AppInfoPos         : LongInt;
 
 SizeDoc            : LongInt;
 SizeBmp            : LongInt;
 SizeMailto         : LongInt;
 SizeLinks          : LongInt;
 SizeBookmarks      : LongInt;
 SizeRest           : LongInt;
 SizeHeader         : LongInt;

BEGIN
 
 {******************************************************************}
 {******************************************************************}
 {* Check Command Line and Write Welcome message                   *}
 {******************************************************************}
 {******************************************************************}

 IF CheckCommandLine ( Config ) = False THEN
  BEGIN
   Write ( MSG_Welcome );
   Write ( MSG_Usage );
   Halt ( 1 );
  END;

 If Config.Verbose > 1 Then Write ( MSG_Welcome );

 InstallFile ( Config.Install, Config.User, Config.Verbose );
 If length ( Config.Plucker_Dir ) < 1 Then Halt ( 0 );
 {******************************************************************}
 {******************************************************************}
 
 
 
 {******************************************************************}
 {******************************************************************}
 {* Read the Datas and Write to a Stram                            *}
 {******************************************************************}
 {******************************************************************}
 If Config.Verbose > 0 THEN WriteLn ( Format ( MSG_Reading, [ Config.Plucker_Dir + Config.Cache + FileMask ] ) );
 Data := TMemoryStream.Create;
 Sizes := TStringList.Create;
 Names := TStringList.Create;
 Req_Name:= TStringList.Create;
 Req_NameID:= TStringList.Create;
 Req_ID:= TStringList.Create;

 If Length(Config.Category)>0 Then WriteCategoryRec(Config.Verbose,Config.Category,Config.Plucker_Dir + Config.Cache);

 I := GetFileList(Config.Plucker_Dir + Config.Cache + FileMask,Names);

 IF Names.Count < 1 THEN
  BEGIN
   If Config.Verbose = 0 THEN WriteLn ( Format ( MSG_Reading, [ Config.Plucker_Dir + Config.Cache + FileMask ] ) );
   WriteLN ( MSG_Error_Dir );
   Halt ( 1 );
  END;

 If I <> 0 Then
  BEGIN
   IF (I AND Index_Record) = Index_Record Then WriteLN ( MSG_Error_Index );
   Halt ( 1 );
  END;

 If Config.Verbose > 0
  THEN WriteLn ( Format ( MSG_DBType, [ CompressionTypes[GetReq(Config.Plucker_Dir + Config.Cache + IntToStr(Index_Record),Req_NameID, Req_Name, Req_ID) ]] ) )
  ELSE GetReq(Config.Plucker_Dir + Config.Cache + IntToStr(Index_Record),Req_NameID, Req_Name, Req_ID);

 SizeDoc:=0;
 SizeBmp:=0;
 SizeMailto:=0;
 SizeLinks:=0;
 SizeBookmarks:=0;
 SizeRest:=0;
 SizeHeader:=0;

 For I:=0 To Names.Count-1 DO
  BEGIN
   AssignFile ( Input, Config.Plucker_Dir + Config.Cache + Names[I] );
   FileMode := 0;
   Reset ( Input, 1 );
   If Config.Verbose > 1 THEN Write ( Format ( MSG_Read_File, [ IntToStr ( I+1 ), Names[I], Names[I], IntToStr ( FileSize ( Input ) ) ] ) );
   TRY
    GetMem ( TempData, FileSize ( Input ) );
    BlockRead ( Input, TempData^, FileSize ( Input ) );
    Groesse := FileSize ( Input );

    CheckUID(TempData,StrToInt(Names[I]));

    IF Config.Fix THEN
     FOR II := 0 TO FileSize ( Input ) - 2 DO
      IF ( TempData^ [ II ] = $0D ) AND ( TempData^ [ II + 1 ] = $0A )
       THEN
       BEGIN
        FOR III := II TO FileSize ( Input ) - 1 DO
         TempData^ [ III ] := TempData^ [ III + 1 ];
        Groesse := Groesse - 1;
       END;

    If Groesse < DBMinRecSize Then
     BEGIN
      WriteLN ( Format ( MSG_Error_Record, [ IntToStr(Groesse) ] ) );
      Halt ( 1 );
     END;

    If Groesse > RecMax Then
     BEGIN
      If Config.Verbose < 2 THEN Write ( Format ( MSG_Read_File, [ IntToStr ( I+1 ), Names[I], Names[I], IntToStr ( FileSize ( Input ) ) ] ) );
      WriteLN ( Format ( MSG_Error_Size, [ IntToStr(RecMax) ] ) );
      Halt ( 1 );
     END;

    Data.Write ( TempData^, Groesse );
    Sizes.Add ( IntToStr ( Groesse ) );
    CloseFile ( Input );
    If Config.Verbose > 1 THEN WriteLN ( MSG_Done );

    If Config.Verbose > 1 THEN Writeln(Format(MSG_FileInfo,[GetRecName(StrToInt(Names[I]),Req_Name, Req_ID),CT[GetCT(TempData,StrToInt(Names[I]))]]));

    IF StrToInt(Names[I]) = Index_Record
    Then SizeHeader := SizeHeader + Groesse+20
    ELSE
    Case GetCT(TempData,StrToInt(Names[I])) OF
      0: SizeDoc:=SizeDoc+Groesse+20;
      1: SizeDoc:=SizeDoc+Groesse+20;
      2: SizeBmp:=SizeBmp+Groesse+20;
      3: SizeBmp:=SizeBmp+Groesse+20;
      4: SizeMailto:=SizeMailto+Groesse+20;
      5: SizeLinks:=SizeLinks+Groesse+20;
      6: SizeLinks:=SizeLinks+Groesse+20;
      7: SizeLinks:=SizeLinks+Groesse+20;
      8: SizeBookmarks:=SizeBookmarks+Groesse+20;
      9: SizeHeader:=SizeHeader+Groesse+20;
     10: SizeHeader:=SizeHeader+Groesse+20;
     else SizeRest:=SizeRest+Groesse+20;
    End;

    FreeMem ( TempData );

   EXCEPT
    If Config.Verbose < 2 THEN Write ( Format ( MSG_Read_File, [ IntToStr ( I+1 ), Names[I], Names[I], IntToStr ( FileSize ( Input ) ) ] ) );
    WriteLN ( Format ( MSG_Error_FileR, [ Names[I] ] ) );
    Halt ( 1 );
   END;

  END;
 {******************************************************************}
 {******************************************************************}
 
 
 
 {******************************************************************}
 {******************************************************************}
 {* Build the Header and the Index and write to the Stream         *}
 {******************************************************************}
 {******************************************************************}
 Padding := 2;
 AppInfo := TMemoryStream.Create;

 If Length(Trim(Config.AppInfo)) > 0 Then
  Begin
   MakePQAAppInfoBlock(Config.AppInfo,Config.DBName,IntToStr(Config.DBVersion), AppInfo);
   AppInfoSize := AppInfo.Size;
   AppInfoPos := 78+Padding+(Names.Count*8);
  End
  Else
  Begin
   AppInfoSize := 0;
   AppInfoPos := 0;
  End;

 Header := TMemoryStream.Create;
 
 MakeDBHeader ( Config.DBName, Config.DBVersion, Now, Now, mydmNullTime, DBType, Config.DBID, Names.Count,Config.HFlags, AppInfoPos, Header );

 Counter := 0;
 FOR I := 0 TO Names.Count - 1 DO
  BEGIN
   Pos := 78 + Padding + ( Names.Count * 8 ) + Counter + AppInfoSize;
   MakeDBIndexEntry ( Pos, Record_attrib, StrToInt(Names.Strings[I]), Header );
   Counter := Counter + StrToInt ( Sizes.Strings [ I ] );
  END;
 
 I := 0;
 Header.WriteBuffer ( I, 2 );
 {******************************************************************}
 {******************************************************************}
 
 
 
 {******************************************************************}
 {******************************************************************}
 {* Put the Header&Index and the Data together and Save to File    *}
 {******************************************************************}
 {******************************************************************}
 TRY
  If Length(Trim(Config.AppInfo)) > 0 Then AppInfo.SaveToStream ( Header );
  Data.SaveToStream ( Header );
  If Config.Verbose > 0 Then Write ( Format ( MSG_Write_DB, [ Config.Plucker_Dir +
   Config.FName, IntToStr ( Trunc ( Header.Size / 1024 ) ) ] ) );
  Header.SaveToFile ( Config.Plucker_Dir + Config.FName );
  If Config.Verbose > 0 Then WriteLN ( MSG_Done );
  I := 0;
  If Config.Verbose > 1 Then WriteLN ( Format ( MSG_WriteDB_Info, [
    Format('%8.2f',[SizeDoc/1024]),
    Format('%8d',[SizeDoc]),
    Format('%8.2f',[(SizeDoc*100)/(SizeDoc+SizeBmp+SizeMailto+SizeLinks+SizeBookmarks+SizeHeader+SizeRest+84+I)]),

    Format('%8.2f',[SizeBmp/1024]),
    Format('%8d',[SizeBmp]),
    Format('%8.2f',[(SizeBmp*100)/(SizeDoc+SizeBmp+SizeMailto+SizeLinks+SizeBookmarks+SizeHeader+SizeRest+84+I)]),

    Format('%8.2f',[SizeMailto/1024]),
    Format('%8d',[SizeMailto]),
    Format('%8.2f',[(SizeMailto*100)/(SizeDoc+SizeBmp+SizeMailto+SizeLinks+SizeBookmarks+SizeHeader+SizeRest+84+I)]),

    Format('%8.2f',[SizeLinks/1024]),
    Format('%8d',[SizeLinks]),
    Format('%8.2f',[(SizeLinks*100)/(SizeDoc+SizeBmp+SizeMailto+SizeLinks+SizeBookmarks+SizeHeader+SizeRest+84+I)]),

    Format('%8.2f',[SizeBookmarks/1024]),
    Format('%8d',[SizeBookmarks]),
    Format('%8.2f',[(SizeBookmarks*100)/(SizeDoc+SizeBmp+SizeMailto+SizeLinks+SizeBookmarks+SizeHeader+SizeRest+84+I)]),

    Format('%8.2f',[SizeRest/1024]),
    Format('%8d',[SizeRest]),
    Format('%8.2f',[(SizeRest*100)/(SizeDoc+SizeBmp+SizeMailto+SizeLinks+SizeBookmarks+SizeHeader+SizeRest+84+I)]),

    Format('%8.2f',[(SizeHeader+84+I)/1024]),
    Format('%8d',[SizeHeader+84+I]),
    Format('%8.2f',[((SizeHeader+84+I)*100)/(SizeDoc+SizeBmp+SizeMailto+SizeLinks+SizeBookmarks+SizeHeader+SizeRest+84+I)]),

    Format('%8.2f',[(SizeDoc+SizeBmp+SizeMailto+SizeLinks+SizeBookmarks+SizeHeader+SizeRest+84+I)/1024]),
    Format('%8d',[(SizeDoc+SizeBmp+SizeMailto+SizeLinks+SizeBookmarks+SizeHeader+SizeRest+84+I)]),
    Format('%8.2f',[((SizeDoc+SizeBmp+SizeMailto+SizeLinks+SizeBookmarks+SizeHeader+SizeRest+84+I)*100)/(SizeDoc+SizeBmp+SizeMailto+SizeLinks+SizeBookmarks+SizeHeader+SizeRest+84+I)])
    ]));
 EXCEPT
  If Config.Verbose = 0 Then Write ( Format ( MSG_Write_DB, [ Config.Plucker_Dir +
   Config.FName, IntToStr ( Round ( Header.Size / 1024 ) ) ] ) );
  WriteLN ( Format ( MSG_Error_Save, [ Config.Plucker_Dir +
   Config.FName ] ) );
  Halt ( 1 );
 END;
 {******************************************************************}
 {******************************************************************}
 
 
 
 {******************************************************************}
 {******************************************************************}
 {* Check if Install for User need, get a List of User and Install *}
 {* if a valid user Found                                          *}
 {******************************************************************}
 {******************************************************************}
 InstallFile(Config.Plucker_Dir + Config.FName,Config.User, Config.Verbose);
 {******************************************************************}
 {******************************************************************}
 
 
 
 {******************************************************************}
 {******************************************************************}
 {* Free the Mem                                                   *}
 {******************************************************************}
 {******************************************************************}
 Req_Name.Free;
 Req_NameID.Free;
 Req_ID.Free;
 Sizes.Free;
 Names.Free;
 Data.Free;
 Header.Free;
 AppInfo.Free;
 {******************************************************************}
 {******************************************************************}
 
END.
