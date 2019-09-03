#pragma once
class sizeDriver
{
private:
	__int64 mTotalSize;
	__int64 mFreeSpace;


public:
	sizeDriver();
	sizeDriver(__int64 totalSize, __int64 freeSpace);
	~sizeDriver();

	static LPWSTR convertByteToStringSize(__int64 nSize);

	LPWSTR getTotalSize();
	LPWSTR getFreeSpace();

};