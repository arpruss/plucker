{********************************************************************}
{                                                                    }
{           Program Name:     DBTools Unit for Conduit.dpr           }
{           Source Language:  Delphi 3                               }
{           Author:           Dirk Heiser <conduit@Dirk-Heiser.de>   }
{                                                                    }
{********************************************************************}
{  This unit is part of the Plucker conduit for Win32, see in        }
{  Conduit.dpr for more Infos                                        }
{                                                                    }
{********************************************************************}
UNIT DBTools;

INTERFACE

{$R Resource.res}

USES Classes, SysUtils, Windows, ConstStrings;

TYPE
 TDBHeader = ARRAY [ 0..77 ] OF Byte;
 TDBIndex = ARRAY [ 0..7 ] OF Byte;
 
PROCEDURE MakeDBHeader ( DBName : STRING; DBVersion : LongInt; CTime, MTime, BTime
 :
  TDateTime; DBType : STRING; DBCreator : STRING; RecCount : LongInt;
 Flags : Word; AppInfo : LongInt; VAR Stream : TMemoryStream );

PROCEDURE MakeDBIndexEntry ( Pos : LongInt; Attrib : Byte; ID :
 LongInt; VAR Stream : TMemoryStream );

PROCEDURE MakePQAAppInfoBlock ( Icon,Name,Version : String; VAR Stream : TMemoryStream );

IMPLEMENTATION

PROCEDURE MakeDBHeader ( DBName : STRING; DBVersion : LongInt; CTime, MTime, BTime
 :
  TDateTime; DBType : STRING; DBCreator : STRING; RecCount : LongInt;
 Flags : Word; AppInfo : LongInt; VAR Stream : TMemoryStream );

VAR
 TimePalm           : Cardinal;
 Header             : TDBHeader;
 I                  : Integer;
 
BEGIN
 
 { Write: DBname [32 Char]******************************************}
 FOR I := 0 TO 31 DO
  Header [ I ] := 0;
 FOR I := 0 TO Length ( DBName ) - 1 DO
  Header [ I ] := Ord ( DBName [ I + 1 ] );
 { Write: flags [Word]**********************************************}
 Header [ 32 ] := ( Flags AND $FF00 ) SHR 8;
 Header [ 33 ] := ( Flags AND $00FF ) SHR 0;
 { Write: version [Word]********************************************}
 Header [ 34 ] := ( DBVersion AND $FF00 ) SHR 8;
 Header [ 35 ] := ( DBVersion AND $00FF ) SHR 0;
 { Write: create_time [DWord]***************************************}
 TimePalm := ( ( Trunc ( CTime ) * 86400 ) - ( 1462 * 86400 ) ) +
  Trunc ( Frac ( CTime ) * 86400 );
 Header [ 36 ] := ( TimePalm AND $FF000000 ) SHR 24;
 Header [ 37 ] := ( TimePalm AND $00FF0000 ) SHR 16;
 Header [ 38 ] := ( TimePalm AND $0000FF00 ) SHR 8;
 Header [ 39 ] := ( TimePalm AND $000000FF ) SHR 0;
 { Write: mod_time [DWord]******************************************}
 TimePalm := ( ( Trunc ( MTime ) * 86400 ) - ( 1462 * 86400 ) ) +
  Trunc ( Frac ( MTime ) * 86400 );
 Header [ 40 ] := ( TimePalm AND $FF000000 ) SHR 24;
 Header [ 41 ] := ( TimePalm AND $00FF0000 ) SHR 16;
 Header [ 42 ] := ( TimePalm AND $0000FF00 ) SHR 8;
 Header [ 43 ] := ( TimePalm AND $000000FF ) SHR 0;
 { Write: backup_time [DWord]***************************************}
 TimePalm := ( ( Trunc ( BTime ) * 86400 ) - ( 1462 * 86400 ) ) +
  Trunc ( Frac ( BTime ) * 86400 );
 Header [ 44 ] := ( TimePalm AND $FF000000 ) SHR 24;
 Header [ 45 ] := ( TimePalm AND $00FF0000 ) SHR 16;
 Header [ 46 ] := ( TimePalm AND $0000FF00 ) SHR 8;
 Header [ 47 ] := ( TimePalm AND $000000FF ) SHR 0;
 { Write: mod_num [DWord]*******************************************}
 FOR I := 48 TO 51 DO
  Header [ I ] := 0;
 { Write: app_info [DWord]******************************************}
 Header [ 52 ] := ( AppInfo AND $FF000000 ) SHR 24;
 Header [ 53 ] := ( AppInfo AND $00FF0000 ) SHR 16;
 Header [ 54 ] := ( AppInfo AND $0000FF00 ) SHR 8;
 Header [ 55 ] := ( AppInfo AND $000000FF ) SHR 0;
 { Write: sort_info [DWord]*****************************************}
 FOR I := 56 TO 59 DO
  Header [ I ] := 0;
 { Write: apptype [4 Char]******************************************}
 FOR I := 1 TO 4 DO
  Header [ I + 59 ] := Ord ( DBType [ I ] );
 { Write: id [4 Char]***********************************************}
 FOR I := 1 TO 4 DO
  Header [ I + 63 ] := Ord ( DBCreator [ I ] );
 { Write: unique_id_seed [DWord]; next_record_list [DWord]**********}
 FOR I := 68 TO 75 DO
  Header [ I ] := 0;
 { Write: num_records [DWord]***************************************}
 Header [ 76 ] := ( RecCount AND $FF00 ) SHR 8;
 Header [ 77 ] := RecCount AND $00FF;
 
 Stream.WriteBuffer ( Header, 78 );
 
END;

PROCEDURE MakeDBIndexEntry ( Pos : LongInt; Attrib : Byte; ID :
 LongInt; VAR Stream : TMemoryStream );

VAR
 Index              : TDBIndex;
 
BEGIN
 
 { Write: Offset [DWord]**offset to record 0 from the **************}
 {                        beginning of the file*********************}
 Index [ 0 ] := ( Pos AND $FF000000 ) SHR 24;
 Index [ 1 ] := ( Pos AND $00FF0000 ) SHR 16;
 Index [ 2 ] := ( Pos AND $0000FF00 ) SHR 08;
 Index [ 3 ] := ( Pos AND $000000FF ) SHR 00;
 { Write: attrib [Byte]*********************************************}
 Index [ 4 ] := Attrib;
 { Write: unique ID [3 Bytes]***************************************}
 Index [ 5 ] := ( ID AND $FF0000 ) SHR 24;
 Index [ 6 ] := ( ID AND $00FF00 ) SHR 8;
 Index [ 7 ] := ( ID AND $0000FF ) SHR 0;

 Stream.WriteBuffer ( Index, 8 );
 
END;

PROCEDURE MakePQAAppInfoBlock ( Icon,Name,Version : String; VAR Stream : TMemoryStream );
Var
  Temp     : TMemoryStream;
  TempRes  : TResourceStream;
  Dummy    : Word;
  Ch       : Byte;
  DummyStr : String;

 Function SwapWord(Input : Word ): Word;
  BEGIN
   Result := ((Input AND $00FF) SHL 08) + ((Input AND $FF00) SHR 08);
  END;

 Procedure WriteText(Input : String; Var Stream : TMemoryStream);
 Var
   Dummy : Word;
   Text : String;
   I   : LongInt;
   Ch : Byte;

   Function SwapWord(Input : Word ): Word;
    BEGIN
     Result := ((Input AND $00FF) SHL 08) + ((Input AND $FF00) SHR 08);
    END;

  BEGIN

   Text := Input;
   Text:=Text+#0;
   If Frac(Length(Text)/2) <> 0 Then Text:=Text+#00;
   Dummy:=SwapWord(Trunc(Length(Text)/2));
   Stream.WriteBuffer ( Dummy, 2 );
   For I:=1 To Length(Text) Do
    Begin
     Ch := Ord(Text[I]);
     Stream.WriteBuffer ( Ch, 1 );
    End;

  END;

BEGIN

 Version:=Format(VersionStr,[Version]);
 Stream.Clear;

 DummyStr := PQASignature;
 Stream.WriteBuffer ( DummyStr[1], 1 );
 Stream.WriteBuffer ( DummyStr[2], 1 );
 Stream.WriteBuffer ( DummyStr[3], 1 );
 Stream.WriteBuffer ( DummyStr[4], 1 );

 Dummy:=SwapWord(PQAhdrVersion);
 Stream.WriteBuffer ( Dummy, 2 );

 Dummy:=SwapWord(PQAencVersion);
 Stream.WriteBuffer ( Dummy, 2 );

 WriteText(Version,Stream);

 WriteText(Name,Stream);
 
 If FileExists(Format(BigIconName,[Icon])) Then
  Begin
   Temp := TMemoryStream.Create;
   Temp.Clear;
   Temp.LoadFromFile(Format(BigIconName,[Icon]));
   Ch := 0;
   If Frac(Temp.Size/2) <> 0 Then Temp.WriteBuffer ( Ch, 1 );
   Dummy:=SwapWord(Trunc(Temp.Size/2));
   Stream.WriteBuffer ( Dummy, 2 );
   Temp.SaveToStream ( Stream );
   Temp.Free;
  End
  Else
  Begin
   TempRes := TResourceStream.CreateFromID(hinstance,BIGICON,RT_RCDATA);
   Ch := 0;
   If Frac(TempRes.Size/2) <> 0 Then TempRes.WriteBuffer ( Ch, 1 );
   Dummy:=SwapWord(Trunc(TempRes.Size/2));
   Stream.WriteBuffer ( Dummy, 2 );
   TempRes.SaveToStream ( Stream );
   TempRes.Free;
  End;

 If FileExists(Format(SmallIconName,[Icon])) Then
  Begin
   Temp := TMemoryStream.Create;
   Temp.Clear;
   Temp.LoadFromFile(Format(SmallIconName,[Icon]));
   Ch := 0;
   If Frac(Temp.Size/2) <> 0 Then Temp.WriteBuffer ( Ch, 1 );
   Dummy:=SwapWord(Trunc(Temp.Size/2));
   Stream.WriteBuffer ( Dummy, 2 );
   Temp.SaveToStream ( Stream );
   Temp.Free;
  End
  Else
  Begin
   TempRes := TResourceStream.CreateFromID(hinstance,SMALICON,RT_RCDATA);
   Ch := 0;
   If Frac(TempRes.Size/2) <> 0 Then TempRes.WriteBuffer ( Ch, 1 );
   Dummy:=SwapWord(Trunc(TempRes.Size/2));
   Stream.WriteBuffer ( Dummy, 2 );
   TempRes.SaveToStream ( Stream );
   TempRes.Free;
  End;

END;

END.
