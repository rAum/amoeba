#pragma once
#include "pch.h"
#include "shlwapi.h"
#pragma comment(lib,"shlwapi.lib")

class CRegister
{
public:
	static bool DisableWSH()
	{
		HKEY wsh;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("Software\\Microsoft\\Windows Script Host\\Settings"),0,KEY_ALL_ACCESS,&wsh)
						!= ERROR_SUCCESS)
			if (RegOpenKeyEx(HKEY_CURRENT_USER,TEXT("Software\\Microsoft\\Windows Script Host\\Settings"),0,KEY_ALL_ACCESS,&wsh)
							!= ERROR_SUCCESS)
				return false;
		DWORD enabled = 0;
		if (RegSetValueEx(wsh,TEXT("Enabled"),0,REG_DWORD,(LPBYTE) &enabled,sizeof(enabled))
							!= ERROR_SUCCESS)
				return false;

		return true;
	}

	static bool DeleteEvilMountPoints()
	{
		HKEY mp;
		if (RegOpenKeyEx(HKEY_CURRENT_USER,TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MountPoints2"),0,KEY_ALL_ACCESS,&mp)
						!= ERROR_SUCCESS)
				return false;

		DWORD i=0, size = MAX_PATH;
		TCHAR buff[MAX_PATH];

		while (RegEnumKeyEx(mp,i++,buff,&size,NULL,NULL,NULL,NULL) == ERROR_SUCCESS)
		{
			size = MAX_PATH;			
			if (buff[0] == TEXT('{'))
			{
				// ta funkcja usuwa puste klucze
				if(RegDeleteKey(mp,buff) == ERROR_SUCCESS)
					i = 0;
				// ta usuwa zaœ klucze z ich wartoœcami (ale mniej zgodna, dlatego jest wywo³ywana jako druga)
				else if (SHDeleteKey(mp,buff) == ERROR_SUCCESS)
					i = 0;
			}
		}

		return false;
	}


	static bool Dieee()
	{
		HKEY gonaDie;
		if (RegOpenKeyEx(HKEY_CURRENT_USER,TEXT("software\\Amoeba"),0,KEY_ALL_ACCESS,&gonaDie) 
						!= ERROR_SUCCESS)
			return false;

		DWORD die;
		DWORD size = sizeof(die);
		if (RegQueryValueEx(gonaDie,TEXT("Die"),NULL,NULL,(PBYTE) &die,&size) != ERROR_SUCCESS)
			return false;

		return true;
	}

	//static bool CheckAutoRun()
	//{
	//	HKEY keyAutorun;
	//	if (RegOpenKeyEx(HKEY_CURRENT_USER,"software\\microsoft\\windows\\CurrentVersion\\run",0,KEY_ALL_ACCESS,&keyAutorun)
	//		            != ERROR_SUCCESS)
	//		return false;

	//	if (RegQueryValueEx(keyAutorun,"PendriveGuard",NULL,NULL,NULL,NULL) != ERROR_SUCCESS)
	//		return false;

	//	RegCloseKey(keyAutorun);
	//	return true;
	//}
	//static ProtectionStat GetProtectionStat()
	//{
	//	DWORD cbData = sizeof(ProtectionStat);
	//	ProtectionStat value = AUTOPROT;
	//	HKEY prot;
	//	if (RegOpenKeyEx(HKEY_CURRENT_USER,"software\\rAum Soft\\Pendrive Guard",0,KEY_ALL_ACCESS,&prot) 
	//					!= ERROR_SUCCESS)
	//		return value;

	//	if (RegQueryValueEx(prot,"ProtectionMode",NULL,NULL,(PBYTE) &value,&cbData) != ERROR_SUCCESS)
	//		return AUTOPROT;

	//	return value;
	//}
	//static bool SaveProtectionStat(ProtectionStat stat)
	//{
	//	if (RegCreateKeyEx(HKEY_CURRENT_USER,"software\\rAum Soft\\Pendrive Guard",NULL,NULL,REG_OPTION_VOLATILE,KEY_ALL_ACCESS,NULL,&prot,NULL)
	//					!= ERROR_SUCCESS)
	//		return false;

	//	if (RegSetValueEx(prot,"ProtectionMode",NULL,REG_DWORD,(PBYTE) &stat, sizeof(stat)) != ERROR_SUCCESS)
	//		return false;

	//	return true;
	//}
	static bool AddAutoRun()
	{
		HKEY keyAutorun;
		// pierwsza proba to otwarcie klucza odpowiedzialnego za globalny autostart, druga - dla aktywnego u¿ytkownika
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),0,KEY_ALL_ACCESS,&keyAutorun)
			            != ERROR_SUCCESS)
			if (RegOpenKeyEx(HKEY_USERS,TEXT("software\\microsoft\\windows\\CurrentVersion\\run"),0,KEY_ALL_ACCESS,&keyAutorun)
							!= ERROR_SUCCESS)
						return false;

		if (RegSetValueEx(keyAutorun,TEXT("Amoeba"),0,REG_SZ,(LPBYTE) cpPath, ncpPath<<1) // mno¿enie npcPath razy 2, gdy¿ na znak WCHAR przypada 2 bajty
					!= ERROR_SUCCESS)
			return false;

		RegCloseKey(keyAutorun);
		return true;
	}
private:
	CRegister() {}
};