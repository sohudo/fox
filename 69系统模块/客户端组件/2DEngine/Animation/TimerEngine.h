
#ifndef TIMER_ENGINE_H_
#define TIMER_ENGINE_H_
#pragma once

#include <afxtempl.h>
#include "Thread.h"

#define TIMES_INFINITY		((DWORD)-1) // 无限次数
#define UM_TIMER_ENGINE		(0x7FEE)		// 定时器消息 (WM_APP(0x8000) -- 0xBFFF)
#define UM_TIMER_END		(0x7FEF)		// 

// 类声明
class CTimerEngine;

// 定时器线程
class CTimerThread : public CThread
{
public:
	// 构造函数
	CTimerThread(void);
	// 析构函数
	virtual ~CTimerThread(void);

	// 功能函数
public:
	// 配置函数
	bool InitThread(CTimerEngine* pTimerEngine, DWORD dwTimerSpace);

	// 重载函数
private:
	// 运行函数
	virtual bool OnEventThreadRun();

	// 变量定义
private:
	DWORD m_dwTimerSpace; // 时间间隔
	CTimerEngine* m_pTimerEngine; // 定时器引擎
};

//////////////////////////////////////////////////////////////////////////

// 定时器子项
struct tagTimerItem
{
	DWORD dwTimerID; // 定时器 ID
	DWORD dwElapse; // 定时时间
	DWORD dwTimeLeave; // 倒计时间
	DWORD dwRepeatTimes; // 重复次数
	DWORD dwStartTime; //  开始时间
	DWORD dwCurrentTime; // 当前时间
	WPARAM wBindParam; // 绑定参数
};

//  定时器子项数组定义
typedef CArray<tagTimerItem*> CTimerItemPtr;

//  定时器引擎
class CTimerEngine
{
	friend class CTimerThread;

public:
	// 构造函数
	CTimerEngine(void);
	// 析构函数
	virtual ~CTimerEngine(void);

	// 接口函数
public:
	// 设置定时器
	bool SetTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeatTimes, WPARAM wParam);
	// 删除定时器
	bool KillTimer(DWORD dwTimerID);
	// 删除定时器
	bool KillAllTimer();

	// 管理接口
public:
	// 开始服务
	bool InitTimerEngine(CWnd* pSinkWnd);
	// 停止服务
	bool UninitTimerEngine();

	// 内部函数
private:
	// 定时器通知
	void OnTimerThreadSink();

	// 配置定义
private:
	DWORD m_dwTimerSpace; // 时间间隔

	// 状态变量
private:
	bool m_bService; // 运行标志
	DWORD m_dwTimePass; // 经过时间
	DWORD m_dwTimeLeave; // 倒计时间
	CTimerItemPtr m_TimerItemFree; // 空闲数组
	CTimerItemPtr m_TimerItemActive; // 活动数组

	// 组件变量
private:
	CCriticalSection m_CriticalSection; // 线程锁定
	CTimerThread m_TimerThread; // 定时器线程
	CWnd* m_pSinkWnd; // 通知窗口
};

#endif // TIMER_ENGINE_H_