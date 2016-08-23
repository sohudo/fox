#pragma once
#include "Stdafx.h"

class CSkinEditEx2 :
	public CEdit
{
public:
	CSkinEditEx2(void);
	~CSkinEditEx2(void);
	//消息函数 
protected:
	//重画消息
	VOID OnNcPaint();
	//绘画控件
	HBRUSH CtlColor(CDC * pDC, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};
