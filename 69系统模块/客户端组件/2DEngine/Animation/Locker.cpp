
#include "StdAfx.h"
#include "Locker.h"

//构造函数
CLocker::CLocker(CCriticalSection& CriticalSection, bool bLockAtOnce) 
	: m_CriticalSection(CriticalSection)
{
	// 设置变量
	m_nLockCount = 0;

	// 锁定对象
	if (bLockAtOnce == true)
	{
		Lock();
	}

	return;
}

// 析构函数
CLocker::~CLocker()
{
	// 解除锁定
	while (m_nLockCount > 0)
	{
		UnLock();
	}

	return;
}

// 锁定函数
VOID CLocker::Lock()
{
	// 锁定对象
	m_CriticalSection.Lock();

	// 设置变量
	m_nLockCount++;

	return;
}

// 解锁函数
VOID CLocker::UnLock()
{
	// 效验状态
	ASSERT(m_nLockCount > 0);
	if (m_nLockCount == 0) return;

	// 设置变量
	m_nLockCount--;

	// 解除锁定
	m_CriticalSection.Unlock();

	return;
}