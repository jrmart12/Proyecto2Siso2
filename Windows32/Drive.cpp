#include "stdafx.h"
#include "Drive.h"
#include <shellapi.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
Drive::Drive()
{
	_numberOfDrive = 0;
}

Drive::~Drive()
{
	delete(driveLetter);
	delete(volumeName);
	delete(driveType);
}

void Drive::getSystemDrives()
{
	TCHAR str[STRING_SIZE];
	//Standard function to fills a buffer with strings that specify valid drives in the system.
	long temp = GetLogicalDriveStrings(STRING_SIZE, str);
	//Count number of existing drive
	int i = 0;
	while (!(str[i] == '\0' && str[i + 1] == '\0'))
	{
		if (str[i] == '\0')
			_numberOfDrive++;
		i++;
	}
	_numberOfDrive++;
	driveLetter = new TCHAR*[_numberOfDrive];
	volumeName = new TCHAR*[_numberOfDrive];
	driveType = new TCHAR*[_numberOfDrive];
	icon = new int[_numberOfDrive];
	for (int i = 0; i < _numberOfDrive; i++)
	{
		driveLetter[i] = new TCHAR[STRING_SIZE];
		volumeName[i] = new TCHAR[STRING_SIZE];
		driveType[i] = new TCHAR[STRING_SIZE];
	}

	//Get drives'letter
	int index = 0, k;
	for (int j = 0; j < _numberOfDrive; j++)
	{
		k = 0;
		while (str[index] != 0)
		{
			driveLetter[j][k++] = str[index++];
		}
		driveLetter[j][k] = '\0';
		index++;
	}
	//Get label name, type and size of every single volume
	int nType;
	for (int i = 0; i < _numberOfDrive; ++i)
	{
		StrCpy(str, L"");
		//Determines whether a disk drive is a removable, fixed, CD-ROM,
		nType = GetDriveType(driveLetter[i]);
		//Assign type
		switch (nType)
		{
		case DRIVE_FIXED:
			StrCpy(driveType[i], L"Local Disk");
			break;
		case DRIVE_REMOVABLE:
			StrCpy(driveType[i], L"Removable Drive");
			break;
		case DRIVE_REMOTE:
			StrCpy(driveType[i], L"Network Drive");
			break;
		default:
			break;
		}
		//GetVolumeInformation(driveLetter[i], str, STRING_SIZE, NULL, NULL, NULL, NULL, 0);
		StrCpy(volumeName[i], str);

		if (!StrCmp(volumeName[i], L"DATA"))
			icon[i] = IDI_DATA;
		else if (!StrCmp(volumeName[i], L"OS"))
			icon[i] = IDI_OS;

		StrCat(volumeName[i], driveLetter[i]);
	}
}
