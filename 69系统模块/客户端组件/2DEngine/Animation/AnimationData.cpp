
#include "StdAfx.h"
#include "AnimationData.h"
#include <math.h>

// 构造函数
CAnimationData::CAnimationData()
{
	m_dwDuration = 200; // 持续时间
	m_ptStartPos.SetPoint(0, 0);  // 起始位置
	m_ptTargetPos.SetPoint(0, 0);	// 目标位置
	m_TweenType = Tween::LINEAR; // 补间类型
	m_bReverse = false;
	m_bAnimationEnd = false;
}

// 析造函数
CAnimationData::~CAnimationData()
{

}

// 初始数据
void CAnimationData::InitAnimationData(DWORD dwDuration, const CPoint& ptStartPos, const CPoint& ptTargetPos, Tween::Type type)
{
	ASSERT(dwDuration > 0);
	if (dwDuration > 0) m_dwDuration = dwDuration;
	m_ptStartPos.SetPoint(ptStartPos.x, ptStartPos.y);
	m_ptTargetPos.SetPoint(ptTargetPos.x, ptTargetPos.y);
	m_TweenType = type;
	m_bReverse = false;
}

// 设置持续时间
void CAnimationData::SetDuration(DWORD dwDuration)
{
	m_dwDuration = dwDuration;
}

// 设置起始位置
void CAnimationData::SetStartPos(const CPoint& ptStartPos)
{
	m_ptStartPos.SetPoint(ptStartPos.x, ptStartPos.y);
}

// 设置目标位置
void CAnimationData::SetTargetPos(const CPoint& ptTargetPos)
{
	m_ptTargetPos.SetPoint(ptTargetPos.x, ptTargetPos.y);
}

// 设置补间类型
void CAnimationData::SetTweenType(Tween::Type type)
{
	m_TweenType = type;
}

// 设置逆向
void CAnimationData::SetReverse(bool bReverse)
{
	m_bReverse = bReverse;
}

// 重置
void CAnimationData::ResetAnimationData()
{
	m_bAnimationEnd = false;
}

// 得到当前位置
CPoint CAnimationData::GetCurrentPos(DWORD dwStartTime, DWORD dwCurrentTime)
{
	ASSERT(dwCurrentTime >= dwStartTime);
	DWORD dwElapse = dwCurrentTime - dwStartTime;
	double state = (double)dwElapse / (double)m_dwDuration;
	if (m_TweenType == Tween::EASE_OUT_SNAP && fabs(1.0 - state) <= 0.06) state = 1.0;
	if (state >= 1.0)
	{
		state = 1.0;
		m_bAnimationEnd = true;
	}
	double dValue = Tween::CalculateValue(m_TweenType, state);
	if (m_bReverse)
	{
		int nXPos = Tween::ValueBetween(dValue, m_ptTargetPos.x, m_ptStartPos.x);
		int nYPos = Tween::ValueBetween(dValue, m_ptTargetPos.y, m_ptStartPos.y);
		CPoint ptCurrentPos = CPoint(nXPos, nYPos);
		return ptCurrentPos;
	}
	else
	{
		int nXPos = Tween::ValueBetween(dValue, m_ptStartPos.x, m_ptTargetPos.x);
		int nYPos = Tween::ValueBetween(dValue, m_ptStartPos.y, m_ptTargetPos.y);
		CPoint ptCurrentPos = CPoint(nXPos, nYPos);
		return ptCurrentPos;
	}
}
