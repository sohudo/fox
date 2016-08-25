#pragma once

//////////////////////////////////////////////////////////////////////////////////

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WINVER
#define WINVER 0x601
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x601
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0400
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0400
#endif

#define _ATL_ATTRIBUTES
#define _AFX_ALL_WARNINGS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

//////////////////////////////////////////////////////////////////////////////////

//MFC 文件
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <AfxDisp.h>
#include <afxmt.h>

//MFC 文件
#include <Nb30.h>
#include <AfxInet.h>
#include <GdiPlus.h>
#include <AtlImage.h>

//命名空间
using namespace Gdiplus;

//////////////////////////////////////////////////////////////////////////////////

//平台文件
#include "..\..\全局定义\Platform.h"

//控件文件
#include "PngImage\PngImage.h"

//////////////////////////////////////////////////////////////////////////////////
//函数声明

//进程目录
bool GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount);

//压缩数据
ULONG CompressData(LPBYTE pcbSourceData, ULONG lSourceSize, BYTE cbResultData[], ULONG lResultSize);

//解压数据
ULONG UnCompressData(LPBYTE pcbSourceData, ULONG lSourceSize, BYTE cbResultData[], ULONG lResultSize);

//////////////////////////////////////////////////////////////////////////////////
