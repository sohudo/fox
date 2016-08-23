#include "Stdafx.h"
#include "SkinDialog.h"
#include ".\skindialog.h"

//////////////////////////////////////////////////////////////////////////
//静态变量

CSkinDialogAttribute				CSkinDialog::m_SkinAttribute;		//资源变量

//////////////////////////////////////////////////////////////////////////
//宏定义

//按钮标识
#define BST_CLOSE					0									//关闭按钮
#define BST_MAX_SIZE				1									//还原按钮
#define BST_MIN_SIZE				2									//缩小按钮

//按钮状态
#define BUT_EXIST					0x01								//按钮存在
#define BUT_NORMAL					0x02								//按钮正常
#define BUT_DISABLE					0x04								//按钮禁止

#define ICON_WIDTH					20									//ICON宽度
#define ICON_HEIGHT					20									//ICON高度

#define X_BORDER					2									//边框宽度
#define Y_BORDER					2									//边框高度

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSkinDialog, CDialog)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	ON_WM_ACTIVATE()
	ON_WM_ERASEBKGND()
	ON_WM_ACTIVATEAPP()
	ON_WM_NCHITTEST()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETTINGCHANGE()
	ON_WM_WINDOWPOSCHANGED()
	ON_MESSAGE(WM_SETTEXT,OnSetTextMesage)
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinDialogAttribute::CSkinDialogAttribute()
{
	//控制信息
	m_bShowIcon=false;
	m_bShowTitle=true;
	m_hDefaultIcon=NULL;

	//偏移信息
	m_nXIconExcursion=0;
	m_nYIconExcursion=0;
	m_nXButtonExcursion=0;
	m_nYButtonExcursion=0;
	m_nXTitleTextExcursion=0;
	m_nYTitleTextExcursion=0;

	//边框信息
	m_crTitleText=RGB(0,0,0);
	m_crBackGround=RGB(0,0,0);
	m_crControlText=RGB(0,0,0);

	//辅助变量
	m_nButtonWidth=0;
	m_nButtonHeigth=0;

	//边框变量
	m_nXBorder=0;
	m_nYBorder=0;
	m_nCaptionHeigth=0;

	return;
}
	
//析构函数
CSkinDialogAttribute::~CSkinDialogAttribute()
{
}
//
////更新资源
//bool CSkinDialogAttribute::UpdateSkinResource(ISkinResource * pISkinResource)
//{
//	//获取信息
//	tagSkinDialogResource SkinDialogResource;
//	HINSTANCE hResResource=pISkinResource->GetResInstance();
//
//	//获取资源
//	if (pISkinResource!=NULL) pISkinResource->GetDialogResource(SkinDialogResource);
//
//	//控制信息
//	m_bShowIcon=SkinDialogResource.bShowIcon;
//	m_bShowTitle=SkinDialogResource.bShowTitle;
//	m_hDefaultIcon=SkinDialogResource.hDefaultIcon;
//
//	//偏移信息
//	m_nXIconExcursion=SkinDialogResource.nXIconExcursion;
//	m_nYIconExcursion=SkinDialogResource.nYIconExcursion;
//	m_nXButtonExcursion=SkinDialogResource.nXButtonExcursion;
//	m_nYButtonExcursion=SkinDialogResource.nYButtonExcursion;
//	m_nXTitleTextExcursion=SkinDialogResource.nXTitleTextExcursion;
//	m_nYTitleTextExcursion=SkinDialogResource.nYTitleTextExcursion;
//
//	//配色信息
//	m_crTitleText=SkinDialogResource.crTitleText;
//	m_crBackGround=SkinDialogResource.crBackGround;
//	m_crControlText=SkinDialogResource.crControlText;
//
//	//标题信息
//	m_SkinImageTL.SetLoadInfo(SkinDialogResource.uImageTL,hResResource);
//	m_SkinImageTM.SetLoadInfo(SkinDialogResource.uImageTM,hResResource);
//	m_SkinImageTR.SetLoadInfo(SkinDialogResource.uImageTR,hResResource);
//	m_SkinImageMR.SetLoadInfo(SkinDialogResource.uImageMR,hResResource);
//	m_SkinImageML.SetLoadInfo(SkinDialogResource.uImageML,hResResource);
//	m_SkinImageBL.SetLoadInfo(SkinDialogResource.uImageBL,hResResource);
//	m_SkinImageBM.SetLoadInfo(SkinDialogResource.uImageBM,hResResource);
//	m_SkinImageBR.SetLoadInfo(SkinDialogResource.uImageBR,hResResource);
//
//	//按钮信息
//	m_ButtonMin.SetLoadInfo(SkinDialogResource.uButtonMin,hResResource);
//	m_ButtonMax.SetLoadInfo(SkinDialogResource.uButtonMax,hResResource);
//	m_ButtonRes.SetLoadInfo(SkinDialogResource.uButtonRes,hResResource);
//	m_ButtonClose.SetLoadInfo(SkinDialogResource.uButtonClose,hResResource);
//
//	//创建画刷
//	m_brBackGround.DeleteObject();
//	m_brBackGround.CreateSolidBrush(m_crBackGround);
//
//	//加载资源
//	CImageHandle ButtonHandle(&m_ButtonClose);
//	CImageHandle SkinImageTMHandle(&m_SkinImageTM);
//	CImageHandle SkinImageMLHandle(&m_SkinImageML);
//	CImageHandle SkinImageBMHandle(&m_SkinImageBM);
//
//	//计算大小
//	m_nXBorder=m_SkinImageML.GetWidth();
//	m_nYBorder=m_SkinImageBM.GetHeight();
//	m_nButtonWidth=m_ButtonClose.GetWidth()/4;
//	m_nButtonHeigth=m_ButtonClose.GetHeight();
//	m_nCaptionHeigth=m_SkinImageTM.GetHeight();
//
//	return true;
//}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinDialog::CSkinDialog(UINT nIDTemplate, CWnd * pParentWnd) : CDialog(nIDTemplate,pParentWnd)
{
	//窗口状态
	m_bActive=false;
	m_bMaxShow=false;

	//按钮状态
	m_cbNowHotButton=255;
	m_cbNowDownButton=255;
	ZeroMemory(m_cbButtonState,sizeof(m_cbButtonState));

	return;
}

//析构函数
CSkinDialog::~CSkinDialog()
{
}

//初始化消息
BOOL CSkinDialog::OnInitDialog()
{
	__super::OnInitDialog();

	//设置变量
	m_bActive=false;
	m_bMaxShow=false;

	//按钮状态
	m_cbNowHotButton=255;
	m_cbNowDownButton=255;
	ZeroMemory(m_cbButtonState,sizeof(m_cbButtonState));

	//计算属性
	LONG lWindowStyte=GetWindowLongPtr(m_hWnd,GWL_STYLE);
	if (lWindowStyte&WS_SYSMENU)
	{
		m_cbButtonState[BST_CLOSE]=BUT_EXIST|BUT_NORMAL;
		if ((lWindowStyte&WS_MAXIMIZEBOX)||(lWindowStyte&WS_MINIMIZEBOX))
		{
			m_cbButtonState[BST_MAX_SIZE]=BUT_EXIST|((lWindowStyte&WS_MAXIMIZEBOX)?BUT_NORMAL:BUT_DISABLE);
			m_cbButtonState[BST_MIN_SIZE]=BUT_EXIST|((lWindowStyte&WS_MINIMIZEBOX)?BUT_NORMAL:BUT_DISABLE);
		}
	}

	//设置属性
	ModifyStyle(WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU|WS_TILED,0,WS_CLIPCHILDREN|WS_CLIPSIBLINGS);

	//计算位置
	CRect rcClient;
	GetClientRect(&rcClient);
	GetWindowRect(&m_rcNormalSize);
	AdjustTitleButton(rcClient.Width(),rcClient.Height());

	//设置背景
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	return TRUE;
}

//最大窗口
VOID CSkinDialog::MaxSizeWindow()
{
	//状态判断
	if (m_bMaxShow==true) return;

	//计算位置
	CRect rcClient;
	GetClientRect(&rcClient);
	ClientToScreen(&rcClient);
	GetWindowRect(&m_rcNormalSize);

	//计算位置
	CRect rcRect;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rcRect,SPIF_SENDCHANGE);
	rcRect.top-=(rcClient.top-m_rcNormalSize.top);
	rcRect.left-=(rcClient.left-m_rcNormalSize.left);
	rcRect.right+=(m_rcNormalSize.right-rcClient.right);
	rcRect.bottom+=(m_rcNormalSize.bottom-rcClient.bottom);

	//移动窗口
	m_bMaxShow=true;
	SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOCOPYBITS);

	return;
}

//还原窗口
VOID CSkinDialog::RestoreWindow()
{
	//状态判断
	if (m_bMaxShow==false) return;

	//移动窗口
	m_bMaxShow=false;
	SetWindowPos(NULL,m_rcNormalSize.left,m_rcNormalSize.top,m_rcNormalSize.Width(),m_rcNormalSize.Height(),SWP_NOZORDER|SWP_NOCOPYBITS);

	return;
}

//按钮控制
VOID CSkinDialog::EnableButton(UINT uButtonStyle)
{
	//设置按钮
	if ((uButtonStyle&WS_MAXIMIZEBOX)||(uButtonStyle&WS_MINIMIZEBOX))
	{
		m_cbButtonState[BST_MAX_SIZE]=BUT_EXIST|((uButtonStyle&WS_MAXIMIZEBOX)?BUT_NORMAL:BUT_DISABLE);
		m_cbButtonState[BST_MIN_SIZE]=BUT_EXIST|((uButtonStyle&WS_MINIMIZEBOX)?BUT_NORMAL:BUT_DISABLE);
	}
	else
	{
		m_cbButtonState[BST_MAX_SIZE]=0;
		m_cbButtonState[BST_MIN_SIZE]=0;
	}

	//更新界面
	if (m_hWnd!=NULL) Invalidate();

	return;
}

//更新标题
VOID CSkinDialog::UpdateCaption()
{
	//获取区域
	CRect rcClient;
	GetClientRect(&rcClient);

	//状态判断
	if (rcClient.IsRectEmpty()) return;

	//绘画标题
	CClientDC ClientDC(this);
	DrawCaption(&ClientDC);

	return;
}

//调整按钮
VOID CSkinDialog::AdjustTitleButton(INT nWidth, INT nHeight)
{
	//调整按钮
	for (INT i=0;i<CountArray(m_rcButton);i++)
	{
		//计算偏移
		INT nYPos=m_SkinAttribute.m_nYButtonExcursion;
		INT nXPos=nWidth-m_SkinAttribute.m_nButtonWidth*(i+1)-m_SkinAttribute.m_nXButtonExcursion;

		//计算位置
		m_rcButton[i].top=nYPos;
		m_rcButton[i].left=nXPos;
		m_rcButton[i].right=nXPos+m_SkinAttribute.m_nButtonWidth;
		m_rcButton[i].bottom=nYPos+m_SkinAttribute.m_nButtonHeigth;
	}

	return;
}

//绘画边框
VOID CSkinDialog::DrawBorder(CDC * pDC)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);
	if (rcClient.IsRectEmpty()) return;

	//加载资源
	//CImageHandle ImageHandleMR(&m_SkinAttribute.m_SkinImageMR);
	//CImageHandle ImageHandleML(&m_SkinAttribute.m_SkinImageML);
	//CImageHandle ImageHandleBL(&m_SkinAttribute.m_SkinImageBL);
	//CImageHandle ImageHandleBM(&m_SkinAttribute.m_SkinImageBM);
	//CImageHandle ImageHandleBR(&m_SkinAttribute.m_SkinImageBR);

	//边框框架
	INT nXDrawPos,nYDrawPos,nDrawStep;

	//绘画左右
	nDrawStep=ImageHandleMR->GetHeight();
	nXDrawPos=rcClient.right-m_SkinAttribute.m_nXBorder;
	for (nYDrawPos=m_SkinAttribute.m_nCaptionHeigth;nYDrawPos<(rcClient.bottom-m_SkinAttribute.m_nYBorder);nYDrawPos+=nDrawStep)
	{
		ImageHandleML->BitBlt(pDC->m_hDC,0,nYDrawPos);
		ImageHandleMR->BitBlt(pDC->m_hDC,nXDrawPos,nYDrawPos);
	}

	//绘画下边
	nDrawStep=ImageHandleBM->GetWidth();
	nYDrawPos=rcClient.bottom-m_SkinAttribute.m_nYBorder;
	ImageHandleBL->BitBlt(pDC->m_hDC,0,rcClient.bottom-ImageHandleBL->GetHeight());
	for (nXDrawPos=ImageHandleBL->GetWidth();nXDrawPos<rcClient.right-m_SkinAttribute.m_nXBorder;nXDrawPos+=nDrawStep)
	{
		ImageHandleBM->BitBlt(pDC->m_hDC,nXDrawPos,nYDrawPos);
	}
	ImageHandleBR->BitBlt(pDC->m_hDC,rcClient.right-ImageHandleBR->GetWidth(),rcClient.bottom-ImageHandleBR->GetHeight());
	
	return;
}

//绘画标题
VOID CSkinDialog::DrawCaption(CDC * pDC)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);
	if (rcClient.IsRectEmpty()) return;

	//加载资源
	CImageHandle ImageHandleTL(&m_SkinAttribute.m_SkinImageTL);
	CImageHandle ImageHandleTM(&m_SkinAttribute.m_SkinImageTM);
	CImageHandle ImageHandleTR(&m_SkinAttribute.m_SkinImageTR);

	//建立缓冲
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),m_SkinAttribute.m_nCaptionHeigth,16);

	//绘画标题
	CDC BufferDC;
	BufferDC.CreateCompatibleDC(pDC);
	BufferDC.SelectObject(ImageBuffer);

	//获取宽度
	INT nMidWidth=m_SkinAttribute.m_SkinImageTM.GetWidth();
	INT nLeftWidth=m_SkinAttribute.m_SkinImageTL.GetWidth();
	INT nRightWidth=m_SkinAttribute.m_SkinImageTR.GetWidth();

	//绘画背景
	ImageHandleTL->BitBlt(BufferDC,0,0);
	for (INT i=nLeftWidth;i<(rcClient.right-nRightWidth);i+=nMidWidth) ImageHandleTM->BitBlt(BufferDC,i,0);
	ImageHandleTR->BitBlt(BufferDC,rcClient.right-nRightWidth,0);

	//绘画图标
	if (m_SkinAttribute.m_bShowIcon==true)
	{
		//加载图标
		HICON hIcon=GetIcon(FALSE);
		if (hIcon==NULL) hIcon=m_SkinAttribute.m_hDefaultIcon;

		//绘画图标
		if (hIcon!=NULL)
		{
			INT nXPos=m_SkinAttribute.m_nXIconExcursion;
			INT nYPos=m_SkinAttribute.m_nYIconExcursion;
			DrawIconEx(BufferDC,nXPos,nYPos,hIcon,ICON_WIDTH,ICON_HEIGHT,0,NULL,DI_NORMAL);
		}
	}
	
	//绘画标题
	if ((m_SkinAttribute.m_bShowTitle==true)&&(m_ImageTitle.IsNull()==true))
	{
		//获取标题
		CString strTitle;
		GetWindowText(strTitle);

		//计算区域
		CRect rcTitle;
		rcTitle.right=rcClient.right-nRightWidth;
		rcTitle.bottom=m_SkinAttribute.m_nCaptionHeigth;
		rcTitle.top=m_SkinAttribute.m_nYTitleTextExcursion;
		rcTitle.left=m_SkinAttribute.m_nXTitleTextExcursion;

		//绘画文字
		BufferDC.SetBkMode(TRANSPARENT);
		BufferDC.SetTextColor(m_SkinAttribute.m_crTitleText);
		BufferDC.SelectObject(CSkinResourceManager::GetDefaultFont());
		BufferDC.DrawText(strTitle,&rcTitle,DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS);
	}

	//绘画标题
	if (m_ImageTitle.IsNull()==false)
	{
		INT nYPos=(CY_DEF_FONT-m_ImageTitle.GetHeight())/2;
		m_ImageTitle.DrawImage(&BufferDC,m_SkinAttribute.m_nXTitleTextExcursion,m_SkinAttribute.m_nYTitleTextExcursion+nYPos);
	}

	//关闭按钮
	if (m_cbButtonState[BST_CLOSE]&BUT_EXIST)
	{
		INT nDrawPos=0;
		CImageHandle ImageHandle(&m_SkinAttribute.m_ButtonClose);
		if (m_cbButtonState[BST_CLOSE]&BUT_DISABLE) nDrawPos=ImageHandle->GetWidth()*3/4;
		else if (m_cbNowDownButton==BST_CLOSE) nDrawPos=ImageHandle->GetWidth()/4;
		else if (m_cbNowHotButton==BST_CLOSE) nDrawPos=ImageHandle->GetWidth()*2/4;
		ImageHandle->BitBlt(BufferDC,m_rcButton[BST_CLOSE].left,m_rcButton[BST_CLOSE].top,
			m_SkinAttribute.m_nButtonWidth,m_SkinAttribute.m_nButtonHeigth,nDrawPos,0);
	}

	//最大化和还原按钮
	if (m_cbButtonState[BST_MAX_SIZE]&BUT_EXIST)
	{
		INT nDrawPos=0;
		CImageHandle ImageHandle;
		if (m_bMaxShow==true) ImageHandle.AttachResource(&m_SkinAttribute.m_ButtonRes);
		else ImageHandle.AttachResource(&m_SkinAttribute.m_ButtonMax);
		if (m_cbButtonState[BST_MAX_SIZE]&BUT_DISABLE) nDrawPos=ImageHandle->GetWidth()*3/4;
		if (m_cbNowDownButton==BST_MAX_SIZE) nDrawPos=ImageHandle->GetWidth()/4;
		else if (m_cbNowHotButton==BST_MAX_SIZE) nDrawPos=ImageHandle->GetWidth()*2/4;
		ImageHandle->BitBlt(BufferDC,m_rcButton[BST_MAX_SIZE].left,m_rcButton[BST_MAX_SIZE].top,
			m_SkinAttribute.m_nButtonWidth,m_SkinAttribute.m_nButtonHeigth,nDrawPos,0);
	}

	//最小化按钮
	if (m_cbButtonState[BST_MIN_SIZE]&BUT_EXIST)	
	{
		INT nDrawPos=0;
		CImageHandle ImageHandle(&m_SkinAttribute.m_ButtonMin);
		if (m_cbButtonState[BST_MIN_SIZE]&BUT_DISABLE) nDrawPos=ImageHandle->GetWidth()*3/4;
		if (m_cbNowDownButton==BST_MIN_SIZE) nDrawPos=ImageHandle->GetWidth()/4;
		else if (m_cbNowHotButton==BST_MIN_SIZE) nDrawPos=ImageHandle->GetWidth()*2/4;
		ImageHandle->BitBlt(BufferDC,m_rcButton[BST_MIN_SIZE].left,m_rcButton[BST_MIN_SIZE].top,
			m_SkinAttribute.m_nButtonWidth,m_SkinAttribute.m_nButtonHeigth,nDrawPos,0);
	}

	//绘画界面
	pDC->BitBlt(0,0,rcClient.Width(),m_SkinAttribute.m_nCaptionHeigth,&BufferDC,0,0,SRCCOPY);

	//清理资源
	BufferDC.DeleteDC();
	ImageBuffer.Destroy();

	return;	
}

//绘画背景
VOID CSkinDialog::DrawBackGround(CDC * pDC)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);
	if (rcClient.IsRectEmpty()) return;

	//填充背景
	pDC->FillSolidRect(rcClient.left+m_SkinAttribute.m_nXBorder,m_SkinAttribute.m_nCaptionHeigth,
		rcClient.right-m_SkinAttribute.m_nXBorder*2,rcClient.bottom-m_SkinAttribute.m_nCaptionHeigth-m_SkinAttribute.m_nYBorder,
		m_SkinAttribute.m_crBackGround);

	return;
}

//激活消息
VOID CSkinDialog::OnActivate(UINT nState, CWnd * pWndOther, BOOL bMinimized)
{
	__super::OnActivate(nState,pWndOther,bMinimized);

	//设置变量
	m_bActive=(nState!=WA_INACTIVE);

	return;
}

//激活消息
VOID CSkinDialog::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	__super::OnActivateApp(bActive,dwThreadID);

	//设置变量
	m_bActive=bActive?true:false;

	return;
}

//鼠标消息
VOID CSkinDialog::OnLButtonDblClk(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDblClk(nFlags,Point);

	//判断按下
	if (m_cbNowDownButton!=255) return;	

	//按钮搜索
	for (INT i=0;i<CountArray(m_cbButtonState);i++)
	{
		if ((m_cbButtonState[i]&BUT_EXIST)&&(m_rcButton[i].PtInRect(Point))) return;
	}

	//状态判断
	if (Point.y>m_SkinAttribute.m_nCaptionHeigth) return;
	if (((m_cbButtonState[BST_MAX_SIZE]&BUT_EXIST)==0)||(m_cbButtonState[BST_MAX_SIZE]&BUT_DISABLE)!=0) return;

	//控制窗口
	if (m_bMaxShow==true) RestoreWindow();
	else MaxSizeWindow();
	
	return;
}

//鼠标消息
VOID CSkinDialog::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//判断按下
	if (m_cbNowDownButton!=255) return;

	//按钮搜索
	for (INT i=0;i<CountArray(m_rcButton);i++)
	{
		if ((m_cbButtonState[i]&BUT_EXIST)&&((m_cbButtonState[i]&BUT_DISABLE)==0)&&(m_rcButton[i].PtInRect(Point)))
		{
			//设置变量
			m_cbNowDownButton=i;

			//更新界面
			SetCapture();
			UpdateCaption();

			return;
		}
	}

	//模拟按标题
	if ((m_bMaxShow==false)&&(Point.y<m_SkinAttribute.m_nCaptionHeigth))
	{
		PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));
	}

	return;
}

//鼠标消息
VOID CSkinDialog::OnLButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnLButtonUp(nFlags,Point);

	//状态判断
	if (m_cbNowDownButton==255) return;

	//锁定释放
	ReleaseCapture();

	//按钮判断
	if ((m_rcButton[m_cbNowDownButton].PtInRect(Point))&&((m_cbButtonState[m_cbNowDownButton]&BUT_DISABLE)==0))
	{
		switch (m_cbNowDownButton)
		{
		case BST_CLOSE:		//关闭按钮
			{
				m_cbNowDownButton=255;
				PostMessage(WM_CLOSE,0,0);
				return;
			}
		case BST_MAX_SIZE:		//还原按钮
			{
				m_cbNowDownButton=255;
				if (m_bMaxShow==true) RestoreWindow();
				else MaxSizeWindow();
				return;
			}
		case BST_MIN_SIZE:		//最小化按钮
			{
				m_cbNowDownButton=255;
				ShowWindow(SW_MINIMIZE);
				return;
			}
		}
	}

	//设置数据
	m_cbNowDownButton=255;

	//更新标题
	UpdateCaption();

	return;
}

//鼠标消息
VOID CSkinDialog::OnMouseMove(UINT nFlags, CPoint Point)
{
	__super::OnMouseMove(nFlags,Point);

	//设置光标
	SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW)));

	//判断按下
	if (m_cbNowDownButton!=255) return;
	
	//按钮搜索
	for (INT i=0;i<CountArray(m_rcButton);i++)
	{
		if ((m_cbButtonState[i]&BUT_EXIST)&&(m_rcButton[i].PtInRect(Point)))
		{
			if (m_cbNowHotButton==i) return;
			if (m_cbNowHotButton!=255)
			{
				m_cbButtonState[m_cbNowHotButton]|=BUT_NORMAL;
				m_cbNowHotButton=255;
			}
			if ((m_cbButtonState[i]&BUT_EXIST)&&(!(m_cbButtonState[i]&BUT_DISABLE)))
			{
				m_cbNowHotButton=i;
				m_cbButtonState[m_cbNowHotButton]&=~BUT_NORMAL;
			}

			//更新标题
			UpdateCaption();

			return;
		}
	}

	//清理焦点
	if (m_cbNowHotButton!=255)
	{
		//设置变量
		m_cbButtonState[m_cbNowHotButton]|=BUT_NORMAL;
		m_cbNowHotButton=255;

		//更新标题
		UpdateCaption();
	}

	return;
}

//位置改变
VOID CSkinDialog::OnWindowPosChanged(WINDOWPOS * lpwndpos)
{
	__super::OnWindowPosChanged(lpwndpos);

	//调整位置
	CRect rcClient;
	m_cbNowHotButton=255;
	m_cbNowDownButton=255;
	GetClientRect(&rcClient);
	AdjustTitleButton(rcClient.Width(),rcClient.Height());

	//重画界面
	Invalidate();

	return;
}

//设置改变
VOID CSkinDialog::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	__super::OnSettingChange(uFlags,lpszSection);

	//调整框架
	if (m_bMaxShow==true)
	{
		//计算位置
		CRect rcClient;
		GetClientRect(&rcClient);
		ClientToScreen(&rcClient);
		GetWindowRect(&m_rcNormalSize);

		//计算位置
		CRect rcRect;
		SystemParametersInfo(SPI_GETWORKAREA,0,&rcRect,SPIF_SENDCHANGE);
		rcRect.top-=(rcClient.top-m_rcNormalSize.top);
		rcRect.left-=(rcClient.left-m_rcNormalSize.left);
		rcRect.right+=(m_rcNormalSize.right-rcClient.right);
		rcRect.bottom+=(m_rcNormalSize.bottom-rcClient.bottom);

		//移动窗口
		SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOCOPYBITS);
	}

	return;
}

//绘画背景
BOOL CSkinDialog::OnEraseBkgnd(CDC * pDC)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//建立缓冲
	CDC BufferDC;
	CImage ImageBuffer;
	BufferDC.CreateCompatibleDC(NULL);
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//设置缓冲
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SelectObject(ImageBuffer);
	BufferDC.SelectObject(CSkinResourceManager::GetDefaultFont());

	//绘画界面
	DrawBorder(&BufferDC);
	DrawCaption(&BufferDC);
	DrawBackGround(&BufferDC);
	OnDrawClientArea(&BufferDC,rcClient.Width(),rcClient.Height());

	//绘画界面
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),&BufferDC,0,0,SRCCOPY);

	//清理资源
	BufferDC.DeleteDC();
	ImageBuffer.Destroy();
	return TRUE;
}

//按键测试
UINT CSkinDialog::OnNcHitTest(CPoint Point)
{
	//按钮测试
	if ((m_bMaxShow==false)&&(m_cbButtonState[BST_MAX_SIZE]&BUT_EXIST)&&(m_cbButtonState[BST_MAX_SIZE]&BUT_NORMAL))
	{
		//获取位置
		CRect rcClient;
		GetClientRect(&rcClient);

		//转换位置
		CPoint ClientPoint=Point;
		ScreenToClient(&ClientPoint);

		//左面位置
		if (ClientPoint.x<=X_BORDER)
		{
			if (ClientPoint.y<=Y_BORDER) return HTTOPLEFT;
			if (ClientPoint.y>=(rcClient.Height()-Y_BORDER)) return HTBOTTOMLEFT;
			return HTLEFT;
		}

		//右面位置
		if (ClientPoint.x>=(rcClient.Width()-X_BORDER))
		{
			if (ClientPoint.y<=Y_BORDER) return HTTOPRIGHT;
			if (ClientPoint.y>=(rcClient.Height()-Y_BORDER)) return HTBOTTOMRIGHT;
			return HTRIGHT;
		}

		//上下位置
		if (ClientPoint.y<=Y_BORDER) 
		{
			if( ( !(m_cbButtonState[BST_CLOSE]&BUT_EXIST) || !m_rcButton[2].PtInRect(ClientPoint) ) &&
				( !(m_cbButtonState[BST_MAX_SIZE]&BUT_EXIST) || !m_rcButton[1].PtInRect(ClientPoint) ) &&
				( !(m_cbButtonState[BST_MIN_SIZE]&BUT_EXIST) || !m_rcButton[0].PtInRect(ClientPoint) ) )
				return HTTOP;
		}
		if (ClientPoint.y>=(rcClient.Height()-Y_BORDER)) return HTBOTTOM;
	}

	return __super::OnNcHitTest(Point);
}

//控件颜色
HBRUSH CSkinDialog::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_STATIC:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetBkColor(m_SkinAttribute.m_crBackGround);
			pDC->SetTextColor(m_SkinAttribute.m_crControlText);
			return m_SkinAttribute.m_brBackGround;
		}
	}
	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}

//标题消息
LRESULT	CSkinDialog::OnSetTextMesage(WPARAM wParam, LPARAM lParam)
{
	//默认调用
	LRESULT lResult=DefWindowProc(WM_SETTEXT,wParam,lParam);

	//更新标题
	UpdateCaption();

	return lResult;
}

//获取大小
VOID CSkinDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	__super::OnGetMinMaxInfo(lpMMI);

	lpMMI->ptMinTrackSize.x = GetXBorder()*2+(m_SkinAttribute.m_bShowIcon?ICON_WIDTH:0);
	lpMMI->ptMinTrackSize.y = GetYBorder()+GetCaptionHeight();

	if( m_cbButtonState[BST_CLOSE] & BUT_EXIST )
	{
		lpMMI->ptMinTrackSize.x += m_SkinAttribute.m_nButtonWidth+m_SkinAttribute.m_nXButtonExcursion;
	}

	if( m_cbButtonState[BST_MAX_SIZE] & BUT_EXIST )
	{
		lpMMI->ptMinTrackSize.x += m_SkinAttribute.m_nButtonWidth+m_SkinAttribute.m_nXButtonExcursion;
	}

	if( m_cbButtonState[BST_MIN_SIZE] & BUT_EXIST )
	{
		lpMMI->ptMinTrackSize.x += m_SkinAttribute.m_nButtonWidth+m_SkinAttribute.m_nXButtonExcursion;
	}
}

//////////////////////////////////////////////////////////////////////////
