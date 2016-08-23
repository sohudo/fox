#ifndef DLG_USERINFO_HEAD_FILE
#define DLG_USERINFO_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
//////////////////////////////////////////////////////////////////////////////////

//关闭窗口
class CDlgUserInfo : public CDialog
{

	//界面资源
protected:
	IClientUserItem*                m_pUserData;					//用户信息
	CSkinLayered					m_SkinLayered;						//分层窗口
	CUserOrderParserHelper			m_UserOrderParserModule;
    IGameLevelParser *				m_pIGameLevelParser;				//等级接口
	TCHAR							m_szStatus[100];
	//函数定义
public:
	//构造函数
	CDlgUserInfo();
	//析构函数
	virtual ~CDlgUserInfo();

    VOID SetGameLevelParser(IGameLevelParser * pIGameLevelParser) { m_pIGameLevelParser=pIGameLevelParser; }
	void ShowUserInfo(IClientUserItem*pUserData,LPCTSTR lpszStatus=NULL);
	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//配置函数
	virtual BOOL OnInitDialog();
	//按钮消息

	//消息函数
public:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);

		//位置改变
	VOID OnWindowPosChanged(WINDOWPOS * lpWndPos);
		//显示消息
	VOID OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif