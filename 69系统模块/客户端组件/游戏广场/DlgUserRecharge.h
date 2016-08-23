#ifndef DLG_USER_RECHARGE_HEAD_FILE
#define DLG_USER_RECHARGE_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//锁机设置
class CDlgUserRecharge : public CDialog, public CMissionItem
{
	//变量定义
protected:
	BYTE							m_cbMachine;						//绑定标志
	TCHAR							m_szID[LEN_ACCOUNTS];		                    	//用户ID
    TCHAR							m_szCard[LEN_PASSWORD];			//卡号
	TCHAR							m_szPassword[LEN_PASSWORD];			//用户密码
	//控件变量
protected:
	CSkinButton						m_btOk;								//确定按钮
	CSkinButton						m_btCancel;							//取消按钮
	
	CEdit							m_IDControl;					     //用户ID
    CEdit							m_CardControl;					     //卡号
	CEdit							m_PasswordControl;					 //用户密码
	CPngImage						m_ImageBack;

	//组件变量
protected:
	CMissionManager					m_MissionManager;					//任务管理
    CSkinLayered					m_SkinLayered;						//分层窗口
	//函数定义
public:
	//构造函数
	CDlgUserRecharge();
	//析够函数
	virtual ~CDlgUserRecharge();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//创建函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK();

	//重载函数
public:
	//连接事件
	virtual bool OnEventMissionLink(INT nErrorCode);
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//读取事件
	virtual bool OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize);

public:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//显示消息
	VOID OnShowWindow(BOOL bShow, UINT nStatus);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//位置改变
	VOID OnWindowPosChanged(WINDOWPOS * lpWndPos);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif