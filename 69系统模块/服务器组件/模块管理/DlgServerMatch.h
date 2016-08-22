//#ifndef DLG_SERVER_MATCH_HEAD_FILE
//#define DLG_SERVER_MATCH_HEAD_FILE

#pragma once

#include "ModuleManagerHead.h"
#include "ServerInfoManager.h"
#include "ServerListControl.h"

//主对话框
class MODULE_MANAGER_CLASS CDlgServerMatch : public CDialog
{
	//配置变量
public:
	tagModuleInitParameter			m_ModuleInitParameter;				//配置参数

	//函数定义
public:
	//构造函数
	CDlgServerMatch();
	//析构函数
	virtual ~CDlgServerMatch();
	//配置函数
public:
	//打开比赛对话框
	bool OpenGameMatch();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
};