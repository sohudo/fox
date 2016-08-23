#include "StdAfx.h"
#include "TimerEngine.h"
#include "Locker.h"

// 宏定义
#define NO_TIME_LEFT	DWORD(-1)	// 不响应时间

// 构造函数
CTimerThread::CTimerThread()
{
	m_dwTimerSpace = 0L;
	m_pTimerEngine = NULL;
}

// 析构函数
CTimerThread::~CTimerThread()
{
}

// 配置函数
bool CTimerThread::InitThread(CTimerEngine* pTimerEngine, DWORD dwTimerSpace)
{
	// 效验参数
	ASSERT(dwTimerSpace >= 10L);
	ASSERT(pTimerEngine != NULL);
	if (pTimerEngine == NULL) return false;

	// 设置变量
	m_dwTimerSpace = dwTimerSpace;
	m_pTimerEngine = pTimerEngine;

	return true;
}

// 运行函数
bool CTimerThread::OnEventThreadRun()
{
	ASSERT(m_pTimerEngine != NULL);
	Sleep(m_dwTimerSpace);
	m_pTimerEngine->OnTimerThreadSink();
	return true;
}

//////////////////////////////////////////////////////////////////////////

// 构造函数
CTimerEngine::CTimerEngine(void)
{
	m_bService = false;
	m_dwTimePass = 0L;
	m_dwTimerSpace = 25L;
	m_dwTimeLeave = NO_TIME_LEFT;
	m_pSinkWnd = NULL;
}

// 析构函数
CTimerEngine::~CTimerEngine(void)
{
	// 停止服务
	UninitTimerEngine();

	// 清理内存
	tagTimerItem* pTimerItem = NULL;
	for (INT_PTR i = 0; i < m_TimerItemFree.GetCount(); i++)
	{
		pTimerItem = m_TimerItemFree[i];
		ASSERT(pTimerItem != NULL);
		if (pTimerItem != NULL)
		{
			delete pTimerItem;
			pTimerItem = NULL;
		}
	}
	for (INT_PTR i = 0; i < m_TimerItemActive.GetCount(); i++)
	{
		pTimerItem = m_TimerItemActive[i];
		ASSERT(pTimerItem != NULL);
		if (pTimerItem != NULL)
		{
			delete pTimerItem;
			pTimerItem = NULL;
		}
	}
	m_TimerItemFree.RemoveAll();
	m_TimerItemActive.RemoveAll();

	return;
}

// 设置定时器
bool CTimerEngine::SetTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeatTimes, WPARAM wParam)
{
	// 锁定资源
	CLocker Locker(m_CriticalSection);

	// 效验参数
	ASSERT(dwRepeatTimes > 0L);
	if (dwRepeatTimes == 0) return false;
	dwElapse = (dwElapse + m_dwTimerSpace - 1) / m_dwTimerSpace * m_dwTimerSpace;

	// 查找定时器
	bool bTimerExist = false;
	tagTimerItem* pTimerItem = NULL;
	for (INT_PTR i = 0; i < m_TimerItemActive.GetCount(); i++)
	{
		pTimerItem = m_TimerItemActive[i];
		ASSERT(pTimerItem != NULL);
		if (pTimerItem->dwTimerID == dwTimerID) 
		{
			bTimerExist = true;
			break;
		}
	}

	// 创建定时器
	if (bTimerExist == false)
	{
		INT_PTR nFreeCount = m_TimerItemFree.GetCount();
		if (nFreeCount > 0)
		{
			pTimerItem = m_TimerItemFree[nFreeCount - 1];
			ASSERT(pTimerItem != NULL);
			m_TimerItemFree.RemoveAt(nFreeCount - 1);
		}
		else
		{
			try
			{
				pTimerItem = new tagTimerItem;
				ASSERT(pTimerItem != NULL);
				if (pTimerItem == NULL) return false;
			}
			catch (...) 
			{ 
				return false; 
			}
		}
	}

	// 设置参数
	ASSERT(pTimerItem != NULL);
	pTimerItem->dwTimerID = dwTimerID;
	pTimerItem->wBindParam = wParam;
	pTimerItem->dwElapse = dwElapse;
	pTimerItem->dwRepeatTimes = dwRepeatTimes;
	pTimerItem->dwTimeLeave = dwElapse + m_dwTimePass;
	pTimerItem->dwStartTime = pTimerItem->dwCurrentTime = GetTickCount();

	// 激活定时器
	m_dwTimeLeave = min(m_dwTimeLeave, dwElapse);
	if (bTimerExist == false) m_TimerItemActive.Add(pTimerItem);

	return false;
}

// 删除定时器
bool CTimerEngine::KillTimer(DWORD dwTimerID)
{
	// 锁定资源
	CLocker Locker(m_CriticalSection);

	// 查找定时器
	tagTimerItem* pTimerItem = NULL;
	for (INT_PTR i = 0; i < m_TimerItemActive.GetCount(); i++)
	{
		pTimerItem = m_TimerItemActive[i];
		ASSERT(pTimerItem != NULL);
		if (pTimerItem->dwTimerID == dwTimerID) 
		{
			m_TimerItemActive.RemoveAt(i);
			m_TimerItemFree.Add(pTimerItem);
			if (m_TimerItemActive.GetCount() == 0) 
			{
				m_dwTimePass = 0L;
				m_dwTimeLeave = NO_TIME_LEFT;
			}
			return true;;
		}
	}

	return false;
}

// 删除定时器
bool CTimerEngine::KillAllTimer()
{
	// 锁定资源
	CLocker Locker(m_CriticalSection);

	// 删除定时器
	m_TimerItemFree.Append(m_TimerItemActive);
	m_TimerItemActive.RemoveAll();

	// 设置变量
	m_dwTimePass = 0L;
	m_dwTimeLeave = NO_TIME_LEFT;

	return true;
}

// 开始服务
bool CTimerEngine::InitTimerEngine(CWnd* pSinkWnd)
{
	// 效验状态
	if (m_bService==true) 
	{
		return true;
	}

	// 判断绑定
	if (pSinkWnd == NULL)
	{
		return false;
	}
	m_pSinkWnd = pSinkWnd;

	// 设置变量
	m_dwTimePass = 0L;
	m_dwTimeLeave = NO_TIME_LEFT;
	if (m_TimerThread.InitThread(this, m_dwTimerSpace) == false)
	{
		return false;
	}

	// 启动服务
	if (m_TimerThread.StartThread() == false)
	{
		return false;
	}

	// 设置变量
	m_bService = true;

	return true;
}

// 停止服务
bool CTimerEngine::UninitTimerEngine()
{
	// 设置变量
	m_bService = false;
	m_pSinkWnd = NULL;

	// 停止线程
	m_TimerThread.ConcludeThread(INFINITE);

	// 设置变量
	m_dwTimePass = 0L;
	m_dwTimeLeave = NO_TIME_LEFT;
	m_TimerItemFree.Append(m_TimerItemActive);
	m_TimerItemActive.RemoveAll();

	return true;
}

// 定时器通知
void CTimerEngine::OnTimerThreadSink()
{
	// 锁定资源
	CLocker Locker(m_CriticalSection);

	// 倒计时间
	if (m_dwTimeLeave == NO_TIME_LEFT) 
	{
		ASSERT(m_TimerItemActive.GetCount() == 0);
		return;
	}

	// 减少时间
	ASSERT(m_dwTimeLeave >= m_dwTimerSpace);
	m_dwTimeLeave -= m_dwTimerSpace;
	m_dwTimePass += m_dwTimerSpace;

	// 查询定时器
	if (m_dwTimeLeave == 0)
	{
		bool bKillTimer = false;
		tagTimerItem* pTimerItem = NULL;
		DWORD dwTimeLeave = NO_TIME_LEFT;
		for (INT_PTR i = 0; i < m_TimerItemActive.GetCount() ;)
		{
			// 效验参数
			pTimerItem = m_TimerItemActive[i];
			ASSERT(pTimerItem != NULL);
			ASSERT(pTimerItem->dwTimeLeave >= m_dwTimePass);

			// 定时器处理
			bKillTimer = false;
			pTimerItem->dwTimeLeave -= m_dwTimePass;
			if (pTimerItem->dwTimeLeave == 0L)
			{
				// 发送通知
				if (m_pSinkWnd != NULL && m_pSinkWnd->GetSafeHwnd() != NULL)
				{
					// 更新当前时间
					pTimerItem->dwCurrentTime = GetTickCount();
                    // (LPARAM)pTimerItem这个可能会有同步问题,如果有同步要求可以使用CAnimationCtrl接收消息,不去占用主线程的消息队列.
                    m_pSinkWnd->PostMessage(UM_TIMER_ENGINE, (WPARAM)pTimerItem->dwTimerID, (LPARAM)pTimerItem);
				}

				// 设置次数
				if (pTimerItem->dwRepeatTimes != TIMES_INFINITY)
				{
					ASSERT(pTimerItem->dwRepeatTimes > 0L);
					if (pTimerItem->dwRepeatTimes == 1L)
					{
						bKillTimer = true;
						m_TimerItemActive.RemoveAt(i);
						m_TimerItemFree.Add(pTimerItem);
						m_pSinkWnd->PostMessage(UM_TIMER_END, (WPARAM)pTimerItem->dwTimerID, (LPARAM)pTimerItem);
					}
					else pTimerItem->dwRepeatTimes--;
				}

				// 设置时间
				if (bKillTimer == false) pTimerItem->dwTimeLeave = pTimerItem->dwElapse;
			}

			// 增加索引
			if (bKillTimer == false) 
			{
				i++;
				dwTimeLeave = min(dwTimeLeave, pTimerItem->dwTimeLeave);
				ASSERT(dwTimeLeave % m_dwTimerSpace == 0);
			}
		}

		// 设置响应
		m_dwTimePass = 0L;
		m_dwTimeLeave = dwTimeLeave;
	}

	return;
}
