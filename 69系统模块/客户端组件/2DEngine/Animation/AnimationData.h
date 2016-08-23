
#ifndef ANIMATION_DATA_H_
#define ANIMATION_DATA_H_
#pragma once

#include "tween.h"

// 动画数据
class CAnimationData
{
public:
	// 构造函数
	CAnimationData();
	// 析造函数
	~CAnimationData();

	// 配置函数
public:
	// 初始数据
	void InitAnimationData(DWORD dwDuration, const CPoint& ptStartPos, const CPoint& ptTargetPos, Tween::Type type = Tween::LINEAR);
	// 设置持续时间
	void SetDuration(DWORD dwDuration);
	// 设置起始位置
	void SetStartPos(const CPoint& ptStartPos);
	// 设置目标位置
	void SetTargetPos(const CPoint& ptTargetPos);
	// 设置补间类型
	void SetTweenType(Tween::Type type);
	// 设置逆向
	void SetReverse(bool bReverse);
	// 重置
	void ResetAnimationData();

	// 查询函数
public:
	// 得到当前位置
	CPoint GetCurrentPos(DWORD dwStartTime, DWORD dwCurrentTime);
	// 动画结束
	bool IsAnimationEnd() { return m_bAnimationEnd; }
	// 得到起始位置
	const CPoint& GetStartPos() const { return m_ptStartPos; }
	// 得到目标位置
	const CPoint& GetTargetPos() const { return m_ptTargetPos; }
	// 是否逆向
	bool IsReverse() { return m_bReverse; }

private:
	DWORD m_dwDuration; // 持续时间
	CPoint m_ptStartPos; // 起始位置
	CPoint m_ptTargetPos; // 目标位置
	Tween::Type m_TweenType; // 补间类型
	bool m_bReverse; // 逆向(目标位置到起始位置)
	bool m_bAnimationEnd; // 动画结束
};

#endif // ANIMATION_DATA_H_
