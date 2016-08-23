#include "stdafx.h"

//压缩文件
#include "Compress\ZLib.h"

//链接文件
#ifndef _DEBUG
	#pragma comment(lib,"Version")
	#pragma comment(lib,"Compress\\ZLib.lib")
#else
	#pragma comment(lib,"Version")
	#pragma comment(lib,"Compress\\ZLibD.lib")
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////

//宏定义
#define XOR_TIMES					8									//加密倍数
#define MAX_SOURCE_LEN				64									//最大长度
#define MAX_ENCRYPT_LEN				(MAX_SOURCE_LEN*XOR_TIMES)			//最大长度

//密钥长度
#define ENCRYPT_KEY_LEN				8									//密钥长度

/////////////////////////////////////////////////////////////////////////////////////////////////
//进程目录
bool GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount)
{
	//模块路径
	TCHAR szModulePath[MAX_PATH]=TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(),szModulePath,CountArray(szModulePath));

	//分析文件
	for (INT i=lstrlen(szModulePath);i>=0;i--)
	{
		if (szModulePath[i]==TEXT('\\'))
		{
			szModulePath[i]=0;
			break;
		}
	}

	//设置结果
	ASSERT(szModulePath[0]!=0);
	lstrcpyn(szWorkDirectory,szModulePath,wBufferCount);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//压缩数据
ULONG CompressData(LPBYTE pcbSourceData, ULONG lSourceSize, BYTE cbResultData[], ULONG lResultSize)
{
	//压缩数据
	if (compress(cbResultData,&lResultSize,pcbSourceData,lSourceSize)==0L)
	{
		return lResultSize;
	}

	return 0L;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

//解压数据
ULONG UnCompressData(LPBYTE pcbSourceData, ULONG lSourceSize, BYTE cbResultData[], ULONG lResultSize)
{
	//解压数据
	if (uncompress(cbResultData,&lResultSize,pcbSourceData,lSourceSize)==0L)
	{
		return lResultSize;
	}

	return 0L;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

