// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 项目特定的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 从 Windows 标头中排除不常使用的资料
#endif

// 如果您必须使用下列所指定的平台之前的平台，则修改下面的定义。
// 有关不同平台的相应值的最新信息，请参考 MSDN。
#ifndef WINVER				// 允许使用 Windows 95 和 Windows NT 4 或更高版本的特定功能。
#define WINVER 0x601		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_WINNT		// 允许使用 Windows NT 4 或更高版本的特定功能。
#define _WIN32_WINNT 0x601		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用 Windows 98 或更高版本的特定功能。
#define _WIN32_WINDOWS 0x0410 //为 Windows Me 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_IE			// 允许使用 IE 4.0 或更高版本的特定功能。
#define _WIN32_IE 0x0400	//为 IE 5.0 及更新版本改变为适当的值。
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常被安全忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心和标准组件
#include <afxext.h>         // MFC 扩展
#include <afxdisp.h>        // MFC 自动化类

#include <afxdtctl.h>		// Internet Explorer 4 公共控件的 MFC 支持
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Windows 公共控件的 MFC 支持
#endif // _AFX_NO_AFXCMN_SUPPORT

//平台文件
#include "..\..\全局定义\Platform.h"

//组件文件
#include "..\..\公共组件\服务核心\ServiceCoreHead.h"
//组件文件
#include "..\..\客户端组件\图像控件\WHImageHead.h"
#include "..\..\客户端组件\形象组件\AvatarControlHead.h"


//////////////////////////////////////////////////////////////////////////////////
//链接代码

//多媒体库

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../链接库/Ansi/WHImage.lib")
#pragma comment (lib,"../../链接库/Ansi/ServiceCore.lib")
#pragma comment (lib,"../../链接库/Ansi/AvatarControl.lib")
#else
#pragma comment (lib,"../../链接库/Unicode/WHImage.lib")
#pragma comment (lib,"../../链接库/Unicode/ServiceCore.lib")
#pragma comment (lib,"../../链接库/Unicode/AvatarControl.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../链接库/Ansi/WHImageD.lib")
#pragma comment (lib,"../../链接库/Ansi/DownLoadD.lib")
#pragma comment (lib,"../../链接库/Ansi/ServiceCoreD.lib")
#pragma comment (lib,"../../链接库/Ansi/SkinControlD.lib")
#pragma comment (lib,"../../链接库/Ansi/GamePropertyD.lib")
#pragma comment (lib,"../../链接库/Ansi/ShareControlD.lib")
#pragma comment (lib,"../../链接库/Ansi/PlatformDataD.lib")
#pragma comment (lib,"../../链接库/Ansi/AvatarControlD.lib")
#pragma comment (lib,"../../链接库/Ansi/GameEngineD.lib")
#else
#pragma comment (lib,"../../链接库/Unicode/WHImageD.lib")
#pragma comment (lib,"../../链接库/Unicode/DownLoadD.lib")
#pragma comment (lib,"../../链接库/Unicode/ServiceCoreD.lib")
#pragma comment (lib,"../../链接库/Unicode/SkinControlD.lib")
#pragma comment (lib,"../../链接库/Unicode/GamePropertyD.lib")
#pragma comment (lib,"../../链接库/Unicode/ShareControlD.lib")
#pragma comment (lib,"../../链接库/Unicode/PlatformDataD.lib")
#pragma comment (lib,"../../链接库/Unicode/AvatarControlD.lib")
#pragma comment (lib,"../../链接库/Unicode/GameEngineD.lib")
#endif
#endif