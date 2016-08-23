#ifndef WND_LAST_PLAY_HEAD_FILE
#define WND_LAST_PLAY_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

struct tagLastGameItem
{
	CRect rcArea;
	bool  bActive;
	WORD  wServerID;
};


//////////////////////////////////////////////////////////////////////////////////

//用户信息
class CWndLastGamePlay : public CWnd
{
	//状态变量
protected:
	bool							m_bClickFace;						//点击标志
	bool							m_bMouseEvent;						//注册标志

	tagLastGameItem					m_LastGame[3];

	CLastGameServerList             m_LastPlayGameList;                 //最近玩过游戏

	//盘旋标志
protected:
	bool							m_bHoverFace;						//盘旋标志

	CBitImage						m_ImgType;
	CPngImage						m_ImgArrow;

	CFont							m_FontBold;							//粗体字体

	//区域位置
protected:
	CRect							m_rcFaceArea;						//头像区域

	//函数定义
public:
	//构造函数
	CWndLastGamePlay();
	//析构函数
	virtual ~CWndLastGamePlay();

	//消息函数
protected:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//创建消息
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	VOID OnLButtonUp(UINT nFlags, CPoint MousePoint);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint MousePoint);
	//光标消息
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	//游戏记录
public:
	//加载记录
	void LoadLastPlayGame();
	//保存记录
	void SaveLastPlayGame();
	//添加最后游戏
	void AddLastPlayGame(WORD wServerID);
	//
	VOID GetGameResDirectory(TCHAR szProcessName[], TCHAR szResDirectory[], WORD wBufferCount);

	//自定消息
protected:
	//鼠标离开
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif