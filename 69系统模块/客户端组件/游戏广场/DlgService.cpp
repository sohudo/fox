#include "StdAfx.h"
#include "DlgService.h"
#include "GlobalUnits.h"
#include ".\dlgservice.h"

//////////////////////////////////////////////////////////////////////////////////
//宏定义

//尺寸定义
#define ITEM_IMAGE_CX				20									//图标宽度
#define ITEM_IMAGE_CY				20									//图标高度 

//尺寸定义
#define SERVICE_BAR_CX				150									//控制宽度
#define SERVICE_BAR_CY				375									//控制高度 

//尺寸定义
#define BAR_BT_SPACE				3									//按钮间距
#define BAR_BT_EXCURSION			5									//按钮偏移

//控件标识
#define IDC_SERVICE_BAR				100									//子项控制

#define IDC_BT_INDIVIDUAL			101									//个人设置
#define IDC_BT_PASSWORD				102									//修改密码

#define	SERVICE_PAGE_1				0									//
#define	SERVICE_PAGE_2				1									//
#define	SERVICE_PAGE_3				2									//

#define SERVICE_PAGE_COUNT			3								
//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgServiceBar, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgService, CDialog)
	//ON_BN_CLICKED(IDC_BT_INDIVIDUAL,ShowIndividualPage)
	//ON_BN_CLICKED(IDC_BT_PASSWORD,ShowPasswordPage)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CONTROL, OnTcnSelchangeLogonMode)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgServiceBar::CDlgServiceBar()
{
	//活动信息
	m_bHovering=false;
	m_wItemDown=INVALID_WORD;
	m_wItemHover=INVALID_WORD;
	m_wItemActive=INVALID_WORD;

	//子项信息
	m_SizeButton.SetSize(0,0);
	m_rcItemCreate.SetRect(0,0,0,0);

	//加载资源
	CPngImage ImageButton;
	ImageButton.LoadImage(AfxGetInstanceHandle(),TEXT("SERVICE_BAR_BT"));
	m_SizeButton.SetSize(ImageButton.GetWidth()/3,ImageButton.GetHeight());

	return;
}

//析构函数
CDlgServiceBar::~CDlgServiceBar()
{
}

//消息解释
BOOL CDlgServiceBar::PreTranslateMessage(MSG * pMsg)
{
	//消息过虑
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//设置区域
bool CDlgServiceBar::SetItemCreateRect(CRect rcItemCreate)
{
	//设置变量
	m_rcItemCreate=rcItemCreate;

	//调整控件
	for (INT_PTR i=0;i<m_DlgServiceItemArray.GetCount();i++)
	{
		CWnd * pWnd=m_DlgServiceItemArray[i];
		if (pWnd->m_hWnd!=NULL) pWnd->SetWindowPos(NULL,rcItemCreate.left,rcItemCreate.top,rcItemCreate.Width(),rcItemCreate.Height(),SWP_NOZORDER);
	}

	return true;
}

//插入子项
bool CDlgServiceBar::InsertServiceItem(CDlgServiceItem * pDlgServiceItem)
{
	//设置变量
	m_DlgServiceItemArray.Add(pDlgServiceItem);

	//更新界面
	if (m_hWnd!=NULL)
	{
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return true;
}

//激活设置
bool CDlgServiceBar::SetActiveServiceItem(CDlgServiceItem * pDlgServiceItem)
{
	//效验参数
	ASSERT(pDlgServiceItem!=NULL);
	if (pDlgServiceItem==NULL) return false;

	//变量定义
	CDlgServiceItem * pServiceActive=NULL;

	//切换判断
	if (m_wItemActive!=INVALID_WORD)
	{
		//获取子项
		ASSERT(m_wItemActive<m_DlgServiceItemArray.GetCount());
		pServiceActive=m_DlgServiceItemArray[m_wItemActive];

		//切换判断
		if (pServiceActive==pDlgServiceItem) return true;
	}

	//激活子项
	for (INT_PTR i=0;i<m_DlgServiceItemArray.GetCount();i++)
	{
		if (m_DlgServiceItemArray[i]==pDlgServiceItem)
		{
			//创建子项
			if (pDlgServiceItem->m_hWnd==NULL)
			{
				pDlgServiceItem->CreateServiceItem(GetParent(),m_rcItemCreate);
			}

			//设置变量
			m_wItemActive=(WORD)i;

			//显示窗口
			pDlgServiceItem->ShowWindow(SW_SHOW);
			pDlgServiceItem->SetFocus();

			//隐藏旧项
			if (pServiceActive->GetSafeHwnd()!=NULL)
			{
				pServiceActive->ShowWindow(SW_HIDE);
			}

			//更新界面
			RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);

			return true;
		}
	}

	return false;
}

//索引切换
WORD CDlgServiceBar::SwitchToButtonIndex(CPoint MousePoint)
{
	//窗口位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//边界判断
	if (MousePoint.x<((rcClient.Width()-m_SizeButton.cx)/2)) return INVALID_WORD;
	if (MousePoint.x>((rcClient.Width()+m_SizeButton.cx)/2)) return INVALID_WORD;

	//子项判断
	for (WORD i=0;i<m_DlgServiceItemArray.GetCount();i++)
	{
		//位置计算
		CRect rcButton;
		rcButton.left=(rcClient.Width()-m_SizeButton.cx)/2;
		rcButton.right=(rcClient.Width()+m_SizeButton.cx)/2;
		rcButton.top=(m_SizeButton.cy+BAR_BT_SPACE)*i+BAR_BT_EXCURSION;
		rcButton.bottom=(m_SizeButton.cy+BAR_BT_SPACE)*i+m_SizeButton.cy+BAR_BT_EXCURSION;

		//区域判断
		if (rcButton.PtInRect(MousePoint)) return i;
	}

	return INVALID_WORD;
}

//重画消息
VOID CDlgServiceBar::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//创建缓冲
	CBitImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//创建设备
	CImageDC BufferDC(ImageBuffer);
	CDC * pBufferDC=CDC::FromHandle(BufferDC);

	//设置设备
	pBufferDC->SetBkMode(TRANSPARENT);
	pBufferDC->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

	//加载资源
	CPngImage ImageButton;
	CPngImage ImageBarLogo;
	CPngImage ImageBarImage;
	ImageButton.LoadImage(AfxGetInstanceHandle(),TEXT("SERVICE_BAR_BT"));
	ImageBarLogo.LoadImage(AfxGetInstanceHandle(),TEXT("SERVICE_BAR_LOGO"));
	ImageBarImage.LoadImage(AfxGetInstanceHandle(),TEXT("SERVICE_BAR_IMAGE"));

	//绘画背景
	pBufferDC->FillSolidRect(&rcClient,RGB(223,236,246));
	ImageBarLogo.DrawImage(pBufferDC,rcClient.Width()-ImageBarLogo.GetWidth(),rcClient.Height()-ImageBarLogo.GetHeight());

	//绘画按钮
	for (INT i=0;i<m_DlgServiceItemArray.GetCount();i++)
	{
		//获取标题
		CDlgServiceItem * pDlgServiceItem=m_DlgServiceItemArray[i];
		LPCTSTR pszServiceName=pDlgServiceItem->GetServiceItemName();

		//横行计算
		INT nXButtomPos=0;
		if ((m_wItemHover==i)&&(m_wItemDown!=i)) nXButtomPos=m_SizeButton.cx;
		if ((m_wItemActive==i)||(m_wItemDown==i)) nXButtomPos=m_SizeButton.cx*2;

		//绘画按钮
		INT nXDrawPos=(rcClient.Width()-m_SizeButton.cx)/2;
		INT nYDrawPos=(m_SizeButton.cy+BAR_BT_SPACE)*i+BAR_BT_EXCURSION;
		ImageButton.DrawImage(pBufferDC,nXDrawPos,nYDrawPos,m_SizeButton.cx,m_SizeButton.cy,nXButtomPos,0L);

		//绘画图标
		INT nXImagePos=pDlgServiceItem->m_cbImageIndex*ITEM_IMAGE_CX;
		ImageBarImage.DrawImage(pBufferDC,nXDrawPos+32,nYDrawPos+2,ITEM_IMAGE_CX,ITEM_IMAGE_CY,nXImagePos,0);

		//设置颜色
		if ((m_wItemHover!=i)&&(m_wItemDown!=i)) pBufferDC->SetTextColor(RGB(0,0,0));
		if ((m_wItemHover==i)&&(m_wItemDown!=i)) pBufferDC->SetTextColor(RGB(3,112,218));
		if ((m_wItemActive==i)||(m_wItemDown==i)) pBufferDC->SetTextColor(RGB(255,255,255));

		//绘画字符
		CRect rcItem(nXDrawPos+25,nYDrawPos+1,m_SizeButton.cx+nXDrawPos,m_SizeButton.cy+nYDrawPos);
		pBufferDC->DrawText(pszServiceName,lstrlen(pszServiceName),rcItem,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	}

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),pBufferDC,0,0,SRCCOPY);

	return;
}

//建立消息
INT CDlgServiceBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//设置变量
	m_bHovering=false;
	m_wItemDown=INVALID_WORD;
	m_wItemHover=INVALID_WORD;
	m_wItemActive=INVALID_WORD;

	//子项信息
	m_rcItemCreate.SetRect(0,0,0,0);
	m_DlgServiceItemArray.RemoveAll();

	return 0;
}

//鼠标消息
VOID CDlgServiceBar::OnMouseMove(UINT nFlags, CPoint Point)
{
	__super::OnMouseMove(nFlags, Point);

	//窗口位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//位置计算
	WORD wServiceHover=SwitchToButtonIndex(Point);

	//盘旋变量
	if (wServiceHover!=m_wItemHover)
	{
		//设置变量
		m_wItemHover=wServiceHover;

		//更新界面
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	//进入判断
	if (m_bHovering==false)
	{
		//设置变量
		m_bHovering=true;

		//变量定义
		TRACKMOUSEEVENT TrackMouseEvent;
		ZeroMemory(&TrackMouseEvent,sizeof(TrackMouseEvent));

		//注册消息
		TrackMouseEvent.hwndTrack=m_hWnd;
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);

		//注册事件
		_TrackMouseEvent(&TrackMouseEvent);
	}

	return;
}

//鼠标消息
VOID CDlgServiceBar::OnLButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnLButtonUp(nFlags,Point);

	//激活子项
	if ((m_wItemHover!=INVALID_WORD)&&(m_wItemHover==m_wItemDown))
	{
		SetActiveServiceItem(m_DlgServiceItemArray[m_wItemHover]);
	}

	//按起处理
	if (m_wItemDown!=INVALID_WORD)
	{
		//释放鼠标
		ReleaseCapture();

		//设置变量
		m_wItemDown=INVALID_WORD;

		//更新界面
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return;
}

//鼠标消息
VOID CDlgServiceBar::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//设置焦点
	SetFocus();

	//按下处理
	if ((m_wItemHover!=INVALID_WORD)&&(m_wItemHover!=m_wItemActive))
	{
		//鼠标扑获
		SetCapture();

		//设置变量
		m_wItemDown=m_wItemHover;

		//更新界面
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return;
}

//鼠标消息
LRESULT CDlgServiceBar::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	if (m_bHovering==true)
	{
		//设置变量
		m_bHovering=false;
		m_wItemHover=INVALID_WORD;

		//更新界面
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return 0;
}

//光标消息
BOOL CDlgServiceBar::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//设置光标
	if (m_wItemHover!=INVALID_WORD)
	{
		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HAND_CUR)));
		return TRUE;
	}

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgService::CDlgService() : CDialog(IDD_DLG_SERVICE)
{
	m_wInitPage=SERVICE_PAGE_1;

	return;
}

//析构函数
CDlgService::~CDlgService()
{
}


void CDlgService::OnLButtonDown(UINT nFlags, CPoint point)
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

//消息解释
BOOL CDlgService::PreTranslateMessage(MSG * pMsg)
{
	//按键过虑
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//创建函数
BOOL CDlgService::OnInitDialog()
{
	__super::OnInitDialog();


	//设置标题
	//SetWindowText(TEXT("个人设置"));

	m_ImageBack.LoadImage(AfxGetInstanceHandle(),TEXT("SERVICE_BACK"));

	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE);


	//设置控件
	m_TabControl.SetItemSize(CSize(150,25));
	m_TabControl.InsertItem(SERVICE_PAGE_1,TEXT("个人信息"));
	m_TabControl.InsertItem(SERVICE_PAGE_2,TEXT("绑定机器"));
	m_TabControl.InsertItem(SERVICE_PAGE_3,TEXT("修改密码"));

	//创建按钮
	CRect rcCreate(0,0,0,0);
	m_btClose.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDCANCEL);

	m_DlgServiceIndividual.Create(IDD_DLG_SERVICE_INDIVIDUAL,this);
	//m_DlgServicePassword.Create(IDD_DLG_SERVICE_PASSWORD,this);
	m_DlgServicePassword.Create(IDD_DLG_MODIFY_PASSWORD,this);

	m_DlgLockMachine.Create(IDD_DLG_LOCK_MACHINE,this);

	//调整位置
	CRect rcTabCtrl(14,36,m_ImageBack.GetWidth()-110,36+25);
	m_DlgLockMachine.SetWindowPos(NULL,14,63,0,0,SWP_NOZORDER|SWP_NOSIZE);
	m_DlgServiceIndividual.SetWindowPos(NULL,14,63,0,0,SWP_NOZORDER|SWP_NOSIZE);
	m_DlgServicePassword.SetWindowPos(NULL,14,63,0,0,SWP_NOZORDER|SWP_NOSIZE);
	m_TabControl.SetWindowPos(NULL,rcTabCtrl.left,rcTabCtrl.top,rcTabCtrl.Width(),rcTabCtrl.Height(),SWP_NOZORDER);

	m_TabControl.SetTabCtrlColor(RGB(196,219,231),RGB(131,192,219));

	if(m_wInitPage>=SERVICE_PAGE_COUNT) m_wInitPage=SERVICE_PAGE_1;

	switch(m_wInitPage)
	{
	case SERVICE_PAGE_1:	{	ShowIndividualPage(); break;	}
	case SERVICE_PAGE_2:	{	ShowLockMachine(); break;	}
	case SERVICE_PAGE_3:	{	ShowPasswordPage(); break;	}

	}

	//设置选择
	m_TabControl.SetCurSel(m_wInitPage);

	//设置按钮
	HINSTANCE hInstance=AfxGetInstanceHandle();

	m_btClose.SetButtonImage(IDB_BT_SERVICE_CLOSE,hInstance,false,false);
	m_btClose.SetWindowPos(NULL,m_ImageBack.GetWidth()-40,3,0,0,SWP_NOZORDER|SWP_NOSIZE);


	//ShowIndividualPage();
	

	return TRUE;
}

//确定消息
VOID CDlgService::OnOK()
{
	return;
}

//绘画消息
VOID CDlgService::OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight)
{
//	pDC->Draw3dRect(8,63,457-8,257-63+40,RGB(210,210,210),RGB(210,210,210));
	//if(m_ImageBack.IsNull()!=true)	m_ImageBack.DrawImage(pDC,0,0);
}

VOID CDlgService::ShowIndividualPage()
{

	m_DlgServicePassword.ShowWindow(SW_HIDE);
	m_DlgLockMachine.ShowWindow(SW_HIDE);
	m_DlgServiceIndividual.ShowWindow(SW_SHOW);
}

VOID CDlgService::ShowPasswordPage()
{
	m_DlgServiceIndividual.ShowWindow(SW_HIDE);
	m_DlgLockMachine.ShowWindow(SW_HIDE);

	m_DlgServicePassword.ShowWindow(SW_SHOW);
}

VOID CDlgService::ShowLockMachine()
{
	m_DlgServiceIndividual.ShowWindow(SW_HIDE);
	m_DlgLockMachine.ShowWindow(SW_SHOW);

	m_DlgServicePassword.ShowWindow(SW_HIDE);
}
//////////////////////////////////////////////////////////////////////////

BOOL CDlgService::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if(m_ImageBack.IsNull()!=true)	m_ImageBack.DrawImage(pDC,0,0);
	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}

void CDlgService::DoDataExchange(CDataExchange* pDX)
{
	// TODO: 在此添加专用代码和/或调用基类

	DDX_Control(pDX, IDC_TAB_CONTROL, m_TabControl);

	CDialog::DoDataExchange(pDX);
}

//类型改变
VOID CDlgService::OnTcnSelchangeLogonMode(NMHDR * pNMHDR, LRESULT * pResult)
{
	switch (m_TabControl.GetCurSel())
	{
	case SERVICE_PAGE_1:		//个人信息
		{
			ShowIndividualPage();

			//设置选择
			m_TabControl.SetCurSel(SERVICE_PAGE_1);

			break;
		}
	case SERVICE_PAGE_2:		//绑定机器
		{
			ShowLockMachine();
			//设置选择
			m_TabControl.SetCurSel(SERVICE_PAGE_2);

			break;
		}
	case SERVICE_PAGE_3:		//修改密码
		{
			ShowPasswordPage();
			//设置选择
			m_TabControl.SetCurSel(SERVICE_PAGE_3);

			break;
		}
	default:
		{
			ASSERT(FALSE);
		}
	}

	return;
}
