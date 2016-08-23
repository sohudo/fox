#ifndef PASSWORD_CONTROLEX_HEAD_FILE
#define PASSWORD_CONTROLEX_HEAD_FILE

#pragma once

#include "ShareControlHead.h"

interface IKeyBoardSink
{
	virtual VOID OnKeyBoardChar(WORD wChar)=NULL;
	virtual VOID OnKeyBoardClean()=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

//密码键盘
class SHARE_CONTROL_CLASS CPasswordKeyboard2 : public CDialog
{
	//配置变量
protected:
	INT								m_nRandLine[3];						//随机数字
	INT								m_nItemCount[3];					//字符数目
	TCHAR							m_szKeyboradChar[3][2][15];			//键盘字符

	//键盘状态
protected:
	bool							m_bCapsLockStatus;					//按键状态

	//状态变量
protected:
	WORD							m_wHoverRow;						//盘旋列数
	WORD							m_wHoverLine;						//盘旋行数
	bool							m_bMouseDown;						//按下状态

	IKeyBoardSink*					m_pKeyBoardSink;

	//资源大小
protected:
	CSize							m_SizeImageItem1;					//资源大小
	CSize							m_SizeImageItem2;					//资源大小
	CSize							m_SizeBackGround;					//背景大小

	//函数定义
public:
	//构造函数
	CPasswordKeyboard2();
	//析构函数
	virtual ~CPasswordKeyboard2();

	VOID SetKeyBoardSink(IKeyBoardSink* pSink);

	//重载函数
protected:
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//创建函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK() { return; }
	//取消消息
	virtual VOID OnCancel() { return; }

	//辅助函数
protected:
	//更新位置
	VOID SetCurrentStation(CPoint MousePoint);
	//虚拟编码
	WORD GetVirualKeyCode(WORD wHoverLine, WORD wHoverRow);

	//消息定义
protected:
	//界面绘画
	VOID OnNcPaint();
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//失去焦点
	VOID OnKillFocus(CWnd * pNewWnd);
	//鼠标消息
	VOID OnLButtonUp(UINT nFlags, CPoint Point);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//光标消息
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif