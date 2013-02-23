{ --------------------------------------------------------
	 TPalmUserData Class for Delphi
	 A Delphi class to auto-install Palm .PDB and .PRC files.

	 Copyright © 1997, 2000 by Art Dahm
	 All rights reserved

	 Web:     http://palm.dahm.com
	 E-mail:  palm@dahm.com
 --------------------------------------------------------
	 Based loosely on the C++ TPalmUserData class
	 created by Mark Pierce (MakeDocW)
 -------------------------------------------------------- }

unit PalmUserData;

interface

uses
	Windows, SysUtils;

type
	PUsers = ^TPalmUsers;

	TPalmUsers = record
		Magic: Word;
		Name: Array[0..31] of Char;
		SubDir: Array[0..31] of Char;
	end;

	TPalmUserData = class
	private
		FUsers: Byte;
		FLastUserNumber: Integer;
		FPalmDesktopPath: Array[0..255] of Char;
		FLastUserName: Array[0..31] of Char;
		FUserList: Pointer;
		procedure	ReadUserData;
	public
		constructor	Create;
		destructor Destroy; override;
		function GetLastUserNumber: Integer;
		function GetNumUsers: Integer;
		function GetPalmDesktopPath: String;
		function GetUserMagic(User: Integer): Integer;
		function GetUserName(User: Integer): String;
		function GetUserSubDir(User: Integer): String;
		function InstallUser(User: Integer; FileName: String): Boolean;
	end;

const
	SizeOfTPalmUsers = sizeof(TPalmUsers);

implementation

constructor TPalmUserData.Create;
var
	HKeyHnd: HKEY;
	HKeyUsr: HKEY;
	KeyLen: DWORD;
	KeyType: DWORD;
begin
	inherited;
	FUserList := nil;
	FUsers := 0;
	FLastUserNumber := 0;
	FLastUserName := '';
	HKeyUsr := 0;

	// First look for DeskTop 1.0 Entries...
	if (RegOpenKey(HKEY_CURRENT_USER, 'Software\U.S. Robotics', HKeyUsr) <>
		ERROR_SUCCESS) then
	begin
		// Not found, look for DeskTop 2.0 entry....
		if (RegOpenKey(HKEY_CURRENT_USER, 'Software\Palm Computing', HKeyUsr) <>
			ERROR_SUCCESS) then
		begin
			MessageBox(0, 'The Palm Desktop software is not correctly installed.',
				'Install', MB_ICONSTOP or MB_OK);
			HKeyUsr := 0;
		end;
	end;

	if (HKeyUsr <> 0) then
	begin
		if (RegOpenKey(HKeyUsr, 'Pilot Desktop\Preferences', HKeyHnd) =
			ERROR_SUCCESS) then
		begin
			KeyLen := sizeof(FLastUserName);
			KeyType := REG_SZ;
			RegQueryValueEx(HKeyHnd, 'LastUserName', nil, @KeyType, @FLastUserName,
				@KeyLen);
			RegCloseKey(HKeyHnd);
		end;

		if (RegOpenKey(HKeyUsr, 'Pilot Desktop\Core', HKeyHnd) = ERROR_SUCCESS) then
		begin
			KeyLen := sizeof(FPalmDesktopPath);
			KeyType := REG_SZ;
			if (RegQueryValueEx(HKeyHnd, 'Path', nil, @KeyType, @FPalmDesktopPath,
				@KeyLen) = ERROR_SUCCESS) then
					ReadUserData();
			RegCloseKey(HKeyHnd);
		end;

		RegCloseKey(HKeyUsr);
	end;
end;

destructor TPalmUserData.Destroy;
begin
	if FUserList <> nil then
		FreeMem(FUserList);
	FUsers := 0;
	inherited;
end;

procedure TPalmUserData.ReadUserData;
var
	UserData: file;
	StringLen: Byte;
	Offset: LongInt;
	Count: Integer;
	ListItemP: PUsers;
	FoundUser: Boolean;
begin
	AssignFile(UserData, FPalmDesktopPath + '\users.dat');
	try
		Offset := 0;
		Reset(UserData, 1);
		Seek(UserData, Offset);
		BlockRead(UserData, FUsers, 1);

		if (FUsers > 0) then
		begin
			GetMem(FUserList, SizeOfTPalmUsers * FUsers);

			if (FUserList <> nil) then
			begin
				// Offset of first user entry
				Offset := 13;
				ListItemP := PUsers(FUserList);

				for Count := 0 to FUsers-1 do
				begin
					StringLen := 0;
					ListItemP^.Magic := 0;

					// Read the user's magic number
					Seek(UserData, Offset);
					BlockRead(UserData, ListItemP^.Magic, 2);

					// Read the user's name
					Inc(Offset,4);
					Seek(UserData, Offset);
					BlockRead(UserData, StringLen, 1);
					BlockRead(UserData, ListItemP^.Name, StringLen);
					ListItemP^.Name[StringLen] := Chr(0);
					Offset := Offset + (StringLen + 1);

					// Check if this was the last user
					if (StrComp(PChar(@(FLastUserName[0])),
						PChar(@(ListItemP^.Name[0]))) = 0) then
							fLastUserNumber := Count;

					// Read the user's sub-dir
					BlockRead(UserData, StringLen, 1);
					BlockRead(UserData, ListItemP^.SubDir, StringLen);
					ListItemP^.SubDir[StringLen] := Chr(0);
					Offset := Offset + (StringLen + 1);

					// Read the flag byte
					BlockRead(UserData, StringLen, 1);

					// Set offset of next user entry
					if Count < (FUsers-1) then
					begin
						Inc(Offset, 1);
						FoundUser := False;
						while not FoundUser do
						begin
							Inc(Offset, 1);
							BlockRead(UserData, StringLen, 1);
							if StringLen = 1 then
							begin
								Inc(Offset, 1);
								BlockRead(UserData, StringLen, 1);
								if StringLen = 128 then
									FoundUser := True;
							end;
						end;
					end;
					Inc(ListItemP);
				end;
			end;
		end;
	finally
		CloseFile(UserData);
	end;
end;

function TPalmUserData.GetNumUsers: Integer;
begin
	Result := FUsers;
end;

function TPalmUserData.GetLastUserNumber: Integer;
begin
	Result := fLastUserNumber;
end;

function TPalmUserData.GetPalmDesktopPath: String;
begin
	Result := FPalmDesktopPath;
end;

function TPalmUserData.GetUserMagic(User: Integer): Integer;
var
	UsersP: PUsers;
begin
	if ((FUserList <> nil) and (User >= 0) and (User < FUsers)) then
	begin
		UsersP := FUserList;
		Inc(UsersP, User);
		Result := UsersP^.Magic;
	end
	else
		Result := 0;
end;

function TPalmUserData.GetUserName(User: Integer): String;
var
	UsersP: PUsers;
begin
	if ((FUserList <> nil) and (User >= 0) and (User < FUsers)) then
	begin
		UsersP := FUserList;
		Inc(UsersP, User);
		Result := UsersP^.Name;
	end
	else
		Result := '';
end;

function TPalmUserData.GetUserSubDir(User: Integer): String;
var
	UsersP: PUsers;
begin
	if ((FUserList <> nil) and (User >= 0) and (User < FUsers)) then
	begin
		UsersP := FUserList;
		Inc(UsersP, User);
		Result := UsersP^.SubDir;
	end
	else
		Result := '';
end;

function TPalmUserData.InstallUser(User: Integer; FileName: String): Boolean;
var
	HKeyHnd: HKEY;
	HKeyUsr: HKEY;
	InstallPath: String;
	Param: String;
	KeyValue: Integer;
begin
	Result := False;
	if (GetNumUsers > 0) then
	begin
		HKeyUsr := 0;

		InstallPath := GetPalmDesktopPath() + '\' + GetUserSubDir(User) + '\Install';
		CreateDirectory(PChar(InstallPath), nil);

		InstallPath := InstallPath + '\' + ExtractFileName(FileName);
		if CopyFile(PChar(FileName), PChar(InstallPath), False) then
		begin
			// First look for DeskTop 1.0 Entries...
			if (RegOpenKey(HKEY_CURRENT_USER, 'Software\U.S. Robotics', HKeyUsr) <>
				ERROR_SUCCESS) then
			begin
				// Not found, look for DeskTop 2.0 entry....
				if (RegOpenKey(HKEY_CURRENT_USER, 'Software\Palm Computing', HKeyUsr) <>
					ERROR_SUCCESS) then
				begin
					MessageBox(0, 'The Palm Desktop software is not correctly installed.',
						'Install', MB_ICONSTOP or MB_OK);
					HKeyUsr := 0;
				end;
			end;

			if (HKeyUsr <> 0) then
			begin
					if RegOpenKey(HKeyUsr, 'Pilot Desktop\HotSync Manager', HKeyHnd) =
						ERROR_SUCCESS then
					begin
						KeyValue := 1;

						Param := 'Install' + IntToStr(GetUserMagic(User));
						RegSetValueEx(HKeyHnd, PChar(Param), 0, REG_DWORD, @KeyValue,
							sizeof(DWORD));

						RegCloseKey(HKeyHnd);
						Result := True;
					end;
					RegCloseKey(HKeyUsr);
			end;
		end;
	end;
end;

end.
