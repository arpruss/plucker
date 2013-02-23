{********************************************************************}
{                                                                    }
{           Program Name:     Tools Unit for Conduit.dpr             }
{           Source Language:  Delphi 3                               }
{           Author:           Dirk Heiser <conduit@Dirk-Heiser.de>   }
{                                                                    }
{********************************************************************}
{  This unit is part of the Plucker conduit for Win32, see in        }
{  Conduit.dpr for more Infos                                        }
{                                                                    }
{********************************************************************}
UNIT Tools;

INTERFACE


USES Classes,SysUtils, PalmUserData, ConstStrings, PalmConst;

TYPE
 TConfig = RECORD
  User        : STRING;
  Plucker_Dir : STRING;
  Cache       : STRING;
  DBID        : STRING;
  DBName      : STRING;
  FName       : STRING;
  Fix         : Boolean;
  DBVersion   : Integer;
  Install     : STRING;
  Verbose     : Integer;
  HFlags      : Word;
  AppInfo     : String;
  Category    : String;
 END;

FUNCTION CheckCommandLine ( VAR Config : TConfig ) : Boolean;

PROCEDURE InstallFile ( FileName, Users : STRING; Verbose: Integer );

Function GetFileList ( SearchFor : String;Var Files : TStringList ):Integer;

FUNCTION GetCT ( Data : PByteArray; UID : LongInt ):Integer;

FUNCTION GetRecName ( UID : LongInt; Req_Name_List,Req_UID_List : TStringList ):String;

FUNCTION GetReq ( FileName : String; Var Req_NameID_List, Req_Name_List,Req_UID_List : TStringList ):Integer;

PROCEDURE CheckUID ( Data : PByteArray; UID : LongInt );

Procedure WriteCategoryRec(Verbosity : Integer; Category, CacheDir : String);

IMPLEMENTATION

PROCEDURE CheckUID ( Data : PByteArray; UID : LongInt );
BEGIN

 If UID <> (Data^[0] SHL 8) OR Data^[1] Then
  BEGIN
   WriteLN ( Format ( MSG_Error_UID, [ IntToStr(UID) ] ) );
   Halt ( 1 );
  END;

END;

FUNCTION GetReq ( FileName : String; Var Req_NameID_List,Req_Name_List,Req_UID_List : TStringList ):Integer;
VAR
 TempData           : PByteArray;
 Input              : FILE;
 I                  : LongInt;

 Function ToLongInt(H,L:Byte):LongInt;
 BEGIN
  RESULT := (H SHL 8) OR L;
 END;

BEGIN

 AssignFile ( Input, FileName);
 FileMode := 0;
 Reset ( Input, 1 );
 GetMem ( TempData, FileSize ( Input ) );
 BlockRead ( Input, TempData^, FileSize ( Input ) );
 CloseFile(Input);

 If Sizeof(TempData^) < DBMinIRecSize Then
  BEGIN
   WriteLN ( Format ( MSG_Error_IRecord, [ IntToStr(Sizeof(TempData^)) ] ) );
   Halt ( 1 );
  END;

 If Sizeof(TempData^) < ((ToLongInt(TempData^[6-2],TempData^[6-1])*4)+DBMinIRecSize) Then
  BEGIN
   WriteLN ( MSG_Error_IRecordS );
   Halt ( 1 );
  END;

 Req_NameID_List.Clear;
 Req_Name_List.Clear;
 Req_UID_List.Clear;

 Req_NameID_List.Add(IntToStr(low(RESERVED_NAMES)+1));
 Req_Name_List.Add(RESERVED_NAMES[low(RESERVED_NAMES)+1]);
 Req_UID_List.Add(IntToStr(Index_Record));

 For I:=0 To ToLongInt(TempData^[6-2],TempData^[6-1])-1 DO
  BEGIN
   IF ToLongInt(TempData^[6+(I*4)+0],TempData^[6+(I*4)+1]) > High(RESERVED_NAMES) Then Req_NameID_List.Add(IntToStr(low(RESERVED_NAMES))) ELSE Req_NameID_List.Add(IntToStr(ToLongInt(TempData^[6+(I*4)+0],TempData^[6+(I*4)+1])));
   IF ToLongInt(TempData^[6+(I*4)+0],TempData^[6+(I*4)+1]) > High(RESERVED_NAMES) Then Req_Name_List.Add(RESERVED_NAMES[low(RESERVED_NAMES)]) ELSE Req_Name_List.Add(RESERVED_NAMES[ToLongInt(TempData^[6+(I*4)+0],TempData^[6+(I*4)+1])]);
   Req_UID_List.Add(IntToStr(ToLongInt(TempData^[6+(I*4)+2],TempData^[6+(I*4)+3])));
  END;

 IF ToLongInt(TempData^[2],TempData^[3]) > High(CompressionTypes) Then 
 Result := Low(CompressionTypes)
 ELSE Result := ToLongInt(TempData^[2],TempData^[3]);

END;

FUNCTION GetRecName ( UID : LongInt; Req_Name_List,Req_UID_List : TStringList ):String;
VAR
 I          : LongInt;
 Is_Special : LongInt;

BEGIN

 Is_Special := -1;

 For I:=0 To Req_UID_List.Count-1 Do
  If Req_UID_List[I] = IntToStr(UID) Then IS_Special := I;

 If Is_Special = -1 Then
  Result := RecName
  ELSE Result := Req_Name_List[IS_Special];

END;

FUNCTION GetCT ( Data : PByteArray; UID : LongInt ):Integer;
BEGIN

 IF UID <> Index_Record THEN
 BEGIN
  IF Sizeof(Data^) < DBMinRecSize Then
   BEGIN
    WriteLN ( Format ( MSG_Error_Record, [ IntToStr(Sizeof(Data^)) ] ) );
    Halt ( 1 );
   END
   ELSE
   BEGIN
    Result:=Data^[DBCTPOS-2];
    If Result > High(CT) Then Result:= Low(CT)
   END;
 END
 ELSE Result:= Low(CT)+1;

END;

FUNCTION CheckCommandLine ( VAR Config : TConfig ) : Boolean;

VAR
 I, II              : Integer;
 OK, AllOK          : Boolean;
 Para               : STRING;
 
BEGIN
 
 Config.DBVersion := StrToInt ( DBVersion );
 Config.Fix := False;
 Config.HFlags := $0000;
 Config.Verbose := StrToInt(Verbose);
 Config.AppInfo := Empty;
 Config.Category := Empty;
 AllOK := True;
 
 IF ParamCount > 0 THEN
  BEGIN
   FOR I := 1 TO ParamCount DO
    BEGIN
     OK := False;
     FOR II := 1 TO Switchcount DO
      BEGIN
       IF length ( ParamStr ( I ) ) >= length ( Switches [ II ] )
        THEN
        IF Lowercase ( copy ( ParamStr ( I ), 1, length (
         Switches [ II ] ) ) ) = Switches [ II ] THEN
         BEGIN
          Para := copy ( ParamStr ( I ), length ( Switches [ II ] ) +
           1, Length ( ParamStr ( I ) ) - length ( Switches [ II ] )
            );
          CASE II OF
           1 : Config.User := Para;
           2 : Config.Plucker_Dir := Para;
           3 : Config.Cache := Para;
           4 : Config.DBID := Para;
           5 : Config.DBName := Para;
           6 : Config.FName := Para;
           7 :
            IF length ( Para ) < 1 THEN Config.Fix := True;
           8 :
            IF length ( Para ) > 0 THEN
             try
              Config.DBVersion := StrToInt ( Para );
             EXCEPT
              AllOK := False;
             END;
           9 :
            IF length ( Para ) > 0 THEN Config.Install := Para;
           10:
            IF length ( Para ) > 0 THEN
             TRY
              Config.Verbose := StrToInt ( Para );
             EXCEPT
              AllOK := False;
             END;
           11:
            IF length ( Para ) < 1 THEN Config.HFlags := Config.HFlags OR dmHdrAttrCopyPrevention;
           12:
            IF length ( Para ) < 1 THEN Config.HFlags := Config.HFlags OR dmHdrAttrBackup;
           13:
            IF length ( Para ) < 1 THEN Config.HFlags := Config.HFlags OR dmHdrAttrLaunchableData;
           14:
            IF length ( Para ) < 1 THEN Config.AppInfo := STR_NonIcon;
           15:
            IF length ( Para ) > 0 THEN Config.AppInfo := Para;
           16:
            IF length ( Para ) > 0 THEN Config.Category := Para;
          END;
          OK := True;
         END;
      END;
     IF Ok = False THEN AllOK := False;
    END;

   IF length ( Config.Plucker_Dir ) < 1 THEN
    IF length ( Config.Install ) < 1 THEN AllOK := False;
   IF ( ( length ( Config.User ) < 1 ) AND ( length ( Config.Install )
    > 0 ) ) THEN AllOK := False;
   IF length ( Config.Plucker_Dir ) > 0 THEN
   IF Copy ( Config.Plucker_Dir, length ( Config.Plucker_Dir ), 1 )
    <> PathSep THEN
    Config.Plucker_Dir := Config.Plucker_Dir + PathSep;
   IF length ( Config.Cache ) = 0 THEN
    Config.Cache := CacheDir
   ELSE IF Copy ( Config.Cache, length ( Config.Cache ), 1 ) <>
    PathSep THEN
    Config.Cache := Config.Cache + PathSep;
   IF length ( Config.DBID ) <> 4 THEN Config.DBID := DBCreator;
   IF length ( Config.DBName ) = 0 THEN Config.DBName := DBName;
   If Length(Config.DBName) > (dmDBNameLength-1-DBReservedLength) Then Config.DBName := Copy(Config.DBName,1,(dmDBNameLength-1-DBReservedLength));
   For I:=1 To Length(Config.DBName) Do If NOT (Config.DBName[I] in mydmValidDBNameChars) Then Config.DBName[I] := DBNameSubstitute;
   IF length ( Config.FName ) = 0 THEN Config.FName := DBFileName;
   If Length(Config.Plucker_Dir) > 0 Then Config.Plucker_Dir := ExpandFileName(Config.Plucker_Dir);
  END
 ELSE
  AllOK := False;
 
 result := AllOK;
 
END;

FUNCTION GetFirstUser(Var Users: String):String;
Const
 SEPP = ';';

Begin

 Users:=Trim(Users);
 If Users[1]=SEPP Then Repeat Users:=Copy(Users,2,Length(Users)-1); Until Users[1]<>SEPP;
 If Users[Length(Users)]=SEPP Then Repeat Users:=Copy(Users,1,Length(Users)-1); Until Users[Length(Users)]<>SEPP;

 If Pos(SEPP, Users)=0 Then
 Begin
  Result:=Trim(Users);
  Users:='';
 End
 Else
 Begin
  Result:=Trim(Copy(Users,1,Pos(SEPP,Users)-1));
  Users:=Copy(Users,Pos(SEPP,Users)+1,Length(Users)-Pos(SEPP,Users));
 End;

End;

PROCEDURE InstallFile ( FileName, Users : STRING; Verbose: Integer );

VAR
 UserData           : TPalmUserData;
 Found              : Longint;
 I                  : Longint;
 User               : String;

BEGIN

 User:='';
 Repeat

 User:=GetFirstUser(Users);

 IF (length ( User ) > 0) AND (length ( FileName ) > 0) THEN
  IF FileExists ( FileName ) THEN
   TRY
    If Verbose > 0 Then Write ( Format ( MSG_Installing, [ ExtractFileName ( FileName ), User ] ) );
    UserData := TPalmUserData.Create;
    Found := 0;
    FOR I := 0 TO UserData.GetNumUsers - 1 DO
     IF User = UserData.GetUserName ( I ) THEN
      BEGIN
       UserData.InstallUser ( I, FileName );
       If Verbose > 0 THEN WriteLn ( MSG_Done );
       Found := 1;
      END;
    IF Found = 0 THEN
     BEGIN
      If Verbose = 0 Then  Write ( Format ( MSG_Installing, [ ExtractFileName ( FileName ), User ] ) );
      WriteLN ( Format ( MSG_Error_UserNF, [ User ] ) );
     END;
    UserData.Free;
    If Verbose > 0 Then Writeln;
   EXCEPT
    If Verbose = 0 Then  Write ( Format ( MSG_Installing, [ ExtractFileName ( FileName ), User ] ) );
    WriteLN ( Format ( MSG_Error_User, [ User ] ) );
    If Verbose > 0 Then Writeln;
    Halt ( 1 );
   END
  ELSE
   BEGIN
    If Verbose = 0 Then  Write ( Format ( MSG_Installing, [ ExtractFileName ( FileName ), User ] ) );
    WriteLN ( Format ( MSG_Error_FNF, [ ExtractFileName ( FileName ) ] ) );
    If Verbose > 0 Then Writeln;
    Halt ( 1 );
   END;

 Until length ( User ) = 0;

END;

Function GetFileList ( SearchFor : String;Var Files : TStringList ):Integer;

Var
 SearchRec          : TSearchRec;
 Found              : Longint;

 BIndex_Record      : Boolean;

Function IsFileName(FileName : String):Boolean;
Var
 I : LongInt;

BEGIN

 Result:=True;
 For I:=1 To Length(FileName) Do If NOT (FileName[I] IN ValidChars) Then Result := False;

END;

Procedure SortFiles(FileName : String; Var Files : TStringList );
Var
 I : LongInt;

Begin

 If Files.Count > 1 Then
  If StrToInt(FileName) > StrToInt(Files[Files.Count-1]) Then Files.Add(FileName) ELSE
   If StrToInt(FileName) < StrToInt(Files[0]) Then Files.Insert(0,FileName) ELSE
    BEGIN
     I:=-1;
     Repeat
      I:=I+1;
     Until ((StrToInt(FileName) >= StrToInt(Files[I])) AND (StrToInt(FileName) < StrToInt(Files[I+1]))) OR (I=Files.Count-2);
     Files.Insert(I+1,FileName);
    END
    Else
     If Files.Count = 0
      Then Files.Add(FileName)
      ELSE 
       If StrToInt(FileName) > StrToInt(Files[0])
        Then Files.Add(FileName)
        Else Files.Insert(0,FileName);

END;

BEGIN

 BIndex_Record := False;

 Files.Clear;
 Found := FindFirst ( SearchFor, $27, SearchRec );
 WHILE Found = 0 DO
  BEGIN
   If IsFileName(SearchRec.Name) Then
    Begin
     SortFiles(SearchRec.Name,Files);
     If SearchRec.Name = IntToStr(Index_Record) Then BIndex_Record := True;
    End;
   Found := FindNext ( SearchRec );
  END;
 FindClose ( SearchRec );

 Result := 0;
 If BIndex_Record = False Then Result := Result OR Index_Record;

End;

{That's really dirty code, but it work :-) }
Procedure WriteCategoryRec(Verbosity : Integer; Category, CacheDir : String);
Var
 Input              : FILE of Byte;
 B1,B2              : Byte;
 I,II               : LongInt;
 Name               : Integer;
 Req_Name           : TStringList;
 Req_NameID         : TStringList;
 Req_ID             : TStringList;
 ZLib               : Integer;

Begin

 Req_Name := TStringList.Create;
 Req_NameID := TStringList.Create;
 Req_ID := TStringList.Create;
 Name := -1;

 ZLib := GetReq(CacheDir + IntToStr(Index_Record),Req_NameID, Req_Name, Req_ID);

 {Is there are category already?}
 If Req_NameID.Find(IntToStr(Category_Record_ID),I) = True Then
  Begin
   If Verbosity > 0 Then Writeln(MSG_Rem_Cat);
   Name:=StrToInt(Req_ID[I]);
  End;

 {No Category index There, foud a free UID}
 If Name = -1 Then For I:=2 to 100000 Do If FileExists(CacheDir+IntToStr(I)) = False Then
  Begin
   Name := I;
   Req_Name.Add(RESERVED_NAMES[Category_Record_ID]);
   Req_NameID.Add(IntToStr(Category_Record_ID));
   Req_ID.Add(IntToStr(Name));
   Break;
  End;

 If Name = -1 Then
  Begin
   Writeln(MSG_Error_FindFile);
   Halt(1);
  End;

 {Write a New Index Record}
 AssignFile ( Input, CacheDir+IntToStr(Index_Record));
 FileMode:=1;
 ReWrite ( Input );

 {UID}
 I := Index_Record;
 B1 := (I SHR 8) AND $FF;
 B2 := I AND $FF;
 Write(Input,B1);
 Write(Input,B2);

 {Version}
 I := ZLib;
 B1 := (I SHR 8) AND $FF;
 B2 := I AND $FF;
 Write(Input,B1);
 Write(Input,B2);

 {Records}
 I := Req_ID.Count-1;
 B1 := (I SHR 8) AND $FF;
 B2 := I AND $FF;
 Write(Input,B1);
 Write(Input,B2);

 For II:= 1 To Req_ID.Count-1 Do
  Begin
   {Name}
   I := StrToInt(Req_NameID[II]);
   B1 := (I SHR 8) AND $FF;
   B2 := I AND $FF;
   Write(Input,B1);
   Write(Input,B2);
   {ID}
   I := StrToInt(Req_ID[II]);
   B1 := (I SHR 8) AND $FF;
   B2 := I AND $FF;
   Write(Input,B1);
   Write(Input,B2);
  End;

 CloseFile(Input);

 If Verbosity > 0 Then Writeln(Format(MSG_New_Cat,[Category]));

 {Write the Category Record}
 AssignFile ( Input, CacheDir+IntToStr(Name));
 FileMode:=1;
 ReWrite(Input);

 I := Name;
 B1 := (I SHR 8) AND $FF;
 B2 := I AND $FF;
 Write(Input,B1);
 Write(Input,B2);

 I := 0;
 B1 := (I SHR 8) AND $FF;
 B2 := I AND $FF;
 Write(Input,B1);
 Write(Input,B2);

 I := 0;
 B1 := (I SHR 8) AND $FF;
 B2 := I AND $FF;
 Write(Input,B1);
 Write(Input,B2);

 I := Category_Record_CT;
 B1 := (I SHR 8) AND $FF;
 B2 := I AND $FF;
 Write(Input,B2);
 Write(Input,B1);

 For I:=1 To Length(Category) Do
  Begin
   B1 := Ord(Category[I]);
   Write(Input,B1);
  End;
 B1 := 0;
 Write(Input,B1);

 CloseFile(Input);

End;

END.
