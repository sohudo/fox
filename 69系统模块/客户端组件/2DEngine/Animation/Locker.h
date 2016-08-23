
#ifndef LOCKER_H_
#define LOCKER_H_
#pragma once

// 数据锁定
class CLocker
{
	// 函数定义
public:
	// 构造函数
	CLocker(CCriticalSection& CriticalSection, bool bLockAtOnce = true);
	// 析构函数
	virtual ~CLocker();

	// 操作函数
public:
	// 锁定函数
	VOID Lock();
	// 解锁函数 
	VOID UnLock();

	// 状态函数
public:
	// 锁定次数
	inline INT GetLockCount() { return m_nLockCount; }

	// 变量定义
private:
	INT m_nLockCount; // 锁定计数
	CCriticalSection& m_CriticalSection; // 锁定对象
};

#endif // LOCKER_H_