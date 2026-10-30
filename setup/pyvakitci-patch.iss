[Setup]
AppId={{e4b0d240-ad26-450d-abb1-a3619274df91}
AppName=PyVakitci
AppVersion=1.7
AppVerName=PyVakitci 1.7
AppPublisherURL=https://sourceforge.net/projects/pyvakitci2011
AppSupportURL=https://sourceforge.net/projects/pyvakitci2011
AppUpdatesURL=https://sourceforge.net/projects/pyvakitci2011
DefaultDirName={pf}\PyVakitci
DefaultGroupName=PyVakitci
AllowNoIcons=yes
LicenseFile=C:\Lisans.txt
OutputDir=C:\Users\rahma\Desktop
OutputBaseFilename=pyvakitci-1.7-patch
SetupIconFile=
Compression=lzma
SolidCompression=yes

[Code]
// Yüklü olan sürümü kontrol edip eski sürüm kaldýrýldýktan sonra yeni sürüm kurulur.
// Her sürümde AppId ayný olmalý.
// http://www.lextm.com/2007/08/inno-setup-script-sample-for-version-comparison-2

// OutputDir bölümünü kendinize göre düzenlemelisiniz.

// Her yeni güncellemede PyVakitci.exe ve library.zip dosyasý deðiþir.
// [Files] kýsmýnda bu dosyalarla patch dosyasý oluþturulmaktadýr.

function GetNumber(var temp: String): Integer;
var
  part: String;
  pos1: Integer;
begin
  if Length(temp) = 0 then
  begin
    Result := -1;
    Exit;
  end;
    pos1 := Pos('.', temp);
    if (pos1 = 0) then
    begin
      Result := StrToInt(temp);
    temp := '';
    end
    else
    begin
    part := Copy(temp, 1, pos1 - 1);
      temp := Copy(temp, pos1 + 1, Length(temp));
      Result := StrToInt(part);
    end;
end;
 
function CompareInner(var temp1, temp2: String): Integer;
var
  num1, num2: Integer;
begin
    num1 := GetNumber(temp1);
  num2 := GetNumber(temp2);
  if (num1 = -1) or (num2 = -1) then
  begin
    Result := 0;
    Exit;
  end;
      if (num1 > num2) then
      begin
        Result := 1;
      end
      else if (num1 < num2) then
      begin
        Result := -1;
      end
      else
      begin
        Result := CompareInner(temp1, temp2);
      end;
end;
 
function CompareVersion(str1, str2: String): Integer;
var
  temp1, temp2: String;
begin
    temp1 := str1;
    temp2 := str2;
    Result := CompareInner(temp1, temp2);
end;

const
WM_QUIT = 18;

function InitializeSetup(): Boolean;
var
  oldVersion: String;
  uninstaller: String;
  ErrorCode: Integer;
  winHwnd: longint;
  resultClose : boolean;
  retVal : boolean;
  strProg: string;
begin

  resultClose := true;
  try
    strProg := 'PyVakitci';
    winHwnd := FindWindowByWindowName(strProg);
    Log('winHwnd: ' + inttostr(winHwnd));
    if winHwnd <> 0 then
      retVal:=postmessage(winHwnd,WM_QUIT,0,0);
      if retVal then
        resultClose := True
      else
        resultClose := False;

  except
  end;

  if RegKeyExists(HKEY_LOCAL_MACHINE,
    'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{e4b0d240-ad26-450d-abb1-a3619274df91}_is1') then
  begin
    RegQueryStringValue(HKEY_LOCAL_MACHINE,
      'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{e4b0d240-ad26-450d-abb1-a3619274df91}_is1',
      'DisplayVersion', oldVersion);

    // Her yeni güncelleme de 1.75 olan kýsmýn deðeri attýrýlmalýdýr.
    if (CompareVersion(oldVersion, '1.75') < 0) then
    begin
      if MsgBox('PyVakitci ' + oldVersion + ' yüklü. En güncel PyVakitci programýný kurmak ister misiniz?',
        mbConfirmation, MB_YESNO) = IDNO then
      begin
        Result := False;
      end
      else
      begin
          Result := True;
      end;
    end
    else
    begin
      MsgBox('PyVakitci ' + oldVersion + ' zaten yüklü.',
        mbInformation, MB_OK);
      Result := False;
    end;
  end
  else
  begin
    Result := True;
  end;
end;

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "turkish"; MessagesFile: "compiler:Languages\Turkish.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "C:\pyvakitci-yama\PyVakitci.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\pyvakitci-yama\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\PyVakitci"; Filename: "{app}\PyVakitci.exe"; WorkingDir: "{app}\"
Name: "{group}\{cm:ProgramOnTheWeb,PyVakitci}"; Filename: "https://sourceforge.net/projects/pyvakitci2011"
Name: "{group}\{cm:UninstallProgram,PyVakitci}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\PyVakitci"; Filename: "{app}\PyVakitci.exe"; WorkingDir: "{app}\"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\PyVakitci"; Filename: "{app}\PyVakitci.exe"; WorkingDir: "{app}\"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\PyVakitci.exe"; Description: "{cm:LaunchProgram,PyVakitci}"; Flags: nowait postinstall skipifsilent
