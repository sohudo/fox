// UpdateConfig.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#include "resource.h"		// 主符号


class CUpdateConfigApp : public CWinApp
{
public:
	CUpdateConfigApp();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CUpdateConfigApp theApp;
