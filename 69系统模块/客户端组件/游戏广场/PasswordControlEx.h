#ifndef PASSWORD_CONTROLEXx_HEAD_FILE
#define PASSWORD_CONTROLEXx_HEAD_FILE

#pragma once


//密码编辑
class CPasswordControlEx : public CPasswordControl
{

	//函数定义
public:
	//构造函数
	CPasswordControlEx();
	//析构函数
	virtual ~CPasswordControlEx();

	//重载函数
protected:
	//绑定函数
	virtual VOID PreSubclassWindow();

	afx_msg VOID OnSize(UINT nType, INT cx, INT cy);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif