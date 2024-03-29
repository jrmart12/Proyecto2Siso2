#include "stdafx.h"
#include "sizeDriver.h"
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#define KB 1
#define MB 2
#define GB 3
#define TB 4
#define RADIX 10

sizeDriver::~sizeDriver()
{
}

sizeDriver::sizeDriver(__int64 totalSize, __int64 freeSpace)
{
	mTotalSize = totalSize;
	mFreeSpace = freeSpace;
}

LPWSTR sizeDriver::getTotalSize()
{
	return convertByteToStringSize(mTotalSize);
}

LPWSTR sizeDriver::getFreeSpace()
{
	return convertByteToStringSize(mFreeSpace);
}

LPWSTR sizeDriver::convertByteToStringSize(__int64 nSize)
{
	int nType = 0; //Bytes

	while (nSize >= 1048576)
	{
		nSize /= 1024;
		++nType;
	}

	__int64 nRight;

	if (nSize >= 1024)
	{
		//Get a digit after '.' of nSize in nRight
		nRight = nSize % 1024;

		while (nRight > 99)
			nRight /= 10;

		nSize /= 1024;
		++nType;
	}
	else
		nRight = 0;

	TCHAR *buffer = new TCHAR[11];
	_itow_s(nSize, buffer, 11, RADIX);

	if (nRight != 0 && nType > KB)
	{
		StrCat(buffer, _T("."));
		TCHAR *right = new TCHAR[3];
		_itow_s(nRight, right, 3, RADIX);
		StrCat(buffer, right);
	}

	switch (nType)
	{
	case 0://Bytes
		StrCat(buffer, _T(" bytes"));
		break;
	case KB:
		StrCat(buffer, _T(" KB"));
		break;
	case MB:
		StrCat(buffer, _T(" MB"));
		break;
	case GB:
		StrCat(buffer, _T(" GB"));
		break;
	case TB:
		StrCat(buffer, _T(" TB"));
		break;
	}

	return buffer;
}