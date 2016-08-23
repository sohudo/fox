
#ifndef ANIMATION_CTRL_H_
#define ANIMATION_CTRL_H_
#pragma once

// 前置声明
struct tagTimerItem;

// 计时器引擎消息处理
class ITimerEngineProc
{
public:
	virtual void TimerEngineProc(DWORD dwTimerID, BYTE cbTimerEnd, tagTimerItem* pTimerItem) = 0;
};

// 动画控件,因为如果使用主界面的消息队列可能会造成消息同步问题,也可以不占用主界面的消息队列,如果对同步没什么要求的话可以直接发消息到主界面,不使用此类.
// 实现此类专门用来接收TimerEngine发出的消息,这样实现的话基本不会出现同步的问题,这样可以避免在TimerEngine::OnTimerThreadSink里
// PostMessage之前频繁的new,在另一地方频繁的delete的同步实现.
// 使用方法(主界面):
// 继承ITimerEngineProc, 定义CAnimationCtrl m_AnimationCtrl;
// m_AnimationCtrl.Create(NULL, NULL, WS_CHILD, CRect(0, 0, 0, 0), this, 100);
// m_AnimationCtrl.SetTimerEngineProc(this);
// TimerEngineProc函数里具体实现
class CAnimationCtrl : public CWnd
{
	DECLARE_DYNAMIC(CAnimationCtrl)

public:
	CAnimationCtrl();
	virtual ~CAnimationCtrl();

public:
	// 设置计时器处理函数
	void SetTimerEngineProc(ITimerEngineProc* pITimerEngineProc);

protected:
	// 计时器消息
	LRESULT OnTimerMessage(WPARAM wParam, LPARAM lParam);
	LRESULT OnTimerEndMessage(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:
	ITimerEngineProc* m_pITimerEngineProc;
};

#endif // ANIMATION_CTRL_H_


