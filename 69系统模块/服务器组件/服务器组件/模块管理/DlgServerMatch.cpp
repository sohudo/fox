#include "Stdafx.h"
#include "Resource.h"
#include "DlgServerMatch.h"
#include ".\dlgservermatch.h"

//构造函数
CDlgServerMatch::CDlgServerMatch() : CDialog(IDD_SERVER_MATCH)
{
	//模块参数
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	return;
}

//析构函数
CDlgServerMatch::~CDlgServerMatch()
{
}
void CDlgServerMatch::DoDataExchange(CDataExchange* pDX)
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::DoDataExchange(pDX);
}
//打开比赛对话框
bool CDlgServerMatch::OpenGameMatch()
{
	//设置资源
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//配置房间
	if (DoModal()==IDC_LOAD_SERVER)
	{
		return true;
	}

	return false;
}
