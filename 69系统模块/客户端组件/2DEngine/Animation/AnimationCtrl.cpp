#include "stdafx.h"
#include "AnimationCtrl.h"
#include "TimerEngine.h"

IMPLEMENT_DYNAMIC(CAnimationCtrl, CWnd)

BEGIN_MESSAGE_MAP(CAnimationCtrl, CWnd)
	ON_MESSAGE(UM_TIMER_ENGINE, OnTimerMessage)
	ON_MESSAGE(UM_TIMER_END, OnTimerEndMessage)
END_MESSAGE_MAP()

CAnimationCtrl::CAnimationCtrl()
{
	m_pITimerEngineProc = NULL;
}

CAnimationCtrl::~CAnimationCtrl()
{
}

// 设置计时器处理函数
void CAnimationCtrl::SetTimerEngineProc(ITimerEngineProc* pITimerEngineProc)
{
	m_pITimerEngineProc = pITimerEngineProc;
}

// 计时器消息
LRESULT CAnimationCtrl::OnTimerMessage(WPARAM wParam, LPARAM lParam)
{
	if (m_pITimerEngineProc)
	{
		m_pITimerEngineProc->TimerEngineProc((DWORD)wParam, 0, (tagTimerItem*)lParam);
	}

	return 0;
}

LRESULT CAnimationCtrl::OnTimerEndMessage(WPARAM wParam, LPARAM lParam)
{
	if (m_pITimerEngineProc)
	{
		m_pITimerEngineProc->TimerEngineProc((DWORD)wParam, 1, (tagTimerItem*)lParam);
	}

	return 0;
}