#include "StdAfx.h"
#include "Resource.h"
#include "Information.h"
#include "DlgInsureCounter.h"
#include "UserItemElement.h"

//////////////////////////////////////////////////////////////////////////////////
#define IDC_SCORE_LINK				100
#define IDC_RECORD_DETAIL           101


//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgInsureSave, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_EN_CHANGE(IDC_SCORE, OnEnChangeScore)
	ON_BN_CLICKED(IDC_BT_SAVE_SCORE, OnBnClickedSaveScore)
	ON_BN_CLICKED(IDC_BT_TAKE_SCORE, OnBnClickedTakeScore)
	ON_STN_CLICKED(IDC_FORGET_INSURE, OnBnClickedForgetInsure)
	ON_COMMAND_RANGE(IDC_SCORE_LINK,IDC_SCORE_LINK+7,OnBnClickedScore)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgInsureRecord, CDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(1002, OnBnClickedQueryIn)
	ON_BN_CLICKED(1001, OnBnClickedQueryOut)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


BEGIN_MESSAGE_MAP(CDlgInsurePassword, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BT_SAVE_SCORE, OnBnClickedSaveScore)
	ON_BN_CLICKED(IDC_BT_TAKE_SCORE, OnBnClickedTakeScore)
END_MESSAGE_MAP()


BEGIN_MESSAGE_MAP(CDlgInsureTransfer, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_EN_CHANGE(IDC_SCORE, OnEnChangeScore)
	ON_EN_CHANGE(IDC_NICKNAME,OnEnChangeNick)
	ON_EN_KILLFOCUS(IDC_NICKNAME,OnEnKillFocus)
	ON_STN_CLICKED(IDC_FORGET_INSURE, OnBnClickedForgetInsure)
	ON_BN_CLICKED(IDC_BT_TRANSFER_SCORE, OnBnClickedTransferScore)
	ON_BN_CLICKED(IDC_BY_ID, OnBnClickedById)
	ON_BN_CLICKED(IDC_BY_NAME, OnBnClickedByName)
	ON_COMMAND_RANGE(IDC_SCORE_LINK,IDC_SCORE_LINK+5,OnBnClickedScore)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgInsureMain, CDialog)
	//ON_BN_CLICKED(IDC_BT_QUERY_INFO, OnBnClickedQueryInfo)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BT_QUERY_INFO, OnCancel)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CONTROL, OnTcnSelchangeInsureMode)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgTransferResult, CSkinDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

#define WM_INSUREPLAZA_UPDATA      (WM_USER+1680)             //银行更新消息

struct tagInsurePlazaUpdata
{
	SCORE  lUserScore;
	SCORE  lUserInsure;
};

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgInsureItem::CDlgInsureItem()
{
}

//析构函数
CDlgInsureItem::~CDlgInsureItem()
{
}

//获取欢乐豆
SCORE CDlgInsureItem::GetUserControlScore(CWnd * pWndScore)
{
	//变量定义
	WORD wIndex=0;
	SCORE lScore=0L;

	//获取数值
	TCHAR szBuffer[32]=TEXT("");
	pWndScore->GetWindowText(szBuffer,CountArray(szBuffer));

	//构造数值
	while (szBuffer[wIndex]!=0)
	{
		//变量定义
		WORD wCurrentIndex=wIndex++;

		//构造数值
		if ((szBuffer[wCurrentIndex]>=TEXT('0'))&&(szBuffer[wCurrentIndex]<=TEXT('9')))
		{
			lScore=lScore*10L+(szBuffer[wCurrentIndex]-TEXT('0'));
		}
	}

	return lScore;
}

//绘画数字
VOID CDlgInsureItem::DrawNumberString(CDC * pDC, SCORE lScore, INT nXPos, INT nYPos)
{
	//转换逗号
	TCHAR szControl[128]=TEXT("");
	SwitchScoreFormat(lScore,3L,szControl,CountArray(szControl));

	//变量定义
	INT nXDrawPos=nXPos;
	INT nScoreLength=lstrlen(szControl);

	//绘画判断
	if (nScoreLength>0L)
	{
		//加载资源
		CPngImage ImageNumber;
		ImageNumber.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("SCORE_NUMBER"));

		//获取大小
		CSize SizeNumber;
		SizeNumber.SetSize(ImageNumber.GetWidth()/12L,ImageNumber.GetHeight());

		//绘画数字
		for (INT i=0;i<nScoreLength;i++)
		{
			//绘画逗号
			if (szControl[i]==TEXT(','))
			{
				ImageNumber.DrawImage(pDC,nXDrawPos,nYPos,SizeNumber.cx,SizeNumber.cy,SizeNumber.cx*10L,0L);
			}

			//绘画点号
			if (szControl[i]==TEXT('.'))
			{
				ImageNumber.DrawImage(pDC,nXDrawPos,nYPos,SizeNumber.cx,SizeNumber.cy,SizeNumber.cx*11L,0L);
			}

			//绘画数字
			if (szControl[i]>=TEXT('0')&&szControl[i]<=TEXT('9'))
			{
				ImageNumber.DrawImage(pDC,nXDrawPos,nYPos,SizeNumber.cx,SizeNumber.cy,SizeNumber.cx*(szControl[i]-TEXT('0')),0L);
			}

			//设置位置
			nXDrawPos+=SizeNumber.cx;
		}
	}

	return;
}

//转换字符
VOID CDlgInsureItem::SwitchScoreString(SCORE lScore, LPTSTR pszBuffer, WORD wBufferSize)
{
	//变量定义
	LPCTSTR pszNumber[]={TEXT("零"),TEXT("壹"),TEXT("贰"),TEXT("叁"),TEXT("肆"),TEXT("伍"),TEXT("陆"),TEXT("柒"),TEXT("捌"),TEXT("玖")};
	LPCTSTR pszWeiName[]={TEXT("拾"),TEXT("佰"),TEXT("仟"),TEXT("万"),TEXT("拾"),TEXT("佰"),TEXT("仟"),TEXT("亿"),TEXT("拾"),TEXT("佰"),TEXT("仟"),TEXT("万")};

	//转换数值
	TCHAR szSwitchScore[16]=TEXT("");
	_sntprintf(szSwitchScore,CountArray(szSwitchScore),TEXT("%I64d"),lScore);

	//变量定义
	bool bNeedFill=false;
	bool bNeedZero=false;
	UINT uTargetIndex=0,uSourceIndex=0;

	//字符长度
	UINT uItemLength=lstrlen(pszNumber[0]);
	UINT uSwitchLength=lstrlen(szSwitchScore);

	//转换操作
	for (UINT i=0;i<uSwitchLength;i++)
	{
		//变量定义
		WORD wNumberIndex=szSwitchScore[i]-TEXT('0');

		//补零操作
		if ((bNeedZero==true)&&(wNumberIndex!=0L))
		{
			bNeedZero=false;
			_tcsncat(pszBuffer,pszNumber[0],wBufferSize-lstrlen(pszBuffer)-1);
		}

		//拷贝数字
		if (wNumberIndex!=0L)
		{
			_tcsncat(pszBuffer,pszNumber[wNumberIndex],wBufferSize-lstrlen(pszBuffer)-1);
		}

		//拷贝位名
		if ((wNumberIndex!=0L)&&((uSwitchLength-i)>=2))
		{
			bNeedZero=false;
			_tcsncat(pszBuffer,pszWeiName[uSwitchLength-i-2],wBufferSize-lstrlen(pszBuffer)-1);
		}

		//补零判断
		if ((bNeedZero==false)&&(wNumberIndex==0))
		{
			bNeedZero=true;
		}

		//补位判断
		if ((bNeedFill==false)&&(wNumberIndex!=0))
		{
			bNeedFill=true;
		}

		//填补位名
		if (((uSwitchLength-i)==5)||((uSwitchLength-i)==9))
		{
			//拷贝位名
			if ((bNeedFill==true)&&(wNumberIndex==0L))
			{
				_tcsncat(pszBuffer,pszWeiName[uSwitchLength-i-2],wBufferSize-lstrlen(pszBuffer)-1);
			}

			//设置变量
			bNeedZero=false;
			bNeedFill=false;
		}
	}

	return;
}

//转换字符
VOID CDlgInsureItem::SwitchScoreFormat(SCORE lScore, UINT uSpace, LPTSTR pszBuffer, WORD wBufferSize)
{
	//转换数值
	TCHAR szSwitchScore[16]=TEXT("");
	_sntprintf(szSwitchScore,CountArray(szSwitchScore),TEXT("%I64d"),lScore);

	//变量定义
	WORD wTargetIndex=0;
	WORD wSourceIndex=0;
	UINT uSwitchLength=lstrlen(szSwitchScore);

	//转换字符
	while (szSwitchScore[wSourceIndex]!=0)
	{
		//拷贝字符
		pszBuffer[wTargetIndex++]=szSwitchScore[wSourceIndex++];

		//插入逗号
		if ((uSwitchLength!=wSourceIndex)&&(((uSwitchLength-wSourceIndex)%uSpace)==0L))
		{
			pszBuffer[wTargetIndex++]=TEXT(',');
		}
	}

	//结束字符
	pszBuffer[wTargetIndex++]=0;

	return;
}
//////////////////////////////////////////////////////////////////////////////////
//构造函数
CDlgInsureRecord::CDlgInsureRecord(): CDialog(IDD_INSURE_RECORD)
{

}

//析构函数
CDlgInsureRecord::~CDlgInsureRecord()
{

}

BOOL CDlgInsureRecord::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ListDetail.Create(WS_CHILD | WS_VISIBLE | LVS_REPORT, CRect(0, 0, 0, 0), this, 1000);
	m_HeadCtrl.SubclassWindow(m_ListDetail.GetHeaderCtrl()->GetSafeHwnd());
	m_ListDetail.InsertColumn(0, TEXT("对方昵称"), LVCFMT_LEFT, 80);
	m_ListDetail.InsertColumn(1, TEXT("对方I D"), LVCFMT_LEFT, 80);
	m_ListDetail.InsertColumn(2, TEXT("交易分数"), LVCFMT_LEFT, 80);
	m_ListDetail.InsertColumn(3, TEXT("交易时间"), LVCFMT_LEFT, 130);
	//m_ListDetail.InsertColumn(5, TEXT("相关操作"), LVCFMT_LEFT, 65);
	m_btnQueryOut.Create(TEXT("查询转出"), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1001);
	m_btnQueryIn.Create(TEXT("查询转入"), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1002);

	return TRUE; 
}

VOID CDlgInsureRecord::OnOK()
{

}

VOID CDlgInsureRecord::OnCancel()
{

}

void CDlgInsureRecord::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if(GetDlgItem(IDC_STRING_TIP)!=NULL)
		GetDlgItem(IDC_STRING_TIP)->SetWindowPos(NULL, 2, 0, 430, 14, SWP_NOZORDER);

	m_ListDetail.SetWindowPos(NULL, 2, 14, 435, 185, SWP_NOZORDER);
	m_btnQueryOut.SetWindowPos(NULL, (cx / 2 - 77) / 2, 202, 77, 24, SWP_NOZORDER);
	m_btnQueryIn.SetWindowPos(NULL, (cx / 2 - 77) / 2 + cx / 2, 202, 77, 24, SWP_NOZORDER);

}

//插入记录 插入记录 交易明细
void CDlgInsureRecord::InsertBankDetail(LPCTSTR lpszNickName, DWORD dwGameID, SCORE lSwapScore, LPCTSTR lpszTime, BYTE cbTransferIn)
{
	int nItemCount = m_ListDetail.GetItemCount();
	int nItem = m_ListDetail.InsertItem(nItemCount,lpszNickName);
	TCHAR sz[48] = TEXT("");
	_sntprintf(sz, sizeof(sz), TEXT("%u"), dwGameID);
	m_ListDetail.SetItemText(nItem,1,sz);
	//转换逗号
	TCHAR szControl[128]=TEXT("");
	m_DlgInsureItem.SwitchScoreFormat(lSwapScore,3L,szControl,CountArray(szControl));
	//_sntprintf(sz, sizeof(sz), TEXT("%I64d(%s)"), lSwapScore, cbTransferIn == FALSE ? TEXT("转出") : TEXT("转入"));
	_sntprintf(sz, sizeof(sz), TEXT("%s"), szControl);
	m_ListDetail.SetItemText(nItem,2,sz);
	m_ListDetail.SetItemText(nItem,3,lpszTime);
	//_sntprintf(sz, sizeof(sz), TEXT("%I64d(%s)"), lSwapScore, cbTransferIn == FALSE ? TEXT("转出") : TEXT("转入"));
	//m_ListDetail.SetItemText(nItem,4,sz);
	//GetDlgItem(IDC_STRING_TIP)->SetWindowText(cbTransferIn == FALSE ? TEXT("转出记录") : TEXT("转入记录"));
	
}


void CDlgInsureRecord::OnBnClickedQueryOut()
{
	DWORD dwCurrentTime = (DWORD)time(NULL);
	/*if ((dwCurrentTime - m_dwLastQueryTime) < 60)
	{
		AfxMessageBox(TEXT("你查询太频繁了，请稍候再查询"));
	}
	else*/
	GetDlgItem(IDC_STRING_TIP)->SetWindowText(TEXT(""));
	{
		m_ListDetail.DeleteAllItems();
		m_dwLastQueryTime = dwCurrentTime;
		m_pDlgInsureMain->OnMissionStart();
		m_pDlgInsureMain->PerformQueryRecord(0);
	}
}

void CDlgInsureRecord::OnBnClickedQueryIn()
{
	DWORD dwCurrentTime = (DWORD)time(NULL);
	/*if ((dwCurrentTime - m_dwLastQueryTime) < 60)
	{
		AfxMessageBox(TEXT("你查询太频繁了，请稍候再查询"));
	}
	else*/

	GetDlgItem(IDC_STRING_TIP)->SetWindowText(TEXT(""));
	{
		m_ListDetail.DeleteAllItems();
		m_dwLastQueryTime = dwCurrentTime;
		m_pDlgInsureMain->OnMissionStart();
		m_pDlgInsureMain->PerformQueryRecord(1);
	}
}


//控件颜色
HBRUSH CDlgInsureRecord::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	//switch (nCtlColor)
	//{
	//case CTLCOLOR_DLG:
	//case CTLCOLOR_BTN:
	//case CTLCOLOR_STATIC:
	//	{
	//		pDC->SetBkMode(TRANSPARENT);
	//		pDC->SetTextColor(CSkinDialog::m_SkinAttribute.m_crControlText);
	//		return CSkinDialog::m_SkinAttribute.m_brBackGround;
	//	}
	//}

	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}


//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgInsureSave::CDlgInsureSave() : CDialog(IDD_INSURE_SAVE)
{
	//设置变量
	m_bSwitchIng=false;

	return;
}

//析构函数
CDlgInsureSave::~CDlgInsureSave()
{
}

//控件绑定
VOID CDlgInsureSave::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//按钮控件
	DDX_Control(pDX, IDC_BT_TAKE_SCORE, m_btTakeScore);
	DDX_Control(pDX, IDC_BT_SAVE_SCORE, m_btSaveScore);

	//控件变量
	DDX_Control(pDX, IDC_SCORE, m_edScore);
	DDX_Control(pDX, IDC_PASSWORD, m_PasswordControl);
	DDX_Control(pDX, IDC_FORGET_INSURE, m_ForgetInsure);
	
	
}

//创建函数
BOOL CDlgInsureSave::OnInitDialog()
{
	__super::OnInitDialog();

	//设置窗口
	m_SkinDPI.Attach(_AtlBaseModule.GetResourceInstance(),m_hWnd,m_lpszTemplateName,96.0);

	//设置字体
	CSkinDialog::SetWndFont(this,NULL);

	//设置控件
	m_edScore.LimitText(16L);

	//设置按钮
	HINSTANCE hResInstance=GetModuleHandle(SHARE_CONTROL_DLL_NAME);
	m_btSaveScore.SetButtonImage(IDB_BT_SAVE,hResInstance,false,false);
	m_btTakeScore.SetButtonImage(IDB_BT_TAKE,hResInstance,false,false);

	m_ImageBack.LoadFromResource(hResInstance,IDB_TAKE_SAVE_BACK);
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE);

	CRect rcCreate(0,0,0,0);
	m_bt100W.Create(TEXT("1百万"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rcCreate,this,IDC_SCORE_LINK);
	m_bt500W.Create(TEXT("5百万"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rcCreate,this,IDC_SCORE_LINK+1);
	m_bt1000W.Create(TEXT("1千万"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rcCreate,this,IDC_SCORE_LINK+2);
	m_bt5000W.Create(TEXT("5千万"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rcCreate,this,IDC_SCORE_LINK+3);
	m_bt1Y.Create(TEXT("1亿"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rcCreate,this,IDC_SCORE_LINK+4);
	m_bt5Y.Create(TEXT("5亿"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rcCreate,this,IDC_SCORE_LINK+5);
	m_btAllScore.Create(TEXT("全部游戏内"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rcCreate,this,IDC_SCORE_LINK+6);
	m_btAllInsure.Create(TEXT("全部保险箱"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rcCreate,this,IDC_SCORE_LINK+7);

	m_bt100W.SetHyperLinkTextColor(RGB(84,82,82),RGB(84,82,82),RGB(84,82,82));
	m_bt500W.SetHyperLinkTextColor(RGB(84,82,82),RGB(84,82,82),RGB(84,82,82));
	m_bt1000W.SetHyperLinkTextColor(RGB(84,82,82),RGB(84,82,82),RGB(84,82,82));
	m_bt5000W.SetHyperLinkTextColor(RGB(84,82,82),RGB(84,82,82),RGB(84,82,82));
	m_bt1Y.SetHyperLinkTextColor(RGB(84,82,82),RGB(84,82,82),RGB(84,82,82));
	m_bt5Y.SetHyperLinkTextColor(RGB(84,82,82),RGB(84,82,82),RGB(84,82,82));
	m_btAllScore.SetHyperLinkTextColor(RGB(84,82,82),RGB(84,82,82),RGB(84,82,82));
	m_btAllInsure.SetHyperLinkTextColor(RGB(84,82,82),RGB(84,82,82),RGB(84,82,82));

	//m_bt100W.SetUnderLine(true);
	//m_bt500W.SetUnderLine(true);
	//m_bt1000W.SetUnderLine(true);
	//m_bt5000W.SetUnderLine(true);
	//m_bt1Y.SetUnderLine(true);
	//m_bt5Y.SetUnderLine(true);
	//m_btAllScore.SetUnderLine(true);
	//m_btAllInsure.SetUnderLine(true);
	////m_bt100W.SetWindowPos(NULL,253-16,137-65,30,14,SWP_NOZORDER);
	m_bt100W.SetWindowPos(NULL,237,71,0,0,SWP_NOZORDER|SWP_NOSIZE);
	m_bt1000W.SetWindowPos(NULL,237+40,71,0,0,SWP_NOZORDER|SWP_NOSIZE);
	m_bt1Y.SetWindowPos(NULL,237+80,71,0,0,SWP_NOZORDER|SWP_NOSIZE);
	m_btAllScore.SetWindowPos(NULL,237+110,71,0,0,SWP_NOZORDER|SWP_NOSIZE);

	m_bt500W.SetWindowPos(NULL,237,72+14,0,0,SWP_NOZORDER|SWP_NOSIZE);
	m_bt5000W.SetWindowPos(NULL,237+40,72+14,0,0,SWP_NOZORDER|SWP_NOSIZE);
	m_bt5Y.SetWindowPos(NULL,237+80,72+14,0,0,SWP_NOZORDER|SWP_NOSIZE);
	m_btAllInsure.SetWindowPos(NULL,237+110,72+14,0,0,SWP_NOZORDER|SWP_NOSIZE);

	//获取位置
	CRect rcScore(0,0,0,0);
	m_edScore.GetWindowRect(&rcScore);

	//转换位置
	ScreenToClient(&rcScore);

	//设置位置
	m_edScore.MoveWindow(rcScore.left,rcScore.top,rcScore.Width(),14,SWP_NOZORDER|SWP_NOMOVE);

	return TRUE;
}

//
VOID CDlgInsureSave::OnBnClickedScore(UINT nID)
{
	LONGLONG lScore[]={1000000,5000000,10000000,50000000,100000000,500000000,0,0};
	lScore[6]=m_pUserInsureInfo->lUserScore;
	lScore[7]=m_pUserInsureInfo->lUserInsure;

	CString strScore;
	strScore.Format(TEXT("%I64d"),lScore[nID-IDC_SCORE_LINK]);
	m_edScore.SetWindowText(strScore);

}

//确定按钮
VOID CDlgInsureSave::OnOK()
{
	return;
}

//取消按钮
VOID CDlgInsureSave::OnCancel()
{
	return;
}

//输入信息
VOID CDlgInsureSave::OnEnChangeScore()
{
	//状态变量
	if (m_bSwitchIng==false)
	{
		//设置变量
		m_bSwitchIng=true;

		//变量定义
		CWnd * pWndScore=GetDlgItem(IDC_SCORE);
		SCORE lScore=m_DlgInsureItem.GetUserControlScore(pWndScore);
		SCORE lMaxScore=__max(m_pUserInsureInfo->lUserScore, m_pUserInsureInfo->lUserInsure);
		if((m_pUserInsureInfo->lUserScore==0) || (m_pUserInsureInfo->lUserInsure==0))
			lMaxScore=__max(m_pUserInsureInfo->lUserScore, m_pUserInsureInfo->lUserInsure);

		//输入限制
		if(lScore > lMaxScore) lScore = lMaxScore;

		//设置界面
		if (lScore>0L)
		{
			//转换数值
			TCHAR szString[128]=TEXT("");
			m_DlgInsureItem.SwitchScoreString(lScore,szString,CountArray(szString));

			//转换逗号
			TCHAR szControl[128]=TEXT("");
			m_DlgInsureItem.SwitchScoreFormat(lScore,3L,szControl,CountArray(szControl));

			//设置控件
			SetDlgItemText(IDC_SCORE,szControl);
			SetDlgItemText(IDC_SCORE_STRING,szString);

			//设置光标
			INT nScoreLength=m_edScore.GetWindowTextLength();
			m_edScore.SetSel(nScoreLength,nScoreLength,FALSE);
		}
		else
		{
			//设置控件
			SetDlgItemText(IDC_SCORE,TEXT(""));
			SetDlgItemText(IDC_SCORE_STRING,TEXT(""));
		}

		//设置变量
		m_bSwitchIng=false;
	}

	return;
}

//取款按钮
VOID CDlgInsureSave::OnBnClickedTakeScore()
{
	//获取变量
	CWnd * pWndScore=GetDlgItem(IDC_SCORE);
	SCORE lTakeScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

	//获取密码
	TCHAR szPassword[LEN_PASSWORD];
	m_PasswordControl.GetUserPassword(szPassword);

	//加密密码
	TCHAR szInsurePass[LEN_MD5];
	CWHEncrypt::MD5Encrypt(szPassword,szInsurePass);

	//数据判断
	if (lTakeScore<=0L)
	{
		//提示消息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("取出的欢乐豆数量不能为空，请重新输入欢乐豆数量！"),MB_ICONERROR,30);

		//设置焦点
		m_edScore.SetFocus();

		return;
	}

	//数目判断
	if (lTakeScore>m_pUserInsureInfo->lUserInsure)
	{
		//提示消息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("您保险柜欢乐豆的数目余额不足，请重新输入欢乐豆数量！"),MB_ICONERROR,30);

		//设置焦点
		m_edScore.SetFocus();

		return;
	}

	////密码判断
	//if (szPassword[0]==0)
	//{
	//	//提示消息
	//	CInformation Information(this);
	//	Information.ShowMessageBox(TEXT("保险柜密码不能为空，请重新输入保险柜密码！"),MB_ICONERROR,30);

	//	//设置焦点
	//	m_PasswordControl.SetFocus();

	//	return;
	//}
	//
	//事件通知
	m_pDlgInsureMain->OnMissionStart();
	m_pDlgInsureMain->PerformTakeScore(lTakeScore,szInsurePass);

	return;
}

//存款按钮
VOID CDlgInsureSave::OnBnClickedSaveScore()
{
	//变量定义
	CWnd * pWndScore=GetDlgItem(IDC_SCORE);
	SCORE lSaveScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

	//数据判断
	if (lSaveScore<=0L)
	{
		//提示消息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("存入的欢乐豆数量不能为空，请重新输入欢乐豆数量！"),MB_ICONERROR,30);

		//设置焦点
		m_edScore.SetFocus();

		return;
	}

	//数目判断
	if (lSaveScore>m_pUserInsureInfo->lUserScore)
	{
		//提示消息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("您所携带欢乐豆的数目余额不足，请重新输入欢乐豆数量！"),MB_ICONERROR,30);

		//设置焦点
		m_edScore.SetFocus();

		return;
	}

	//执行存款
	m_pDlgInsureMain->OnMissionStart();
	m_pDlgInsureMain->PerformSaveScore(lSaveScore);

	return;
}

//忘记密码
VOID CDlgInsureSave::OnBnClickedForgetInsure()
{
	//构造地址
	TCHAR szLogonLink[256]=TEXT("");
	_sntprintf(szLogonLink,CountArray(szLogonLink),TEXT("%s/LogonLink4.aspx"),szPlatformLink);

	//打开页面
	ShellExecute(NULL,TEXT("OPEN"),szLogonLink,NULL,NULL,SW_NORMAL);

	return;
}

//绘画背景
BOOL CDlgInsureSave::OnEraseBkgnd(CDC * pDC)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//建立缓冲
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//创建 DC
	CImageDC BufferDC(ImageBuffer);
	CDC * pDCBuffer=CDC::FromHandle(BufferDC);

	m_ImageBack.BitBlt(pDCBuffer->GetSafeHdc(),0,0);

	//设置环境
	pDCBuffer->SetBkMode(TRANSPARENT);
	pDCBuffer->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

	TCHAR szScore[100]=TEXT("");
	TCHAR szInsureScore[100]=TEXT("");

	m_DlgInsureItem.SwitchScoreFormat(m_pUserInsureInfo->lUserScore,3,szScore,CountArray(szScore));
	m_DlgInsureItem.SwitchScoreFormat(m_pUserInsureInfo->lUserInsure,3,szInsureScore,CountArray(szInsureScore));

	pDCBuffer->TextOut(98+8,48,szScore);
	pDCBuffer->TextOut(98+8,20,szInsureScore);

	////绘画背景
	//pDCBuffer->FillSolidRect(&rcClient,CSkinDialog::m_SkinAttribute.m_crBackGround);

	////获取位置
	//CRect rcScore;
	//m_edScore.GetWindowRect(&rcScore);

	////转换位置
	//ScreenToClient(&rcScore);

	////绘画输入
	//COLORREF crColorBorad=CSkinEdit::m_SkinAttribute.m_crEnableBorad;
	//pDCBuffer->Draw3dRect(rcScore.left-6,rcScore.top-5,rcScore.Width()+12,24L,crColorBorad,crColorBorad);

	//绘画界面
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pDCBuffer,0,0,SRCCOPY);

	return TRUE;
}

//控件颜色
HBRUSH CDlgInsureSave::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_BTN:
	case CTLCOLOR_STATIC:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(CSkinDialog::m_SkinAttribute.m_crControlText);
			return CSkinDialog::m_SkinAttribute.m_brBackGround;
		}
	}

	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgInsureTransfer::CDlgInsureTransfer() : CDialog(IDD_INSURE_TRANSFER)
{
	//设置变量
	m_bSwitchIng=false;

	return;
}

//析构函数
CDlgInsureTransfer::~CDlgInsureTransfer()
{
}

//控件绑定
VOID CDlgInsureTransfer::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//控件变量
	DDX_Control(pDX, IDC_SCORE, m_edScore);
	DDX_Control(pDX, IDC_NICKNAME, m_edNickName);
	DDX_Control(pDX, IDC_PASSWORD, m_PasswordControl);
	DDX_Control(pDX, IDC_FORGET_INSURE, m_ForgetInsure);
	DDX_Control(pDX, IDC_BT_TRANSFER_SCORE, m_btTransferScore);
}

//创建函数
BOOL CDlgInsureTransfer::OnInitDialog()
{
	__super::OnInitDialog();

	//设置窗口
	m_SkinDPI.Attach(_AtlBaseModule.GetResourceInstance(),m_hWnd,m_lpszTemplateName,96.0);

	//设置字体
	CSkinDialog::SetWndFont(this,NULL);

	//设置控件
	m_edScore.LimitText(16L);
	m_edNickName.LimitText(LEN_NICKNAME-1);

	//设置按钮
	HINSTANCE hResInstance=GetModuleHandle(SHARE_CONTROL_DLL_NAME);
	m_btTransferScore.SetButtonImage(IDB_BT_TRANS,hResInstance,false,false);
	//((CButton *)GetDlgItem(IDC_BY_NAME))->SetCheck(1);
	((CButton *)GetDlgItem(IDC_BY_ID))->SetCheck(1);
	m_ImageBack.LoadFromResource(hResInstance,IDB_TRANS_BACK);
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE);

	CRect rcCreate(0,0,0,0);
	m_bt100W.Create(TEXT("1百万"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rcCreate,this,IDC_SCORE_LINK);
	m_bt500W.Create(TEXT("5百万"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rcCreate,this,IDC_SCORE_LINK+1);
	m_bt1000W.Create(TEXT("1千万"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rcCreate,this,IDC_SCORE_LINK+2);
	m_bt5000W.Create(TEXT("5千万"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rcCreate,this,IDC_SCORE_LINK+3);
	m_bt1Y.Create(TEXT("1亿"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rcCreate,this,IDC_SCORE_LINK+4);
	m_bt5Y.Create(TEXT("5亿"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rcCreate,this,IDC_SCORE_LINK+5);


	m_bt100W.SetHyperLinkTextColor(RGB(84,82,82),RGB(84,82,82),RGB(84,82,82));
	m_bt500W.SetHyperLinkTextColor(RGB(84,82,82),RGB(84,82,82),RGB(84,82,82));
	m_bt1000W.SetHyperLinkTextColor(RGB(84,82,82),RGB(84,82,82),RGB(84,82,82));
	m_bt5000W.SetHyperLinkTextColor(RGB(84,82,82),RGB(84,82,82),RGB(84,82,82));
	m_bt1Y.SetHyperLinkTextColor(RGB(84,82,82),RGB(84,82,82),RGB(84,82,82));
	m_bt5Y.SetHyperLinkTextColor(RGB(84,82,82),RGB(84,82,82),RGB(84,82,82));


	m_bt100W.SetWindowPos(NULL,237+64,71+25,0,0,SWP_NOZORDER|SWP_NOSIZE);
	m_bt1000W.SetWindowPos(NULL,237+40+64,71+25,0,0,SWP_NOZORDER|SWP_NOSIZE);
	m_bt1Y.SetWindowPos(NULL,237+80+64,71+25,0,0,SWP_NOZORDER|SWP_NOSIZE);

	m_bt500W.SetWindowPos(NULL,237+64,72+14+25,0,0,SWP_NOZORDER|SWP_NOSIZE);
	m_bt5000W.SetWindowPos(NULL,237+40+64,72+14+25,0,0,SWP_NOZORDER|SWP_NOSIZE);
	m_bt5Y.SetWindowPos(NULL,237+80+64,72+14+25,0,0,SWP_NOZORDER|SWP_NOSIZE);


	//获取位置
	CRect rcScore;
	CRect rcNickName;
	m_edScore.GetWindowRect(&rcScore);
	m_edNickName.GetWindowRect(&rcNickName);

	//转换位置
	ScreenToClient(&rcScore);
	ScreenToClient(&rcNickName);

	//设置位置
	m_edScore.MoveWindow(rcScore.left,rcScore.top,rcScore.Width(),14,SWP_NOZORDER|SWP_NOMOVE);
	m_edNickName.MoveWindow(rcNickName.left,rcNickName.top,rcNickName.Width(),14,SWP_NOZORDER|SWP_NOMOVE);

	return TRUE;
}

//确定按钮
VOID CDlgInsureTransfer::OnOK()
{
	//转账操作
	OnBnClickedTransferScore();

	return;
}

//取消按钮
VOID CDlgInsureTransfer::OnCancel()
{
	return;
}


//失去焦点
VOID CDlgInsureTransfer::OnEnKillFocus()
{
	CString strText;
	GetDlgItemText(IDC_NICKNAME,strText);
	if(strText.IsEmpty()) return;
	
	m_pDlgInsureMain->OnMissionStart();
	m_pDlgInsureMain->PerformTransferScore(0,strText,0,TEXT("0"));

}

//输入信息
VOID CDlgInsureTransfer::OnEnChangeNick()
{
	SetDlgItemText(IDC_NICKNAME2,TEXT(""));
}

VOID CDlgInsureTransfer::OnBnClickedScore(UINT nID)
{
	LONGLONG lScore[]={1000000,5000000,10000000,50000000,100000000,500000000,0,0};

	CString strScore;
	strScore.Format(TEXT("%I64d"),lScore[nID-IDC_SCORE_LINK]);
	m_edScore.SetWindowText(strScore);
}

//输入信息
VOID CDlgInsureTransfer::OnEnChangeScore()
{
	//状态变量
	if (m_bSwitchIng==false)
	{
		//设置变量
		m_bSwitchIng=true;

		//变量定义
		CWnd * pWndScore=GetDlgItem(IDC_SCORE);
		SCORE lScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

		//输入限制
		if(lScore > m_pUserInsureInfo->lUserInsure) lScore = m_pUserInsureInfo->lUserInsure;

		//设置界面
		if (lScore>0L)
		{
			//转换数值
			TCHAR szString[128]=TEXT("");
			m_DlgInsureItem.SwitchScoreString(lScore,szString,CountArray(szString));

			//转换逗号
			TCHAR szControl[128]=TEXT("");
			m_DlgInsureItem.SwitchScoreFormat(lScore,3L,szControl,CountArray(szControl));

			//设置控件
			SetDlgItemText(IDC_SCORE,szControl);
			SetDlgItemText(IDC_SCORE_STRING,szString);

			//设置光标
			INT nScoreLength=m_edScore.GetWindowTextLength();
			m_edScore.SetSel(nScoreLength,nScoreLength,FALSE);
		}
		else
		{
			//设置控件
			SetDlgItemText(IDC_SCORE,TEXT(""));
			SetDlgItemText(IDC_SCORE_STRING,TEXT(""));
		}

		//设置变量
		m_bSwitchIng=false;
	}

	return;
}


//忘记密码
VOID CDlgInsureTransfer::OnBnClickedForgetInsure()
{
	//构造地址
	TCHAR szLogonLink[256]=TEXT("");
	_sntprintf(szLogonLink,CountArray(szLogonLink),TEXT("%s/LogonLink4.aspx"),szPlatformLink);

	//打开页面
	ShellExecute(NULL,TEXT("OPEN"),szLogonLink,NULL,NULL,SW_NORMAL);

	return;
}

//转账按钮
VOID CDlgInsureTransfer::OnBnClickedTransferScore()
{
	//条件判断
	if (m_pUserInsureInfo->lUserInsure < m_pUserInsureInfo->lTransferPrerequisite)
	{
		//提示消息
		TCHAR szTipInfo[128]={0};
		_sntprintf(szTipInfo,CountArray(szTipInfo),TEXT("您保险柜欢乐豆的数目余额不足最低转账条件 %I64d，无法进行转账操作！"),m_pUserInsureInfo->lTransferPrerequisite);
		CInformation Information(this);
		Information.ShowMessageBox(szTipInfo,MB_ICONERROR,30);

		return;
	}

	//获取变量
	CWnd * pWndScore=GetDlgItem(IDC_SCORE);
	SCORE lTransferScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

	//获取密码
	TCHAR szPassword[LEN_PASSWORD];
	m_PasswordControl.GetUserPassword(szPassword);

	//加密密码
	TCHAR szInsurePass[LEN_MD5];
	CWHEncrypt::MD5Encrypt(szPassword,szInsurePass);

	//获取昵称
	CString strNickName;
	GetDlgItemText(IDC_NICKNAME,strNickName);

	//格式昵称
	strNickName.TrimLeft();
	strNickName.TrimRight();

	//昵称判断
	if (strNickName.GetLength()==0)
	{
		//提示消息
		CInformation Information(this);
		if(((CButton *)GetDlgItem(IDC_BY_NAME))->GetCheck()==BST_CHECKED)
			Information.ShowMessageBox(TEXT("请输入要赠送的玩家昵称！"),MB_ICONERROR,30);
		else
			Information.ShowMessageBox(TEXT("请输入要赠送的玩家ID！"),MB_ICONERROR,30);

		//设置焦点
		m_edNickName.SetFocus();

		return;
	}

	//ID号判断
	if(((CButton *)GetDlgItem(IDC_BY_ID))->GetCheck()==BST_CHECKED)
	{
		int nlen = strNickName.GetLength();
		for(int i=0; i<nlen; i++)
		{
			if(strNickName[i] < '0' || strNickName[i] > '9')
			{
				//提示消息
				CInformation Information(this);
				Information.ShowMessageBox(TEXT("请输入正确的玩家ID！"),MB_ICONERROR,30);

				//设置焦点
				m_edNickName.SetFocus();
				return;
			}
		}
	}

	//数据判断
	if (lTransferScore<=0L)
	{
		//提示消息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("赠送的欢乐豆数量不能为空，请重新输入欢乐豆数量！"),MB_ICONERROR,30);

		//设置焦点
		m_edScore.SetFocus();

		return;
	}

	//数目判断
	if (lTransferScore>m_pUserInsureInfo->lUserInsure)
	{
		//提示消息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("您保险柜欢乐豆的数目余额不足，请重新输入欢乐豆数量！"),MB_ICONERROR,30);

		//设置焦点
		m_edScore.SetFocus();

		return;
	}

	//数目判断
	if (lTransferScore<m_pUserInsureInfo->lTransferPrerequisite)
	{
		//提示消息
		TCHAR szTipInfo[128]={0};
		_sntprintf(szTipInfo,CountArray(szTipInfo),TEXT("从保险柜转账的欢乐豆数目不能少于 %I64d，无法进行转账操作！"),m_pUserInsureInfo->lTransferPrerequisite);
		CInformation Information(this);
		Information.ShowMessageBox(szTipInfo,MB_ICONERROR,30);

		//设置焦点
		m_edScore.SetFocus();

		return;
	}

	////密码判断
	//if (szPassword[0]==0)
	//{
	//	//提示消息
	//	CInformation Information(this);
	//	Information.ShowMessageBox(TEXT("保险柜密码不能为空，请重新输入保险柜密码！"),MB_ICONERROR,30);

	//	//设置焦点
	//	m_PasswordControl.SetFocus();

	//	return;
	//}
	
	//事件通知
	m_pDlgInsureMain->OnMissionStart();
	m_pDlgInsureMain->PerformTransferScore(((CButton *)GetDlgItem(IDC_BY_NAME))->GetCheck(),strNickName,lTransferScore,szInsurePass);

	//设置焦点
	m_PasswordControl.SetFocus();
	m_PasswordControl.SetUserPassword(TEXT(""));

	return;
}

//绘画背景
BOOL CDlgInsureTransfer::OnEraseBkgnd(CDC * pDC)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//建立缓冲
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//创建 DC
	CImageDC BufferDC(ImageBuffer);
	CDC * pDCBuffer=CDC::FromHandle(BufferDC);

	//设置环境
	pDCBuffer->SetBkMode(TRANSPARENT);
	pDCBuffer->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());


	m_ImageBack.BitBlt(pDCBuffer->GetSafeHdc(),0,0);

	TCHAR szInsureScore[100]=TEXT("");

	m_DlgInsureItem.SwitchScoreFormat(m_pUserInsureInfo->lUserInsure,3,szInsureScore,CountArray(szInsureScore));
	pDCBuffer->TextOut(98+8,23,szInsureScore);



	////绘画背景
	//pDCBuffer->FillSolidRect(&rcClient,CSkinDialog::m_SkinAttribute.m_crBackGround);

	////获取位置
	//CRect rcScore;
	//CRect rcNickName;
	//m_edScore.GetWindowRect(&rcScore);
	//m_edNickName.GetWindowRect(&rcNickName);

	////转换位置
	//ScreenToClient(&rcScore);
	//ScreenToClient(&rcNickName);

	////绘画输入
	//COLORREF crColorBorad=CSkinEdit::m_SkinAttribute.m_crEnableBorad;
	//pDCBuffer->Draw3dRect(rcScore.left-6,rcScore.top-5,rcScore.Width()+12,24L,crColorBorad,crColorBorad);
	//pDCBuffer->Draw3dRect(rcNickName.left-6,rcNickName.top-5,rcNickName.Width()+12,24L,crColorBorad,crColorBorad);

	//绘画界面
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pDCBuffer,0,0,SRCCOPY);

	return TRUE;
}

//控件颜色
HBRUSH CDlgInsureTransfer::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_BTN:
	case CTLCOLOR_STATIC:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(CSkinDialog::m_SkinAttribute.m_crControlText);
			return CSkinDialog::m_SkinAttribute.m_brBackGround;
		}
	}

	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}

void CDlgInsureTransfer::OnBnClickedById()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_TRANSFER_TYPE_TIP)->SetWindowText(TEXT("赠送玩家 I D："));
	m_edNickName.SetFocus();
	m_edNickName.SetWindowText(TEXT(""));
}

void CDlgInsureTransfer::OnBnClickedByName()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_TRANSFER_TYPE_TIP)->SetWindowText(TEXT("赠送玩家昵称："));
	m_edNickName.SetFocus();
	m_edNickName.SetWindowText(TEXT(""));
}
//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgInsureMain::CDlgInsureMain() : CDialog(IDD_INSURE_MAIN)
{
	//服务管理
	m_bSaveMission=false;
	m_bTakeMission=false;
	m_bQueryMission=false;
	m_bModifyPassword=false;
	m_bDetailMission=false;

	m_pInsurePass=NULL;

	//设置变量
	m_bInitInfo=false;
	m_cbInsureMode=INSURE_SAVE;
	ZeroMemory(&m_UserInsureInfo,sizeof(m_UserInsureInfo));
	ZeroMemory(&m_TransInfo,sizeof(m_TransInfo));

	return;
}

//析构函数
CDlgInsureMain::~CDlgInsureMain()
{
}

VOID CDlgInsureMain::SetInsurePassword(LPTSTR lpInsure)
{
	m_pInsurePass=lpInsure;
}

//控件绑定
VOID CDlgInsureMain::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//控件变量
	DDX_Control(pDX, IDC_TAB_CONTROL, m_TabControl);
	DDX_Control(pDX, IDC_BT_QUERY_INFO, m_btQueryInfo);
}

//消息解释
BOOL CDlgInsureMain::PreTranslateMessage(MSG * pMsg)
{
	//按键过虑
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

void CDlgInsureMain::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//模拟标题
	if (point.y<30)
	{
		//模拟标题
		PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

		return;
	}

	__super::OnLButtonDown(nFlags, point);
}

//创建函数
BOOL CDlgInsureMain::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("保险柜"));

	//设置资源
	AfxSetResourceHandle(GetModuleHandle(NULL));

    
	//设置字体
	CSkinDialog::SetWndFont(this,NULL);

	//设置控件
	m_TabControl.SetItemSize(CSize(95,25));

	//存取控件
	m_DlgInsureSave.m_pDlgInsureMain=this;
	m_DlgInsureSave.m_pUserInsureInfo=&m_UserInsureInfo;

	//转账控件
	m_DlgInsureTransfer.m_pDlgInsureMain=this;
	m_DlgInsureTransfer.m_pUserInsureInfo=&m_UserInsureInfo;

	//密码控件
	m_DlgInsurePassword.m_pDlgInsureMain=this;
	m_DlgInsurePassword.m_pUserInsureInfo=&m_UserInsureInfo;

	m_DlgInsureRecord.m_pDlgInsureMain=this;
	m_DlgInsureRecord.m_pUserInsureInfo=&m_UserInsureInfo;

	//加入标签
	m_TabControl.InsertItem(INSURE_SAVE,TEXT("欢乐豆存取"));
	m_TabControl.InsertItem(INSURE_TRANSFER,TEXT("欢乐豆赠送"));
	m_TabControl.InsertItem(INSURE_RECORD,TEXT("赠送记录"));
	m_TabControl.InsertItem(INSURE_PASSWORD,TEXT("口令修改"));

	//设置按钮
	HINSTANCE hResInstance=GetModuleHandle(SHARE_CONTROL_DLL_NAME);
	//m_btQueryInfo.SetButtonImage(IDB_BT_QUERY_INFO,hResInstance,false,false);
	m_btQueryInfo.SetButtonImage(IDB_BT_CLOSE,hResInstance,false,false);

	m_ImageBack.LoadImage(hResInstance,TEXT("INSURE_MAIN_BACK"));

	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE);

	//查询信息
	QueryUserInsureInfo();

	//获取大小
	CRect rcClient;
	GetClientRect(&rcClient);

	//加载资源
	CPngImage ImageInsureInfo;
	ImageInsureInfo.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("INFO_FRAME"));

	//绘画信息
	INT nYPosInfo=86L;
	INT nXPosInfo=(rcClient.Width()-ImageInsureInfo.GetWidth())/2L+350;

	//获取位置
	CRect rcButton(0,0,0,0);
	m_TabControl.MoveWindow(26,33,300,27,SWP_NOZORDER|SWP_NOMOVE);
	m_btQueryInfo.GetWindowRect(&rcButton);
	//m_btQueryInfo.MoveWindow(nXPosInfo,nYPosInfo,rcButton.Width(),rcButton.Height(),SWP_NOZORDER|SWP_NOMOVE);
	m_btQueryInfo.MoveWindow(435,4,rcButton.Width(),rcButton.Height(),SWP_NOZORDER|SWP_NOMOVE);

	//存取模式
	SwitchInsureModeView(INSURE_SAVE);

	return FALSE;
}

VOID CDlgInsureMain::OnCancel()
{
	if(((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck()==BST_CHECKED)
	{
		m_pInsurePass[0]=0;
	}

	__super::OnCancel();
}

//确定按钮
VOID CDlgInsureMain::OnOK()
{
	//默认动作
	switch (m_cbInsureMode)
	{
	case INSURE_SAVE:		//存取方式
		{
			m_DlgInsureSave.OnOK();
			break;
		}
	case INSURE_TRANSFER:	//转账方式
		{
			m_DlgInsureTransfer.OnOK();
			break;
		}
	}

	return;
}

//绘画背景
BOOL CDlgInsureMain::OnEraseBkgnd(CDC * pDC)
{
	CRect rcClient;
	GetClientRect(&rcClient);

	OnDrawClientArea(pDC,rcClient.Width(),rcClient.Height());

	return TRUE;
}

//绘画消息
VOID CDlgInsureMain::OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight)
{
	////获取位置
	//CRect rcTabControl;
	//m_TabControl.GetWindowRect(&rcTabControl);

	////转换位置
	//ScreenToClient(&rcTabControl);

	////绘画线条
	//INT nLBorder=m_SkinAttribute.m_EncircleInfoView.nLBorder;
	//INT nRBorder=m_SkinAttribute.m_EncircleInfoView.nRBorder;
	//pDC->FillSolidRect(nLBorder,rcTabControl.bottom-1L,nWidth-nLBorder-nRBorder,1,RGB(145,168,183));

	m_ImageBack.DrawImage(pDC,0,0);

	////加载资源
	//CPngImage ImageInsureInfo;
	//ImageInsureInfo.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("INFO_FRAME"));

	////绘画信息
	//INT nYPosInfo=75L;
	//INT nXPosInfo=(nWidth-ImageInsureInfo.GetWidth())/2L;
	//ImageInsureInfo.DrawImage(pDC,nXPosInfo,nYPosInfo,ImageInsureInfo.GetWidth(),ImageInsureInfo.GetHeight(),0,0);

	////银行信息
	//if (m_bInitInfo==true)
	//{
	//	//加载资源
	//	CPngImage ImageInsureIcon;
	//	ImageInsureIcon.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("INSURE_ICON"));

	//	//绘画标志
	//	ImageInsureIcon.DrawImage(pDC,nXPosInfo+10,nYPosInfo+17);

	//	//绘画数字
	//	m_DlgInsureItem.DrawNumberString(pDC,m_UserInsureInfo.lUserScore,nXPosInfo+136,nYPosInfo+18L);
	//	m_DlgInsureItem.DrawNumberString(pDC,m_UserInsureInfo.lUserInsure,nXPosInfo+136,nYPosInfo+53L);

	//	//绘画提示
	//	LPCTSTR pszScoreString=TEXT("欢乐豆：");
	//	LPCTSTR pszInsureString=TEXT("保险柜：");
	//	pDC->TextOut(nXPosInfo+83,nYPosInfo+20L,pszScoreString,lstrlen(pszScoreString));
	//	pDC->TextOut(nXPosInfo+83,nYPosInfo+55L,pszInsureString,lstrlen(pszInsureString));

	//	//构造提示
	//	TCHAR szString[128]=TEXT("");
	//	_sntprintf(szString,CountArray(szString),TEXT("温馨提示：存入欢乐豆免手续费，取出将扣除%ld‰,赠送将扣除%ld‰的手续费"),m_UserInsureInfo.wRevenueTake,m_UserInsureInfo.wRevenueTransfer);

	//	//温馨提示
	//	pDC->SetTextColor(RGB(50,50,50));
	//	pDC->TextOut(18,nHeight-28,szString,lstrlen(szString));
	//}
	//else
	//{
	//}

	return;
}

//开始任务
VOID CDlgInsureMain::OnMissionStart()
{
	return;
}

//终止任务
VOID CDlgInsureMain::OnMissionConclude()
{
	return;
}

//查询资料
VOID CDlgInsureMain::QueryUserInsureInfo()
{
	//设置变量
	m_bSaveMission=false;
	m_bTakeMission=false;
	m_bQueryMission=true;
	m_bTransferMission=false;
	m_bModifyPassword=false;

	//事件处理
	OnMissionStart();

	//执行查询
	PerformQueryInfo();

	return;
}

//设置模式
VOID CDlgInsureMain::SwitchInsureModeView(BYTE cbInsureMode)
{
	switch (cbInsureMode)
	{
	case INSURE_SAVE:			//存取方式
		{
			//设置变量
			m_cbInsureMode=INSURE_SAVE;

			//设置控件
			if (m_DlgInsureSave.m_hWnd==NULL)
			{
				//获取位置
				CRect rcClient;
				GetClientRect(&rcClient);

				//设置资源
				AfxSetResourceHandle(GetModuleHandle(SHARE_CONTROL_DLL_NAME));

				//创建控件
				m_DlgInsureSave.Create(IDD_INSURE_SAVE,this);

				//设置资源
				AfxSetResourceHandle(GetModuleHandle(NULL));

				m_DlgInsureSave.SetWindowPos(NULL,16,65,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_SHOWWINDOW);
			}
			else
			{
				//显示窗口
				m_DlgInsureSave.ShowWindow(SW_SHOW);
			}

			//设置控件
			if (m_DlgInsureTransfer.m_hWnd!=NULL) m_DlgInsureTransfer.ShowWindow(SW_HIDE);
			if (m_DlgInsureRecord.m_hWnd!=NULL) m_DlgInsureRecord.ShowWindow(SW_HIDE);
			if (m_DlgInsurePassword.m_hWnd!=NULL) m_DlgInsurePassword.ShowWindow(SW_HIDE);

			//设置焦点
			m_DlgInsureSave.SetFocus();

			//设置选择
			m_TabControl.SetCurSel(m_cbInsureMode);

			break;
		}
	case INSURE_TRANSFER:		//转账方式
		{
			//设置变量
			m_cbInsureMode=INSURE_TRANSFER;

			//设置控件
			if (m_DlgInsureTransfer.m_hWnd==NULL)
			{
				//获取位置
				CRect rcClient;
				GetClientRect(&rcClient);

				//设置资源
				AfxSetResourceHandle(GetModuleHandle(SHARE_CONTROL_DLL_NAME));

				//创建控件
				m_DlgInsureTransfer.Create(IDD_INSURE_TRANSFER,this);

				//设置资源
				AfxSetResourceHandle(GetModuleHandle(NULL));

				m_DlgInsureTransfer.SetWindowPos(NULL,16,65,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_SHOWWINDOW);
			}
			else
			{
				//显示窗口
				m_DlgInsureTransfer.ShowWindow(SW_SHOW);
			}

			//设置控件
			if (m_DlgInsureSave.m_hWnd!=NULL) m_DlgInsureSave.ShowWindow(SW_HIDE);
			if (m_DlgInsureRecord.m_hWnd!=NULL) m_DlgInsureRecord.ShowWindow(SW_HIDE);
			if (m_DlgInsurePassword.m_hWnd!=NULL) m_DlgInsurePassword.ShowWindow(SW_HIDE);

			//设置焦点
			m_DlgInsureTransfer.SetFocus();

			//设置选择
			m_TabControl.SetCurSel(m_cbInsureMode);

			break;
		}
	case INSURE_RECORD:
		{
			//设置变量
			m_cbInsureMode=INSURE_RECORD;

			//设置控件
			if (m_DlgInsureRecord.m_hWnd==NULL)
			{
				//获取位置
				CRect rcClient;
				GetClientRect(&rcClient);

				//设置资源
				AfxSetResourceHandle(GetModuleHandle(SHARE_CONTROL_DLL_NAME));

				//创建控件
				m_DlgInsureRecord.Create(IDD_INSURE_RECORD,this);

				//设置资源
				AfxSetResourceHandle(GetModuleHandle(NULL));

				m_DlgInsureRecord.SetWindowPos(NULL,16,65,439,229,SWP_NOZORDER|SWP_SHOWWINDOW);
			}
			else
			{
				//显示窗口
				m_DlgInsureRecord.ShowWindow(SW_SHOW);
			}

			//设置控件
			if (m_DlgInsureSave.m_hWnd!=NULL) m_DlgInsureSave.ShowWindow(SW_HIDE);
			if (m_DlgInsurePassword.m_hWnd!=NULL) m_DlgInsurePassword.ShowWindow(SW_HIDE);
			if (m_DlgInsureTransfer.m_hWnd!=NULL) m_DlgInsureTransfer.ShowWindow(SW_HIDE);


			//设置选择
			m_TabControl.SetCurSel(m_cbInsureMode);

			break;
		}
	case INSURE_PASSWORD:
		{
			//设置变量
			m_cbInsureMode=INSURE_PASSWORD;

			//设置控件
			if (m_DlgInsurePassword.m_hWnd==NULL)
			{
				//获取位置
				CRect rcClient;
				GetClientRect(&rcClient);

				//设置资源
				AfxSetResourceHandle(GetModuleHandle(SHARE_CONTROL_DLL_NAME));

				//创建控件
				m_DlgInsurePassword.Create(IDD_INSURE_PASSWORD,this);

				//设置资源
				AfxSetResourceHandle(GetModuleHandle(NULL));

				m_DlgInsurePassword.SetWindowPos(NULL,16,65,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_SHOWWINDOW);
			}
			else
			{
				//显示窗口
				m_DlgInsurePassword.ShowWindow(SW_SHOW);
			}

			//设置控件
			if (m_DlgInsureSave.m_hWnd!=NULL) m_DlgInsureSave.ShowWindow(SW_HIDE);
			if (m_DlgInsureRecord.m_hWnd!=NULL) m_DlgInsureRecord.ShowWindow(SW_HIDE);
			if (m_DlgInsureTransfer.m_hWnd!=NULL) m_DlgInsureTransfer.ShowWindow(SW_HIDE);

			//设置焦点
			m_DlgInsurePassword.SetFocus();

			//设置选择
			m_TabControl.SetCurSel(m_cbInsureMode);


			break;
		}

	default:
		{
			ASSERT(FALSE);
		}
	}

	return;
}

//查询按钮
VOID CDlgInsureMain::OnBnClickedQueryInfo()
{
	//查询信息
	QueryUserInsureInfo();

	//设置焦点
	switch (m_cbInsureMode)
	{
	case INSURE_SAVE:		//存取方式
		{
			m_DlgInsureSave.SetFocus();
			break;
		}
	case INSURE_TRANSFER:	//转账方式
		{
			m_DlgInsureTransfer.SetFocus();
			break;
		}
	}

	return;
}

//类型改变
VOID CDlgInsureMain::OnTcnSelchangeInsureMode(NMHDR * pNMHDR, LRESULT * pResult)
{
	switch (m_TabControl.GetCurSel())
	{
	case INSURE_SAVE:		//存取方式
		{
			SwitchInsureModeView(INSURE_SAVE);
			break;
		}
	case INSURE_TRANSFER:	//转账方式
		{
			SwitchInsureModeView(INSURE_TRANSFER);
			break;
		}
	case INSURE_PASSWORD:	//密码修改
		{
			SwitchInsureModeView(INSURE_PASSWORD);
			break;
		}
	case INSURE_RECORD:		//游戏记录
		{
			SwitchInsureModeView(INSURE_RECORD);
			break;
		}
	default:
		{
			ASSERT(FALSE);
		}
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgInsurePlaza::CDlgInsurePlaza()
{
	//设置任务
	m_MissionManager.InsertMissionItem(this);

	//设置变量
	m_lScore=0L;
	ZeroMemory(m_szNickName,sizeof(m_szNickName));
	ZeroMemory(m_szInsurePass,sizeof(m_szInsurePass));

	return;
}

//析构函数
CDlgInsurePlaza::~CDlgInsurePlaza()
{
}

//查询事件
VOID CDlgInsurePlaza::PerformQueryInfo()
{
	//发起连接
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}

	return;
}

//存入事件
VOID CDlgInsurePlaza::PerformSaveScore(SCORE lSaveScore)
{
	//设置变量
	m_bSaveMission=true;
	m_bTakeMission=false;
	m_bQueryMission=false;
	m_bTransferMission=false;
	m_bModifyPassword=false;
	m_bDetailMission=false;

	//设置变量
	m_lScore=lSaveScore;

	//发起连接
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}

	return;
}

//取出事件
VOID CDlgInsurePlaza::PerformTakeScore(SCORE lTakeScore, LPCTSTR pszInsurePass)
{
	//设置变量
	m_bTakeMission=true;
	m_bSaveMission=false;
	m_bQueryMission=false;
	m_bTransferMission=false;
	m_bModifyPassword=false;
	m_bDetailMission=false;

	//设置变量
	m_lScore=lTakeScore;
	//lstrcpyn(m_szInsurePass,pszInsurePass,CountArray(m_szInsurePass));
	lstrcpyn(m_szInsurePass,m_pInsurePass,CountArray(m_szInsurePass));

	//发起连接
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}

	return;
}

//取出事件
VOID CDlgInsurePlaza::PerformQueryRecord(BYTE cbTransferIn)
{

	//设置变量
	m_bSaveMission=false;
	m_bTakeMission=false;
	m_bQueryMission=false;
	m_bTransferMission=false;
	m_bModifyPassword=false;
	m_bDetailMission=true;
	m_cbTransferIn = cbTransferIn;
	

	//发起连接
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}

	return ;
}

//修改密码
VOID CDlgInsurePlaza::PerformModifyInsurePassword(LPCTSTR szSrcPassword, LPCTSTR szDesPassword)
{
	//设置变量
	m_bTakeMission=false;
	m_bSaveMission=false;
	m_bQueryMission=false;
	m_bTransferMission=false;
	m_bModifyPassword=true;
	m_bDetailMission=false;


	//发起连接
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}

}

//转账事件
VOID CDlgInsurePlaza::PerformTransferScore(BYTE cbByNickName, LPCTSTR pszNickName, SCORE lTransferScore, LPCTSTR pszInsurePass)
{
	//设置变量
	m_bSaveMission=false;
	m_bTakeMission=false;
	m_bQueryMission=false;
	m_bTransferMission=true;
	m_bModifyPassword=false;
	m_bDetailMission=false;

	//设置变量
	m_cbByNickName=cbByNickName;
	m_lScore=lTransferScore;
	lstrcpyn(m_szNickName,pszNickName,CountArray(m_szNickName));
	//lstrcpyn(m_szInsurePass,pszInsurePass,CountArray(m_szInsurePass));
	lstrcpyn(m_szInsurePass,m_pInsurePass,CountArray(m_szInsurePass));

	//发起连接
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}

	return;
}

//连接事件
bool CDlgInsurePlaza::OnEventMissionLink(INT nErrorCode)
{
	//连接结果
	if (nErrorCode!=0)
	{
		//重试任务
		if (m_MissionManager.AvtiveMissionItem(this,true)==true)
		{
			return true;
		}

		//事件处理
		OnMissionConclude();

		//错误提示
		if (m_bSaveMission==true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("连接服务器超时，欢乐豆存入失败，请稍后再次重试！"),MB_OK|MB_ICONSTOP,30);
		}

		//错误提示
		if (m_bTakeMission==true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("连接服务器超时，欢乐豆取出失败，请稍后再次重试！"),MB_OK|MB_ICONSTOP,30);
		}

		//错误提示
		if (m_bQueryMission==true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("连接服务器超时，保险柜资料获取失败！"),MB_OK|MB_ICONSTOP,30);
		}

		//错误提示
		if (m_bTransferMission==true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("连接服务器超时，欢乐豆赠送失败，请稍后再次重试！"),MB_OK|MB_ICONSTOP,30);
		}

		//错误提示
		if (m_bModifyPassword==true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("连接服务器超时，密码修改失败，请稍后再次重试！"),MB_OK|MB_ICONSTOP,30);
		}

		return true;
	}

	//存入任务
	if (m_bSaveMission==true)
	{
		//变量定义
		CMD_GP_UserSaveScore UserSaveScore;
		ZeroMemory(&UserSaveScore,sizeof(UserSaveScore));

		//变量定义
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//设置变量
		UserSaveScore.lSaveScore=m_lScore;
		UserSaveScore.dwUserID=pGlobalUserData->dwUserID;
		CWHService::GetMachineIDEx(UserSaveScore.szMachineID);

		//发送数据
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_USER_SAVE_SCORE,&UserSaveScore,sizeof(UserSaveScore));

		return true;
	}

	//取出任务
	if (m_bTakeMission==true)
	{
		//变量定义
		CMD_GP_UserTakeScore UserTakeScore;
		ZeroMemory(&UserTakeScore,sizeof(UserTakeScore));

		//变量定义
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//设置变量
		UserTakeScore.lTakeScore=m_lScore;
		UserTakeScore.dwUserID=pGlobalUserData->dwUserID;
		CWHService::GetMachineIDEx(UserTakeScore.szMachineID);
		lstrcpyn(UserTakeScore.szPassword,m_szInsurePass,CountArray(UserTakeScore.szPassword));
		//lstrcpyn(UserTakeScore.szPassword,m_pInsurePass,CountArray(UserTakeScore.szPassword));

		//发送数据
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_USER_TAKE_SCORE,&UserTakeScore,sizeof(UserTakeScore));

		return true;
	}

	//转账任务
	if (m_bTransferMission==true)
	{
		//变量定义
		CMD_GP_QueryUserInfoRequest QueryUserInfoRequest;
		ZeroMemory(&QueryUserInfoRequest,sizeof(QueryUserInfoRequest));

		//设置变量
		QueryUserInfoRequest.cbByNickName=m_cbByNickName;
		lstrcpyn(QueryUserInfoRequest.szNickName,m_szNickName,CountArray(QueryUserInfoRequest.szNickName));

		//发送数据
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_QUERY_USER_INFO_REQUEST,&QueryUserInfoRequest,sizeof(QueryUserInfoRequest));
	}

	//查询任务
	if (m_bQueryMission==true)
	{
		//变量定义
		CMD_GP_QueryInsureInfo QueryInsureInfo;
		ZeroMemory(&QueryInsureInfo,sizeof(QueryInsureInfo));

		//设置变量
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		QueryInsureInfo.dwUserID=pGlobalUserInfo->GetGlobalUserData()->dwUserID;
		lstrcpyn(QueryInsureInfo.szPassword,pGlobalUserInfo->GetGlobalUserData()->szPassword,CountArray(QueryInsureInfo.szPassword));

		//发送数据
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_QUERY_INSURE_INFO,&QueryInsureInfo,sizeof(QueryInsureInfo));
	}

	//密码修改
	if (m_bModifyPassword==true)
	{
		//变量定义
		CMD_GP_ModifyInsurePass ModifyInsurePass;
		ZeroMemory(&ModifyInsurePass,sizeof(ModifyInsurePass));

		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		ModifyInsurePass.dwUserID=pGlobalUserInfo->GetGlobalUserData()->dwUserID;

		//加密密码
		TCHAR szSrcPasswordEn[LEN_PASSWORD]=TEXT("");
		TCHAR szDesPasswordEn[LEN_PASSWORD]=TEXT("");
		CWHEncrypt::MD5Encrypt(m_DlgInsurePassword.m_szSrcPassword,szSrcPasswordEn);
		CWHEncrypt::MD5Encrypt(m_DlgInsurePassword.m_szDesPassword,szDesPasswordEn);

		//构造数据
		lstrcpyn(ModifyInsurePass.szScrPassword,szSrcPasswordEn,CountArray(ModifyInsurePass.szScrPassword));
		lstrcpyn(ModifyInsurePass.szDesPassword,szDesPasswordEn,CountArray(ModifyInsurePass.szDesPassword));

		//发送数据
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_CHANGE_INSURE_PASSWORD,&ModifyInsurePass,sizeof(ModifyInsurePass));
	}

	//交易记录
	if(m_bDetailMission==true)
	{
		//构造数据
		CMD_GP_QueryBankDetail QueryBankDetail;
		ZeroMemory(&QueryBankDetail,sizeof(QueryBankDetail));
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		QueryBankDetail.dwUserID=pGlobalUserInfo->GetGlobalUserData()->dwUserID;
		QueryBankDetail.cbTransferIn = m_cbTransferIn;

		//发送数据
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_QUERY_BANK_DETAIL,&QueryBankDetail,sizeof(QueryBankDetail));

	}

	return true;
}

//关闭事件
bool CDlgInsurePlaza::OnEventMissionShut(BYTE cbShutReason)
{
	//提示消息
	if (cbShutReason!=SHUT_REASON_NORMAL)
	{
		//重试任务
		if (m_MissionManager.AvtiveMissionItem(this,true)==true)
		{
			return true;
		}

		//存入任务
		if (m_bSaveMission==true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("与服务器的连接异常断开了，欢乐豆存入失败！"),MB_ICONERROR,30);
		}

		//取出任务
		if (m_bTakeMission==true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("与服务器的连接异常断开了，欢乐豆取出失败！"),MB_ICONERROR,30);
		}

		//查询任务
		if (m_bQueryMission==true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("与服务器的连接异常断开了，保险柜资料获取失败！"),MB_ICONERROR,30);
		}

		//交易记录
		if (m_bDetailMission==true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("与服务器的连接异常断开了，交易记录获取失败！"),MB_ICONERROR,30);
		}
	}

	//事件处理
	OnMissionConclude();

	return true;
}

//读取事件
bool CDlgInsurePlaza::OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//命令处理
	if (Command.wMainCmdID==MDM_GP_USER_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_USER_INSURE_INFO:		//银行资料
			{
				//效验参数
				ASSERT(wDataSize==sizeof(CMD_GP_UserInsureInfo));
				if (wDataSize<sizeof(CMD_GP_UserInsureInfo)) return false;

				//变量定义
				CMD_GP_UserInsureInfo * pUserInsureInfo=(CMD_GP_UserInsureInfo *)pData;

				//设置变量
				m_bInitInfo=true;
				m_UserInsureInfo.wRevenueTake=pUserInsureInfo->wRevenueTake;
				m_UserInsureInfo.wRevenueTransfer=pUserInsureInfo->wRevenueTransfer;
				m_UserInsureInfo.wServerID=pUserInsureInfo->wServerID;
				m_UserInsureInfo.lUserScore=pUserInsureInfo->lUserScore;
				m_UserInsureInfo.lUserInsure=pUserInsureInfo->lUserInsure;
				m_UserInsureInfo.lTransferPrerequisite=pUserInsureInfo->lTransferPrerequisite;

				//关闭连接
				m_MissionManager.ConcludeMissionItem(this,false);

				//更新大厅
				tagInsurePlazaUpdata Updata;
				Updata.lUserScore=pUserInsureInfo->lUserScore;
				Updata.lUserInsure=pUserInsureInfo->lUserInsure;
				AfxGetMainWnd()->SendMessage(WM_INSUREPLAZA_UPDATA,(WPARAM)&Updata,0);

				if(m_DlgInsureSave.IsWindowVisible())m_DlgInsureSave.RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);
				if(m_DlgInsureTransfer.IsWindowVisible())m_DlgInsureTransfer.RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);

				//更新界面
				RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);

				return true;
			}
		case SUB_GP_USER_INSURE_SUCCESS:	//银行成功
			{
				//效验参数
				CMD_GP_UserInsureSuccess * pUserInsureSuccess=(CMD_GP_UserInsureSuccess *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_UserInsureSuccess)-sizeof(pUserInsureSuccess->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_UserInsureSuccess)-sizeof(pUserInsureSuccess->szDescribeString))) return false;

				//设置变量
				m_UserInsureInfo.lUserScore=pUserInsureSuccess->lUserScore;
				m_UserInsureInfo.lUserInsure=pUserInsureSuccess->lUserInsure;

				//关闭连接
				m_MissionManager.ConcludeMissionItem(this,false);

				//输入控件
				if(m_DlgInsureSave.m_PasswordControl.m_hWnd != NULL)
				{
					m_DlgInsureSave.m_PasswordControl.SetFocus();
					m_DlgInsureSave.m_PasswordControl.SetUserPassword(TEXT(""));
				}
				if(m_DlgInsureTransfer.m_PasswordControl.m_hWnd != NULL)
				{
					m_DlgInsureTransfer.m_PasswordControl.SetFocus();
					m_DlgInsureTransfer.m_PasswordControl.SetUserPassword(TEXT(""));
				}

				//更新大厅
				tagInsurePlazaUpdata Updata;
				Updata.lUserScore=pUserInsureSuccess->lUserScore;
				Updata.lUserInsure=pUserInsureSuccess->lUserInsure;
				AfxGetMainWnd()->SendMessage(WM_INSUREPLAZA_UPDATA,(WPARAM)&Updata,0);

				if(m_DlgInsureSave.IsWindowVisible())m_DlgInsureSave.RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);
				if(m_DlgInsureTransfer.IsWindowVisible())m_DlgInsureTransfer.RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);

				//更新界面
				RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);

				//转账成功，显示自定义窗口
				if(m_bTransferMission)
				{
				//	m_DlgInsureTransfer.Invalidate();

					CDlgTransferResult Result;
					Result.SetTransInfo(&m_TransInfo);
					Result.DoModal();
					return true;
				}

				//显示消息
				if (pUserInsureSuccess->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pUserInsureSuccess->szDescribeString,MB_ICONINFORMATION,60);
				}

				return true;
			}
		case SUB_GP_USER_INSURE_FAILURE:	//银行失败
			{
				//效验参数
				CMD_GP_UserInsureFailure * pUserInsureFailure=(CMD_GP_UserInsureFailure *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_UserInsureFailure)-sizeof(pUserInsureFailure->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_UserInsureFailure)-sizeof(pUserInsureFailure->szDescribeString))) return false;

				//失败处理
				OnMissionConclude();

				//关闭连接
				m_MissionManager.ConcludeMissionItem(this,false);

				//输入控件
				if(m_DlgInsureSave.m_PasswordControl.m_hWnd != NULL)
				{
					m_DlgInsureSave.m_PasswordControl.SetFocus();
					m_DlgInsureSave.m_PasswordControl.SetUserPassword(TEXT(""));
				}
				if(m_DlgInsureTransfer.m_PasswordControl.m_hWnd != NULL)
				{
					m_DlgInsureTransfer.m_PasswordControl.SetFocus();
					m_DlgInsureTransfer.m_PasswordControl.SetUserPassword(TEXT(""));
				}

				//显示消息
				if (pUserInsureFailure->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pUserInsureFailure->szDescribeString,MB_ICONERROR,60);
				}

				return true;
			}
		case SUB_GP_QUERY_USER_INFO_RESULT:	//用户信息
			{
				//效验参数
				ASSERT(wDataSize==sizeof(CMD_GP_UserTransferUserInfo));
				if (wDataSize<sizeof(CMD_GP_UserTransferUserInfo)) return false;

				//变量定义
				CMD_GP_UserTransferUserInfo * pTransferUserInfo=(CMD_GP_UserTransferUserInfo *)pData;

				if(m_lScore == 0)
				{
					m_DlgInsureTransfer.SetDlgItemText(IDC_NICKNAME2,pTransferUserInfo->szNickName);
				}
				else
				{
					//构造消息
					TCHAR szMessage[128]={0};
					_sntprintf(szMessage, CountArray(szMessage), TEXT("您确定要给[%s], ID:%d 赠送%I64d 欢乐豆吗?"), pTransferUserInfo->szNickName, pTransferUserInfo->dwTargetGameID, m_lScore);

					//提示消息
					CInformation Information(this);
					if (Information.ShowMessageBox(szMessage,MB_ICONQUESTION|MB_YESNO,0)==IDYES)
					{
						//变量定义
						CMD_GP_UserTransferScore UserTransferScore;
						ZeroMemory(&UserTransferScore,sizeof(UserTransferScore));

						//变量定义
						CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
						tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

						//设置变量
						UserTransferScore.cbByNickName=m_cbByNickName;
						UserTransferScore.lTransferScore=m_lScore;
						UserTransferScore.dwUserID=pGlobalUserData->dwUserID;
						CWHService::GetMachineIDEx(UserTransferScore.szMachineID);
						lstrcpyn(UserTransferScore.szNickName,m_szNickName,CountArray(UserTransferScore.szNickName));
						lstrcpyn(UserTransferScore.szPassword,m_szInsurePass,CountArray(UserTransferScore.szPassword));
						//lstrcpyn(UserTransferScore.szPassword,m_pInsurePass,CountArray(UserTransferScore.szPassword));

						ZeroMemory(&m_TransInfo,sizeof(m_TransInfo));
						m_TransInfo.dwSrcGameID=pGlobalUserData->dwGameID;
						m_TransInfo.dwTagGameID=pTransferUserInfo->dwTargetGameID;
						lstrcpyn(m_TransInfo.szSrcNickName,pGlobalUserData->szNickName,CountArray(m_TransInfo.szSrcNickName));
						lstrcpyn(m_TransInfo.szTagNickName,pTransferUserInfo->szNickName,CountArray(m_TransInfo.szTagNickName));
						m_TransInfo.lScore=m_lScore;

						//发送数据
						m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_USER_TRANSFER_SCORE,&UserTransferScore,sizeof(UserTransferScore));
					}

				}
				return true;
			}
		case SUB_GP_OPERATE_FAILURE:		//操作失败
			{
				//效验参数
				CMD_GP_OperateFailure * pOperateFailure=(CMD_GP_OperateFailure *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_OperateFailure)-sizeof(pOperateFailure->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_OperateFailure)-sizeof(pOperateFailure->szDescribeString))) return false;

				//关闭连接
				m_MissionManager.ConcludeMissionItem(this,false);
				//失败处理
				OnMissionConclude();

				m_DlgInsurePassword.ResetControlStatus();

				//显示消息
				if (pOperateFailure->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pOperateFailure->szDescribeString,MB_ICONERROR,60);
				}

				return true;
			}
		case SUB_GP_OPERATE_SUCCESS:		//操作成功
			{
				//变量定义
				CMD_GP_OperateSuccess * pOperateSuccess=(CMD_GP_OperateSuccess *)pData;

				//效验数据
				ASSERT(wDataSize>=(sizeof(CMD_GP_OperateSuccess)-sizeof(pOperateSuccess->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_OperateSuccess)-sizeof(pOperateSuccess->szDescribeString))) return false;

				//关闭连接
				m_MissionManager.ConcludeMissionItem(this,false);

				if(m_bModifyPassword)
				{
					//加密密码
					TCHAR szDesPasswordEn[LEN_PASSWORD]=TEXT("");
					CWHEncrypt::MD5Encrypt(m_DlgInsurePassword.m_szDesPassword,szDesPasswordEn);
					lstrcpy(m_pInsurePass,szDesPasswordEn);

				}

				m_DlgInsurePassword.ResetControlStatus();

				//显示消息
				if (pOperateSuccess->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pOperateSuccess->szDescribeString,MB_ICONINFORMATION,60);
				}

				return true;
			}
		case SUB_GP_BANK_DETAIL_RESULT:	//银行细节
			{
				//效验参数
				ASSERT(wDataSize%sizeof(CMD_GP_BankDetailResult)==0);
				if (wDataSize%sizeof(CMD_GP_BankDetailResult)!=0) return false;
				//变量定义
				WORD wItemCount=wDataSize/sizeof(CMD_GP_BankDetailResult);
				CMD_GP_BankDetailResult * pBankDetailResult=(CMD_GP_BankDetailResult *)(pData);

				if (wItemCount == 1 && pBankDetailResult->cbState == 0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(TEXT("银行明细查询失败"),MB_ICONINFORMATION,60);
					//关闭连接
					m_MissionManager.ConcludeMissionItem(this,false);
				}
				else
				{
					for (WORD i = 0; i < wItemCount; ++i)
					{
						CMD_GP_BankDetailResult* pDetail = pBankDetailResult + i;
						m_DlgInsureRecord.InsertBankDetail(pDetail->szNickName, pDetail->dwGameID, pDetail->lSwapScore, pDetail->szDateTime, m_cbTransferIn);
						if (pDetail->cbState == 2)
						{
							//关闭连接
							m_MissionManager.ConcludeMissionItem(this,false);
						}
					}
				}

				//成功处理
				OnMissionConclude();

				return true;
			}
		}
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgInsurePassword::CDlgInsurePassword() : CDialog(IDD_INSURE_PASSWORD)
{
	//设置变量
	ZeroMemory(m_szSrcPassword,sizeof(m_szSrcPassword));
	ZeroMemory(m_szDesPassword,sizeof(m_szDesPassword));

	return;
}

//析构函数
CDlgInsurePassword::~CDlgInsurePassword()
{
}

//控件绑定
VOID CDlgInsurePassword::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//按钮控件
	DDX_Control(pDX, IDC_BT_TAKE_SCORE, m_btTakeScore);
	DDX_Control(pDX, IDC_BT_SAVE_SCORE, m_btSaveScore);

	//控件变量
	//二级密码
	DDX_Control(pDX, IDC_INSURE_PASSWORD1, m_edInsurePassword1);
	DDX_Control(pDX, IDC_INSURE_PASSWORD2, m_edInsurePassword2);
	DDX_Control(pDX, IDC_INSURE_PASSWORD3, m_edInsurePassword3);
}

VOID CDlgInsurePassword::ResetControlStatus()
{
	GetDlgItem(IDC_INSURE_PASSWORD1)->SetWindowText(TEXT(""));
	GetDlgItem(IDC_INSURE_PASSWORD2)->SetWindowText(TEXT(""));
	GetDlgItem(IDC_INSURE_PASSWORD3)->SetWindowText(TEXT(""));
}

//创建函数
BOOL CDlgInsurePassword::OnInitDialog()
{
	__super::OnInitDialog();

	//设置窗口
	m_ImageBk.LoadFromResource(GetModuleHandle(SHARE_CONTROL_DLL_NAME),IDB_PASSWORD);

	m_btTakeScore.SetButtonImage(IDB_BT_MODIFY,GetModuleHandle(SHARE_CONTROL_DLL_NAME),false,false);

	m_KeyBoard.Create(IDD_PASSWORD_KEYBOARD2,this);
	m_KeyBoard.SetKeyBoardSink(this);

	SetWindowPos(NULL,0,0,m_ImageBk.GetWidth(),m_ImageBk.GetHeight(),SWP_NOMOVE);
	m_KeyBoard.SetWindowPos(NULL,60,150,0,0,SWP_NOSIZE);

	return TRUE;
}

VOID CDlgInsurePassword::OnKeyBoardChar(WORD wChar)
{
	CWnd* pWnd=GetFocus();
	if(pWnd->GetSafeHwnd()==m_edInsurePassword1.GetSafeHwnd())
	{
		m_edInsurePassword1.SendMessage(WM_CHAR,wChar,0);
	}

	if(pWnd->GetSafeHwnd()==m_edInsurePassword2.GetSafeHwnd())
	{
		m_edInsurePassword2.SendMessage(WM_CHAR,wChar,0);
	}

	if(pWnd->GetSafeHwnd()==m_edInsurePassword3.GetSafeHwnd())
	{
		m_edInsurePassword3.SendMessage(WM_CHAR,wChar,0);
	}
}

VOID CDlgInsurePassword::OnKeyBoardClean()
{
	CWnd* pWnd=GetFocus();
	if(pWnd->GetSafeHwnd()==m_edInsurePassword1.GetSafeHwnd())
	{
		m_edInsurePassword1.SetWindowText(TEXT(""));
	}

	if(pWnd->GetSafeHwnd()==m_edInsurePassword2.GetSafeHwnd())
	{
		m_edInsurePassword2.SetWindowText(TEXT(""));
	}

	if(pWnd->GetSafeHwnd()==m_edInsurePassword3.GetSafeHwnd())
	{
		m_edInsurePassword3.SetWindowText(TEXT(""));
	}
}

//确定按钮
VOID CDlgInsurePassword::OnOK()
{
	return;
}

//取消按钮
VOID CDlgInsurePassword::OnCancel()
{
	return;
}

//取款按钮
VOID CDlgInsurePassword::OnBnClickedTakeScore()
{

	//变量定义
	TCHAR szSrcPassword[LEN_PASSWORD];
	TCHAR szDesPassword[2][LEN_PASSWORD];

	//获取信息
	GetDlgItemText(IDC_INSURE_PASSWORD1,szSrcPassword,sizeof(szSrcPassword));
	GetDlgItemText(IDC_INSURE_PASSWORD2,szDesPassword[0],sizeof(szDesPassword[0]));
	GetDlgItemText(IDC_INSURE_PASSWORD3,szDesPassword[1],sizeof(szDesPassword[1]));

	//密码判断
	if (szSrcPassword[0]==0)
	{
		//提示消息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("原密码错误，请重新输入！"),MB_ICONERROR,0);

		m_edInsurePassword1.SetFocus();

		return;
	}

	//密码判断
	if (lstrcmp(szDesPassword[0],szDesPassword[1])!=0)
	{
		//提示消息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("两次输入的新密码不一致，请重新输入！"),MB_ICONERROR,0);

		//设置焦点
		m_edInsurePassword2.SetFocus();

		return;
	}

	//变量定义
	TCHAR szDescribe[128]=TEXT("");
	CUserItemElement * pUserItemElement=CUserItemElement::GetInstance();

	//密码判断
	if (pUserItemElement->EfficacyPassword(szDesPassword[0],szDescribe,CountArray(szDescribe))==false)
	{
		//提示消息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("新密码不能为空，请重新输入！"),MB_ICONERROR,0);

		//设置焦点
		m_edInsurePassword2.SetFocus();

		return;
	}

	//一致判断
	if (lstrcmp(szSrcPassword,szDesPassword[0])==0)
	{
		//提示信息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("您所输入的新密码与原密码一致，请重新输入新密码！"),MB_ICONERROR,0);

		//设置焦点
		m_edInsurePassword2.SetFocus();

		//设置密码
		m_edInsurePassword2.SetWindowText(TEXT(""));
		m_edInsurePassword3.SetWindowText(TEXT(""));

		return;
	}

	//设置变量
	lstrcpyn(m_szSrcPassword,szSrcPassword,CountArray(m_szSrcPassword));
	lstrcpyn(m_szDesPassword,szDesPassword[0],CountArray(m_szDesPassword));

	//事件通知
	m_pDlgInsureMain->OnMissionStart();
	m_pDlgInsureMain->PerformModifyInsurePassword(m_szSrcPassword,m_szDesPassword);

	return;
}

//存款按钮
VOID CDlgInsurePassword::OnBnClickedSaveScore()
{
	//m_pDlgInsureMain->OnCancel();

	return;
}

//绘画背景
BOOL CDlgInsurePassword::OnEraseBkgnd(CDC * pDC)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//建立缓冲
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//创建 DC
	CImageDC BufferDC(ImageBuffer);
	CDC * pDCBuffer=CDC::FromHandle(BufferDC);

	//设置环境
	pDCBuffer->SetBkMode(TRANSPARENT);
	pDCBuffer->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

	m_ImageBk.BitBlt(pDCBuffer->GetSafeHdc(),0,0);

	////绘画背景
	//pDCBuffer->FillSolidRect(&rcClient,RGB(238,242,245));


	//绘画界面
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pDCBuffer,0,0,SRCCOPY);

	return TRUE;
}

//控件颜色
HBRUSH CDlgInsurePassword::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_BTN:
	case CTLCOLOR_STATIC:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(0,0,0));
			static CBrush brPass;
			brPass.CreateSolidBrush(RGB(238,242,245));
			return brPass;
		}
	}

	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgInsureServer::CDlgInsureServer()
{
	//设置变量
	m_pITCPSocket=NULL;
	m_pIMySelfUserItem=NULL;

	m_lScore=0L;

	return;
}

//析构函数
CDlgInsureServer::~CDlgInsureServer()
{
}

//查询事件
VOID CDlgInsureServer::PerformQueryInfo()
{
	//效验参数
	ASSERT(m_pITCPSocket!=NULL);
	if (m_pITCPSocket==NULL) return;

	//变量定义
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//变量定义
	CMD_GR_C_QueryInsureInfoRequest QueryInsureInfo;
	ZeroMemory(&QueryInsureInfo,sizeof(QueryInsureInfo));

	//设置变量
	QueryInsureInfo.cbActivityGame=FALSE;
	lstrcpyn(QueryInsureInfo.szInsurePass,pGlobalUserData->szPassword,CountArray(QueryInsureInfo.szInsurePass));

	//发送数据
	m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_QUERY_INSURE_INFO,&QueryInsureInfo,sizeof(QueryInsureInfo));

	return;
}

//存入事件
VOID CDlgInsureServer::PerformSaveScore(SCORE lSaveScore)
{
	//效验参数
	ASSERT(m_pITCPSocket!=NULL);
	if (m_pITCPSocket==NULL) return;

	m_bTransferMission=false;

	//变量定义
	CMD_GR_C_SaveScoreRequest SaveScoreRequest;
	ZeroMemory(&SaveScoreRequest,sizeof(SaveScoreRequest));

	//设置变量
	SaveScoreRequest.lSaveScore=lSaveScore;

	//发送数据
	m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_SAVE_SCORE_REQUEST,&SaveScoreRequest,sizeof(SaveScoreRequest));

	return;
}

//取出事件
VOID CDlgInsureServer::PerformTakeScore(SCORE lTakeScore, LPCTSTR pszInsurePass)
{
	//效验参数
	ASSERT(m_pITCPSocket!=NULL);
	if (m_pITCPSocket==NULL) return;

	m_bTransferMission=false;

	//变量定义
	CMD_GR_C_TakeScoreRequest TakeScoreRequest;
	ZeroMemory(&TakeScoreRequest,sizeof(TakeScoreRequest));

	//设置变量
	TakeScoreRequest.lTakeScore=lTakeScore;
	//lstrcpyn(TakeScoreRequest.szInsurePass,pszInsurePass,CountArray(TakeScoreRequest.szInsurePass));
	lstrcpyn(TakeScoreRequest.szInsurePass,m_pInsurePass,CountArray(TakeScoreRequest.szInsurePass));

	//发送数据
	m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_TAKE_SCORE_REQUEST,&TakeScoreRequest,sizeof(TakeScoreRequest));

	return;
}


//取出事件
VOID CDlgInsureServer::PerformQueryRecord(BYTE cbTransferIn)
{
	//效验参数
	ASSERT(m_pITCPSocket!=NULL);
	if (m_pITCPSocket==NULL) return;

	//构造数据
	CMD_GR_QueryBankDetail QueryBankDetail;
	ZeroMemory(&QueryBankDetail,sizeof(QueryBankDetail));
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	QueryBankDetail.dwUserID=pGlobalUserInfo->GetGlobalUserData()->dwUserID;
	QueryBankDetail.cbTransferIn = cbTransferIn;

	//发送数据
	m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_QUERY_BANK_DETAIL,&QueryBankDetail,sizeof(QueryBankDetail));

	return ;
}



//修改密码
VOID CDlgInsureServer::PerformModifyInsurePassword(LPCTSTR szSrcPassword, LPCTSTR szDesPassword)
{
	//效验参数
	ASSERT(m_pITCPSocket!=NULL);
	if (m_pITCPSocket==NULL) return;

	//变量定义
	CMD_GP_ModifyInsurePass ModifyInsurePass;
	ZeroMemory(&ModifyInsurePass,sizeof(ModifyInsurePass));

	//加密密码
	TCHAR szSrcPasswordEn[LEN_PASSWORD]=TEXT("");
	TCHAR szDesPasswordEn[LEN_PASSWORD]=TEXT("");
	CWHEncrypt::MD5Encrypt(szSrcPassword,szSrcPasswordEn);
	CWHEncrypt::MD5Encrypt(szDesPassword,szDesPasswordEn);

	//构造数据
	lstrcpyn(ModifyInsurePass.szScrPassword,szSrcPasswordEn,CountArray(ModifyInsurePass.szScrPassword));
	lstrcpyn(ModifyInsurePass.szDesPassword,szDesPasswordEn,CountArray(ModifyInsurePass.szDesPassword));

	//发送数据
	m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_CHANGE_INSURE_PASSWORD,&ModifyInsurePass,sizeof(ModifyInsurePass));
}

//转账事件
VOID CDlgInsureServer::PerformTransferScore(BYTE cbByNickName, LPCTSTR pszNickName, SCORE lTransferScore, LPCTSTR pszInsurePass)
{
	//效验参数
	ASSERT(m_pITCPSocket!=NULL);
	if (m_pITCPSocket==NULL) return;

	m_bTransferMission=true;

	//设置变量
	m_cbByNickName=cbByNickName;
	m_lScore=lTransferScore;
	lstrcpyn(m_szNickName,pszNickName,CountArray(m_szNickName));
	//lstrcpyn(m_szInsurePass,pszInsurePass,CountArray(m_szInsurePass));
	lstrcpyn(m_szInsurePass,m_pInsurePass,CountArray(m_szInsurePass));

	//变量定义
	CMD_GR_C_QueryUserInfoRequest QueryUserInfoRequest;
	ZeroMemory(&QueryUserInfoRequest,sizeof(QueryUserInfoRequest));

	//设置变量
	QueryUserInfoRequest.cbByNickName=cbByNickName;
	lstrcpyn(QueryUserInfoRequest.szNickName,pszNickName,CountArray(QueryUserInfoRequest.szNickName));

	//发送数据
	m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_QUERY_USER_INFO_REQUEST,&QueryUserInfoRequest,sizeof(QueryUserInfoRequest));

	return;
}

//配置银行
VOID CDlgInsureServer::Initialization(ITCPSocket * pITCPSocket, IClientUserItem * pIMySelfUserItem)
{
	//设置变量
	m_pITCPSocket=pITCPSocket;
	m_pIMySelfUserItem=pIMySelfUserItem;

	return;
}

//银行消息
bool CDlgInsureServer::OnServerInsureMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//消息判断
	if (wMainCmdID==MDM_GR_INSURE)
	{
		switch (wSubCmdID)
		{
		case SUB_GR_USER_INSURE_INFO:		//银行资料
			{
				//效验参数
				ASSERT(wDataSize==sizeof(CMD_GR_S_UserInsureInfo));
				if (wDataSize<sizeof(CMD_GR_S_UserInsureInfo)) return false;

				//变量定义
				CMD_GR_S_UserInsureInfo * pUserInsureInfo=(CMD_GR_S_UserInsureInfo *)pData;

				//设置变量
				m_bInitInfo=true;
				m_UserInsureInfo.wRevenueTake=pUserInsureInfo->wRevenueTake;
				m_UserInsureInfo.wRevenueTransfer=pUserInsureInfo->wRevenueTransfer;
				m_UserInsureInfo.wServerID=pUserInsureInfo->wServerID;
				m_UserInsureInfo.lUserScore=pUserInsureInfo->lUserScore;
				m_UserInsureInfo.lUserInsure=pUserInsureInfo->lUserInsure;
				m_UserInsureInfo.lTransferPrerequisite=pUserInsureInfo->lTransferPrerequisite;

				if(m_DlgInsureSave.IsWindowVisible())m_DlgInsureSave.RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);
				if(m_DlgInsureTransfer.IsWindowVisible())m_DlgInsureTransfer.RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);
				//更新界面
				RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);
				return true;
			}
		case SUB_GR_USER_PASSWORD_RESULT:	//密码结果
			{

				return true;
			}
		case SUB_GR_USER_CHANGE_RESULT:		//密码结果
			{
				//效验参数
				ASSERT(wDataSize==sizeof(CMD_GR_S_ChangeInsurePassword));
				if (wDataSize!=sizeof(CMD_GR_S_ChangeInsurePassword)) return false;

				CMD_GR_S_ChangeInsurePassword* pResult=(CMD_GR_S_ChangeInsurePassword*)pData;


				//if(m_bModifyPassword)
				{
					//加密密码
					TCHAR szDesPasswordEn[LEN_PASSWORD]=TEXT("");
					CWHEncrypt::MD5Encrypt(m_DlgInsurePassword.m_szDesPassword,szDesPasswordEn);
					lstrcpy(m_pInsurePass,szDesPasswordEn);

				}

				if(pResult->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pResult->szDescribeString,MB_ICONINFORMATION,60);
				}

				return true;
			}
		case SUB_GR_USER_INSURE_SUCCESS:	//银行成功
			{
				//效验参数
				CMD_GR_S_UserInsureSuccess * pUserInsureSuccess=(CMD_GR_S_UserInsureSuccess *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GR_S_UserInsureSuccess)-sizeof(pUserInsureSuccess->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GR_S_UserInsureSuccess)-sizeof(pUserInsureSuccess->szDescribeString))) return false;

				//设置变量
				m_UserInsureInfo.lUserScore=pUserInsureSuccess->lUserScore;
				m_UserInsureInfo.lUserInsure=pUserInsureSuccess->lUserInsure;

				if(m_DlgInsureSave.IsWindowVisible())m_DlgInsureSave.RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);
				if(m_DlgInsureTransfer.IsWindowVisible())m_DlgInsureTransfer.RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);
				
				//输入控件
				if(m_DlgInsureSave.m_PasswordControl.m_hWnd != NULL)
				{
					m_DlgInsureSave.m_PasswordControl.SetFocus();
					m_DlgInsureSave.m_PasswordControl.SetUserPassword(TEXT(""));
				}
				if(m_DlgInsureTransfer.m_PasswordControl.m_hWnd != NULL)
				{
					m_DlgInsureTransfer.m_PasswordControl.SetFocus();
					m_DlgInsureTransfer.m_PasswordControl.SetUserPassword(TEXT(""));
				}

				//更新界面
				RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);

				//转账成功，显示自定义窗口
				if(m_bTransferMission)
				{
					m_DlgInsureTransfer.Invalidate();

					CDlgTransferResult Result;
					Result.SetTransInfo(&m_TransInfo);
					Result.DoModal();
					return true;
				}

				//显示消息
				if (pUserInsureSuccess->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pUserInsureSuccess->szDescribeString,MB_ICONINFORMATION,60);
				}

				return true;
			}
		case SUB_GR_USER_INSURE_FAILURE:	//银行失败
			{
				//效验参数
				CMD_GR_S_UserInsureFailure * pUserInsureFailure=(CMD_GR_S_UserInsureFailure *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_UserInsureFailure)-sizeof(pUserInsureFailure->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_UserInsureFailure)-sizeof(pUserInsureFailure->szDescribeString))) return false;

				//失败处理
				OnMissionConclude();

				//输入控件
				if (m_bInitInfo==true)
				{
					if(m_DlgInsureSave.m_PasswordControl.m_hWnd != NULL)
					{
						m_DlgInsureSave.m_PasswordControl.SetFocus();
						m_DlgInsureSave.m_PasswordControl.SetUserPassword(TEXT(""));
					}
					if(m_DlgInsureTransfer.m_PasswordControl.m_hWnd != NULL)
					{
						m_DlgInsureTransfer.m_PasswordControl.SetFocus();
						m_DlgInsureTransfer.m_PasswordControl.SetUserPassword(TEXT(""));
					}
				}

				//显示消息
				if (pUserInsureFailure->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pUserInsureFailure->szDescribeString,MB_ICONERROR,60);
				}

				return true;
			}
		case SUB_GR_USER_TRANSFER_USER_INFO:  //用户信息
			{
				//效验参数
				ASSERT(wDataSize==sizeof(CMD_GR_S_UserTransferUserInfo));
				if (wDataSize<sizeof(CMD_GR_S_UserTransferUserInfo)) return false;

				//变量定义
				CMD_GR_S_UserTransferUserInfo * pTransferUserInfo=(CMD_GR_S_UserTransferUserInfo *)pData;



				if(m_lScore == 0)
				{
					CString strDbg;
					strDbg.Format(TEXT("%s"),pTransferUserInfo->szNickName);
					m_DlgInsureTransfer.SetDlgItemText(IDC_NICKNAME2,strDbg);
				}
				else
				{
					//构造消息
					TCHAR szMessage[128]={0};
					_sntprintf(szMessage, CountArray(szMessage), TEXT("您确定要给[%s], ID:%d 赠送%I64d 欢乐豆吗?"), pTransferUserInfo->szNickName, pTransferUserInfo->dwTargetGameID, m_lScore);

					//提示消息
					CInformation Information(this);
					if (Information.ShowMessageBox(szMessage,MB_ICONQUESTION|MB_YESNO,0)==IDYES)
					{
						//变量定义
						CMD_GP_C_TransferScoreRequest TransferScoreRequest;
						ZeroMemory(&TransferScoreRequest,sizeof(TransferScoreRequest));

						//变量定义
						CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
						tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

						//设置变量
						TransferScoreRequest.cbByNickName=m_cbByNickName;
						TransferScoreRequest.lTransferScore=m_lScore;
						lstrcpyn(TransferScoreRequest.szNickName,m_szNickName,CountArray(TransferScoreRequest.szNickName));
						lstrcpyn(TransferScoreRequest.szInsurePass,m_szInsurePass,CountArray(TransferScoreRequest.szInsurePass));
						//lstrcpyn(TransferScoreRequest.szInsurePass,m_pInsurePass,CountArray(TransferScoreRequest.szInsurePass));

						ZeroMemory(&m_TransInfo,sizeof(m_TransInfo));
						m_TransInfo.dwSrcGameID=pGlobalUserData->dwGameID;
						m_TransInfo.dwTagGameID=pTransferUserInfo->dwTargetGameID;
						lstrcpyn(m_TransInfo.szSrcNickName,pGlobalUserData->szNickName,CountArray(m_TransInfo.szSrcNickName));
						lstrcpyn(m_TransInfo.szTagNickName,pTransferUserInfo->szNickName,CountArray(m_TransInfo.szTagNickName));
						m_TransInfo.lScore=m_lScore;

						//发送数据
						m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_TRANSFER_SCORE_REQUEST,&TransferScoreRequest,sizeof(TransferScoreRequest));
					}
				}
				return true;
			}
		case SUB_GR_BANK_DETAIL_RESULT:		//交易记录
			{
				//效验参数
				ASSERT(wDataSize%sizeof(CMD_GR_BankDetailResult)==0);
				if (wDataSize%sizeof(CMD_GR_BankDetailResult)!=0) return false;
				//变量定义
				WORD wItemCount=wDataSize/sizeof(CMD_GR_BankDetailResult);
				CMD_GR_BankDetailResult * pBankDetailResult=(CMD_GR_BankDetailResult *)(pData);


				if (wItemCount == 1 && pBankDetailResult->cbState == 0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(TEXT("银行明细查询失败"),MB_ICONINFORMATION,60);
				}
				else
				{
					for (WORD i = 0; i < wItemCount; ++i)
					{
						CMD_GR_BankDetailResult* pDetail = pBankDetailResult + i;
						m_DlgInsureRecord.InsertBankDetail(pDetail->szNickName, pDetail->dwGameID, pDetail->lSwapScore, pDetail->szDateTime, m_cbTransferIn);
						if (pDetail->cbState == 2)
						{
							////关闭连接
							//m_MissionManager.ConcludeMissionItem(this,false);
						}
					}
				}

				return true;
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////

CDlgTransferResult::CDlgTransferResult():CSkinDialog(IDD_TRANS_MESSAGE)
{
	m_pTransInfo=NULL;
	m_ImageBk.LoadFromResource(GetModuleHandle(SHARE_CONTROL_DLL_NAME),IDB_TRANS_BK);
}

CDlgTransferResult::~CDlgTransferResult()
{

}

VOID CDlgTransferResult::OnOK()
{

}

VOID CDlgTransferResult::SetTransInfo(tagTransInfo* pInfo)
{
	if(pInfo==NULL) return;

	m_pTransInfo=pInfo;

	
}

VOID CDlgTransferResult::OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight)
{
	m_ImageBk.BitBlt(pDC->GetSafeHdc(),20,40);

	if(m_pTransInfo==NULL) return;
	
	pDC->SetBkMode(1);
	pDC->SetTextColor(RGB(0,0,0));
	pDC->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

	CString strSrcID,strTagID,strScore;
	strSrcID.Format(TEXT("%ld"),m_pTransInfo->dwSrcGameID);
	strTagID.Format(TEXT("%ld"),m_pTransInfo->dwTagGameID);
	strScore.Format(TEXT("%I64d"),m_pTransInfo->lScore);

	TCHAR szScore[128]=TEXT("");
	m_DlgInsureItem.SwitchScoreString(m_pTransInfo->lScore,szScore,CountArray(szScore));

	TCHAR szCurrentDateTime[128]=TEXT("");     
	CTime nowtime;     
	nowtime = CTime::GetCurrentTime();
	_stprintf(szCurrentDateTime, TEXT("%4d-%.2d-%.2d  %.2d:%.2d"),     
		nowtime.GetYear(), nowtime.GetMonth(), nowtime.GetDay(),     
		nowtime.GetHour(), nowtime.GetMinute()); 
    
	pDC->TextOut(160,59,m_pTransInfo->szSrcNickName);
	pDC->TextOut(160,83,strSrcID);
	pDC->TextOut(160,105,m_pTransInfo->szTagNickName);
	pDC->TextOut(160,128,strTagID);
	pDC->TextOut(160,151,strScore);
	pDC->TextOut(160,174,szScore);
	pDC->TextOut(160,199,szCurrentDateTime);
}
HBITMAP CDlgTransferResult::CopyScreenToBitmap(LPRECT lpRect)
{
	HDC hScrDC, hMemDC;
	// 屏幕和内存设备描述表
	CBitmap bm;
	//为屏幕创建设备描述表
	hScrDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	//为屏幕设备描述表创建兼容的内存设备描述表
	hMemDC = CreateCompatibleDC(hScrDC);
	// 屏幕分辨率
	int xScrn, yScrn;
	// 获得屏幕分辨率
	xScrn = GetDeviceCaps(hScrDC, HORZRES);
	yScrn = GetDeviceCaps(hScrDC, VERTRES);
	// 选定区域坐标
	int nX, nY, nX2, nY2; 
	nX = lpRect->left;
	nY = lpRect->top;
	nX2 = lpRect->right;
	nY2 = lpRect->bottom;
	//确保选定区域是可见的
	if(nX < 0)
		nX = 0;
	if(nY < 0)
		nY = 0;
	if(nX2 > xScrn)
		nX2 = xScrn;
	if(nY2 > yScrn)
		nY2 = yScrn;
	// 位图宽度和高度
	int nWidth, nHeight; 
	nWidth = nX2 - nX;
	nHeight = nY2 - nY;
	// 位图句柄
	HBITMAP hBitmap, hOldBitmap; 
	// 创建一个与屏幕设备描述表兼容的位图
	hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);
	// 把新位图选到内存设备描述表中
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	// 把屏幕设备描述表拷贝到内存设备描述表中
	BitBlt(hMemDC, 0, 0, nWidth, nHeight,hScrDC, nX, nY, SRCCOPY);
	//得到屏幕位图的句柄
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	//清除 
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);
	// 返回位图句柄
	return hBitmap;
}
///////////////////////////////////////////////////////
bool CDlgTransferResult::SaveToCliboard(HBITMAP hBitmap)
{
	//得到屏幕位图句柄以后,我们可以把屏幕内容粘贴到剪贴板上。
	if (OpenClipboard()) 
		//hWnd为程序窗口句柄
	{
		//清空剪贴板
		EmptyClipboard();
		//把屏幕内容粘贴到剪贴板上,hBitmap 为刚才的屏幕位图句柄
		SetClipboardData(CF_BITMAP, hBitmap);
		//关闭剪贴板
		CloseClipboard();
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////////////

BOOL CDlgTransferResult::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SetWindowText(TEXT("赠送凭证"));

	SetWindowPos(NULL,0,0,464,294,SWP_NOMOVE);

	return TRUE;  
}
