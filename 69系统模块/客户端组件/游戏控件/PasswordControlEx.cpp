#include "StdAfx.h"
#include "Winable.h"
#include "Resource.h"
#include "PasswordControlEx.h"

//////////////////////////////////////////////////////////////////////////////////

//位置定义
#define ITEM_POS_S					3									//按钮间距
#define ITEM_POS_X					0									//按钮位置
#define ITEM_POS_Y					0									//按钮位置

//关闭按钮
#define POS_BUTTON_X				5									//按钮位置
#define POS_BUTTON_Y				8									//按钮位置

//常量定义
#define ROW_BACK					0									//退格按钮
#define ROW_CLEAN					1									//切换按钮
#define ROW_CAPITAL					2									//大写按钮
#define ROW_CLOSE_KEY				3									//关闭按钮
#define LINE_FUNCTION				4									//功能按钮

//控件标识
#define IDC_BT_KEYBOARD				100									//键盘按钮
#define IDC_ED_PASSWORD				200									//密码控件

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CPasswordKeyboard2, CDialog)
	ON_WM_NCPAINT()
	ON_WM_KILLFOCUS()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CPasswordKeyboard2::CPasswordKeyboard2() : CDialog(IDD_PASSWORD_KEYBOARD2)
{
	//设置变量
	m_nRandLine[0]=rand()%14;
	m_nRandLine[1]=rand()%14;
	m_nRandLine[2]=rand()%8;

	//键盘状态
	//m_bCapsLockStatus=false;
	m_bCapsLockStatus=(GetKeyState(VK_CAPITAL)&0x0F)>0;

	m_pKeyBoardSink=NULL;

	//状态变量
	m_bMouseDown=false;
	m_wHoverRow=INVALID_WORD;
	m_wHoverLine=INVALID_WORD;

	//键盘字符
	m_nItemCount[0]=14;
	lstrcpyn(m_szKeyboradChar[0][0],TEXT("abcd1234567890"),CountArray(m_szKeyboradChar[0][0]));
	lstrcpyn(m_szKeyboradChar[0][1],TEXT("ABCD1234567890"),CountArray(m_szKeyboradChar[0][1]));

	//键盘字符
	m_nItemCount[1]=14;
	lstrcpyn(m_szKeyboradChar[1][0],TEXT("efghijklmnopqr"),CountArray(m_szKeyboradChar[1][0]));
	lstrcpyn(m_szKeyboradChar[1][1],TEXT("EFGHIJKLMNOPQR"),CountArray(m_szKeyboradChar[1][1]));

	//键盘字符
	m_nItemCount[2]=8;
	lstrcpyn(m_szKeyboradChar[2][0],TEXT("stuvwxyz"),CountArray(m_szKeyboradChar[2][0]));
	lstrcpyn(m_szKeyboradChar[2][1],TEXT("STUVWXYZ"),CountArray(m_szKeyboradChar[2][1]));

	//变量定义
	HINSTANCE hResInstance=GetModuleHandle(SHARE_CONTROL_DLL_NAME);

	//加载资源
	CImage ImageItem1;
	CImage ImageItem2;

	ImageItem1.LoadFromResource(hResInstance,IDB_KEYBOARD2_ITEM1);
	ImageItem2.LoadFromResource(hResInstance,IDB_KEYBOARD2_ITEM2);
	

	//设置大小
	m_SizeImageItem1.SetSize(ImageItem1.GetWidth()/3,ImageItem1.GetHeight());
	m_SizeImageItem2.SetSize(ImageItem2.GetWidth()/3,ImageItem2.GetHeight());
	
	m_SizeBackGround.SetSize(322,70);

	return;
}

//析构函数
CPasswordKeyboard2::~CPasswordKeyboard2()
{
}


//设置响应
VOID CPasswordKeyboard2::SetKeyBoardSink(IKeyBoardSink* pSink)
{
	m_pKeyBoardSink=pSink;
}

//消息解释
BOOL CPasswordKeyboard2::PreTranslateMessage(MSG * pMsg)
{
	//大写锁键
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_CAPITAL))
	{
		//设置变量
		bool bCapsLockStatus=m_bCapsLockStatus;
		m_bCapsLockStatus=(GetKeyState(VK_CAPITAL)&0x0F)>0;

		//更新界面
		if (bCapsLockStatus!=m_bCapsLockStatus)
		{
			RedrawWindow(NULL,NULL,RDW_FRAME|RDW_ERASE|RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASENOW);
		}

		return TRUE;
	}


	return __super::PreTranslateMessage(pMsg);
}

//创建函数
BOOL CPasswordKeyboard2::OnInitDialog()
{
	__super::OnInitDialog();

	//设置变量
	m_bMouseDown=false;
	m_wHoverRow=INVALID_WORD;
	m_wHoverLine=INVALID_WORD;

	//获取状态
	m_bCapsLockStatus=(GetKeyState(VK_CAPITAL)&0x0F)>0;

	//构造位置
	CRect rcWindow;
	rcWindow.SetRect(0,0,m_SizeBackGround.cx,m_SizeBackGround.cy);

	//移动窗口
	CalcWindowRect(&rcWindow,CWnd::adjustBorder);
	SetWindowPos(NULL,0,0,rcWindow.Width(),rcWindow.Height(),SWP_NOMOVE|SWP_NOZORDER);

	return FALSE;
}

//更新位置
VOID CPasswordKeyboard2::SetCurrentStation(CPoint MousePoint)
{
	//变量定义
	WORD wHoverRow=INVALID_WORD;
	WORD wHoverLine=INVALID_WORD;

	//字符按钮
	if ((MousePoint.x>=ITEM_POS_X)&&(MousePoint.y>=ITEM_POS_Y))
	{
		//列数计算
		if (((MousePoint.x-ITEM_POS_X)%(m_SizeImageItem1.cx+ITEM_POS_S))<=m_SizeImageItem1.cx)
		{
			wHoverRow=(WORD)((MousePoint.x-ITEM_POS_X)/(m_SizeImageItem1.cx+ITEM_POS_S));
		}

		//行数计算
		if (((MousePoint.y-ITEM_POS_Y)%(m_SizeImageItem1.cy+ITEM_POS_S))<=m_SizeImageItem1.cy)
		{
			wHoverLine=(WORD)((MousePoint.y-ITEM_POS_Y)/(m_SizeImageItem1.cy+ITEM_POS_S));
		}

		//参数调整
		if (wHoverLine>=CountArray(m_nItemCount)) wHoverLine=INVALID_WORD;
		if ((wHoverLine==INVALID_WORD)||(wHoverRow>=m_nItemCount[wHoverLine])) wHoverRow=INVALID_WORD;
	}

	//功能按钮
	//if ((wHoverLine<2)&&(wHoverRow==INVALID_WORD))
	{
		INT nYDrawPos=23*2;
		INT nXDrawPos=ITEM_POS_X+(m_SizeImageItem1.cx+ITEM_POS_S)*8;

		//原点位置
		CPoint ButtonPoint[3];
		ButtonPoint[0].SetPoint(nXDrawPos,nYDrawPos);
		nXDrawPos+=((m_SizeImageItem1.cx+ITEM_POS_S)*2);
		ButtonPoint[1].SetPoint(nXDrawPos,nYDrawPos);
		nXDrawPos+=((m_SizeImageItem1.cx+ITEM_POS_S)*2);
		ButtonPoint[2].SetPoint(nXDrawPos,nYDrawPos);

		//按钮区域
		CRect rcCapital(ButtonPoint[0],m_SizeImageItem2);
		CRect rcBack(ButtonPoint[1],m_SizeImageItem2);
		CRect rcClean(ButtonPoint[2],m_SizeImageItem2);

		//退格按钮
		if (rcBack.PtInRect(MousePoint))
		{
			wHoverRow=ROW_BACK;
			wHoverLine=LINE_FUNCTION;
		}

		//切换按钮
		if (rcClean.PtInRect(MousePoint))
		{
			wHoverRow=ROW_CLEAN;
			wHoverLine=LINE_FUNCTION;
		}

		//大写按钮
		if (rcCapital.PtInRect(MousePoint))
		{
			wHoverRow=ROW_CAPITAL;
			wHoverLine=LINE_FUNCTION;
		}
	}

	//设置变量
	m_wHoverRow=wHoverRow;
	m_wHoverLine=wHoverLine;

	return;
}

//虚拟编码
WORD CPasswordKeyboard2::GetVirualKeyCode(WORD wHoverLine, WORD wHoverRow)
{
	//功能建区
	if (wHoverLine==LINE_FUNCTION)
	{
		switch (wHoverRow)
		{
		case ROW_BACK: { return VK_BACK; }
		case ROW_CLEAN: { return VK_SHIFT; }
		case ROW_CAPITAL: { return VK_CAPITAL; }
		}
	}

	//字符建区
	if ((wHoverLine<CountArray(m_nItemCount))&&(wHoverRow<m_nItemCount[wHoverLine]))
	{
		//计算索引
		bool bLowerChar=true;
		//if (m_bShiftStatus==true) bLowerChar=!bLowerChar;
		if ((m_bCapsLockStatus==true)) bLowerChar=!bLowerChar;

		//获取字符
		INT nItemCount=m_nItemCount[wHoverLine];
		INT nCharRowIndex=(wHoverRow+m_nRandLine[wHoverLine])%nItemCount;
		TCHAR chChar=m_szKeyboradChar[wHoverLine][(bLowerChar==true)?0:1][nCharRowIndex];

		return chChar;
	}

	return 0;
}

//界面绘画
VOID CPasswordKeyboard2::OnNcPaint()
{
	__super::OnNcPaint();

	////获取位置
	//CRect rcWindow;
	//GetWindowRect(&rcWindow);

	////绘画边框
	//CWindowDC WindowDC(this);
	//COLORREF crBoradFrame=CSkinEdit::m_SkinAttribute.m_crEnableBorad;
	//WindowDC.Draw3dRect(0,0,rcWindow.Width(),rcWindow.Height(),crBoradFrame,crBoradFrame);

	return;
}

//绘画背景
BOOL CPasswordKeyboard2::OnEraseBkgnd(CDC * pDC)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//创建缓冲
	CDC BufferDC;
	CBitmap ImageBuffer;
	BufferDC.CreateCompatibleDC(pDC);
	ImageBuffer.CreateCompatibleBitmap(pDC,rcClient.Width(),rcClient.Height());

	//设置环境
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SelectObject(&ImageBuffer);
	//BufferDC.SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

	//变量定义
	HINSTANCE hResInstance=GetModuleHandle(SHARE_CONTROL_DLL_NAME);
	CSkinRenderManager * pSkinRenderManager=CSkinRenderManager::GetInstance();

	//加载资源
	CImage ImageItem1;
	CImage ImageItem2;
	CImage ImageButton;
	
	ImageItem1.LoadFromResource(hResInstance,IDB_KEYBOARD2_ITEM1);
	ImageItem2.LoadFromResource(hResInstance,IDB_KEYBOARD2_ITEM2);

	//渲染资源
	pSkinRenderManager->RenderImage(ImageItem1);
	pSkinRenderManager->RenderImage(ImageItem2);

	BufferDC.FillSolidRect(&rcClient,RGB(255,255,255));

	//创建字体
	CFont DrawFont;
	DrawFont.CreateFont(14,0,0,0,600,0,0,0,134,3,2,1,2,TEXT("宋体"));
	BufferDC.SelectObject(DrawFont);

	//字符按钮
	for (INT nLine=0;nLine<CountArray(m_nItemCount);nLine++)
	{
		//绘画子项
		for (INT nRow=0;nRow<m_nItemCount[nLine];nRow++)
		{
			//计算位置
			INT nXImageIndex=0;
			INT nCharItemIndex=(nRow+m_nRandLine[nLine])%m_nItemCount[nLine];
			if ((m_wHoverLine==nLine)&&(m_wHoverRow==nRow)) nXImageIndex=(m_bMouseDown==false)?1:2;

			//绘画子项
			INT nXDrawPos=ITEM_POS_X+(m_SizeImageItem1.cx+ITEM_POS_S)*nRow;
			INT nYDrawPos=ITEM_POS_Y+(m_SizeImageItem1.cy+ITEM_POS_S)*nLine;
			ImageItem1.BitBlt(BufferDC,nXDrawPos,nYDrawPos,m_SizeImageItem1.cx,m_SizeImageItem1.cy,nXImageIndex*m_SizeImageItem1.cx,0);
			
			if(m_bCapsLockStatus!=true)
			{
				//绘画字符
				BufferDC.SetTextColor(RGB(0,0,0));
				BufferDC.TextOut(nXDrawPos+4,nYDrawPos+4,&m_szKeyboradChar[nLine][0][nCharItemIndex],1);
			}
			else
			{
				//绘画字符
				BufferDC.SetTextColor(RGB(0,0,0));
				BufferDC.TextOut(nXDrawPos+4,nYDrawPos+4,&m_szKeyboradChar[nLine][1][nCharItemIndex],1);
			}



			////绘画字符
			//BufferDC.SetTextColor(RGB(0,0,0));
			//BufferDC.TextOut(nXDrawPos+15,nYDrawPos+3,&m_szKeyboradChar[nLine][1][nCharItemIndex],1);
		}
	}

	//大写按钮
	{
		//资源位置
		INT nXImageIndex=0;//(m_bCapsLockStatus==true)?1:0;
		if ((m_wHoverLine==LINE_FUNCTION)&&(m_wHoverRow==ROW_CAPITAL)) nXImageIndex=(m_bMouseDown==false)?1:2;

		//绘画背景
		INT nYDrawPos=23*2;//+ITEM_POS_Y;
		INT nXDrawPos=ITEM_POS_X+(m_SizeImageItem1.cx+ITEM_POS_S)*8;
		ImageItem2.BitBlt(BufferDC,nXDrawPos,nYDrawPos,m_SizeImageItem2.cx,m_SizeImageItem2.cy,nXImageIndex*m_SizeImageItem2.cx,0);

		//大写按钮
		BufferDC.SetTextColor(RGB(0,0,0));
		BufferDC.TextOut(nXDrawPos+7,nYDrawPos+3,TEXT("大写"),2);
	}


	//退格按钮
	{
		//资源位置
		INT nXImageIndex=0;
		if ((m_wHoverLine==LINE_FUNCTION)&&(m_wHoverRow==ROW_BACK)) nXImageIndex=(m_bMouseDown==false)?1:2;

		//绘画背景
		INT nYDrawPos=23*2;//+ITEM_POS_Y;
		INT nXDrawPos=ITEM_POS_X+(m_SizeImageItem1.cx+ITEM_POS_S)*10;//m_nItemCount[0];
		ImageItem2.BitBlt(BufferDC,nXDrawPos,nYDrawPos,m_SizeImageItem2.cx,m_SizeImageItem2.cy,nXImageIndex*m_SizeImageItem2.cx,0);

		//绘画字符
		BufferDC.SetTextColor(RGB(0,0,0));
		BufferDC.TextOut(nXDrawPos+7,nYDrawPos+3,TEXT("退格"),2);
	}

	//清空按钮
	{
		//资源位置
		INT nXImageIndex=(m_bCapsLockStatus==true)?1:0;
		if ((m_wHoverLine==LINE_FUNCTION)&&(m_wHoverRow==ROW_CLEAN)) nXImageIndex=(m_bMouseDown==false)?1:2;

		//绘画背景
		INT nYDrawPos=23*2;//+ITEM_POS_Y;
		INT nXDrawPos=ITEM_POS_X+(m_SizeImageItem1.cx+ITEM_POS_S)*12;
		ImageItem2.BitBlt(BufferDC,nXDrawPos,nYDrawPos,m_SizeImageItem2.cx,m_SizeImageItem2.cy,nXImageIndex*m_SizeImageItem2.cx,0);

		//大写按钮
		BufferDC.SetTextColor(RGB(0,0,0));
		BufferDC.TextOut(nXDrawPos+7,nYDrawPos+3,TEXT("清空"),2);
	}

	//绘画界面
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),&BufferDC,0,0,SRCCOPY);

	//清理资源
	BufferDC.DeleteDC();
	ImageBuffer.DeleteObject();

	return TRUE;
}

//失去焦点
VOID CPasswordKeyboard2::OnKillFocus(CWnd * pNewWnd)
{
	__super::OnKillFocus(pNewWnd);

	//销毁窗口
	//DestroyWindow();

	return;
}

//鼠标消息
VOID CPasswordKeyboard2::OnLButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnLButtonUp(nFlags,Point);

	//取消捕获
	if (m_bMouseDown==true)
	{
		//取消捕获
		ReleaseCapture();

		//设置变量
		m_bMouseDown=false;

		//获取光标
		CPoint MousePoint;
		GetCursorPos(&MousePoint);
		ScreenToClient(&MousePoint);

		//更新位置
		WORD wHoverRow=m_wHoverRow;
		WORD wHoverLine=m_wHoverLine;
		SetCurrentStation(MousePoint);

		//点击处理
		if ((m_wHoverRow==wHoverRow)&&(m_wHoverLine==wHoverLine))
		{
			////关闭按钮
			//if ((m_wHoverLine==LINE_FUNCTION)&&(m_wHoverRow==ROW_CLOSE_KEY))
			//{
			//	//设置焦点
			//	CONTAINING_RECORD(this,CPasswordControl,m_PasswordKeyboard)->m_edPassword.SetFocus();

			//	//销毁窗口
			//	DestroyWindow();

			//	return;
			//}

			//虚拟编码
			WORD wViraulCode=GetVirualKeyCode(m_wHoverLine,m_wHoverRow);

			//按钮处理
			switch (wViraulCode)
			{
			case VK_SHIFT:		//切换按钮
				{
					if(m_pKeyBoardSink!=NULL) m_pKeyBoardSink->OnKeyBoardClean();

					break;
				}
			case VK_CAPITAL:	//大写按钮
				{
					//变量定义
					INPUT Input[2];
					ZeroMemory(Input,sizeof(Input));

					//设置变量
					Input[1].ki.dwFlags=KEYEVENTF_KEYUP;   
					Input[0].type=Input[1].type=INPUT_KEYBOARD;
					Input[0].ki.wVk=Input[1].ki.wVk=wViraulCode;

					//模拟输入
					SendInput(CountArray(Input),Input,sizeof(INPUT));

					m_bCapsLockStatus=!m_bCapsLockStatus;

					//m_bCapsLockStatus=!m_bCapsLockStatus;
					//m_bCapsLockStatus=(GetKeyState(VK_CAPITAL)&0x0F)>0;

					break;
				}
			default:			//默认按钮
				{
					//发送消息
					//CDialog * pPasswordControl=CONTAINING_RECORD(this,CDialog,m_PasswordKeyboard2);
					//CWnd* pPasswordControl=GetParent();
					//if (pPasswordControl!=NULL) pPasswordControl->SendMessage(WM_CHAR,wViraulCode,0L);

					if(m_pKeyBoardSink!=NULL) m_pKeyBoardSink->OnKeyBoardChar(wViraulCode);

					break;
				}
			}
		}

		//更新界面
		RedrawWindow(NULL,NULL,RDW_FRAME|RDW_ERASE|RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return;
}

//鼠标消息
VOID CPasswordKeyboard2::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//点击按钮
	if ((m_wHoverLine!=INVALID_WORD)&&(m_wHoverRow!=INVALID_WORD))
	{
		//捕获鼠标
		SetCapture();

		//设置变量
		m_bMouseDown=true;

		//更新界面
		RedrawWindow(NULL,NULL,RDW_FRAME|RDW_ERASE|RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return;
}

//光标消息
BOOL CPasswordKeyboard2::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//获取光标
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//更新位置
	WORD wHoverRow=m_wHoverRow;
	WORD wHoverLine=m_wHoverLine;
	SetCurrentStation(MousePoint);

	//更新界面
	if ((m_wHoverRow!=wHoverRow)||(m_wHoverLine!=wHoverLine))
	{
		RedrawWindow(NULL,NULL,RDW_FRAME|RDW_ERASE|RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	//设置光标
	if ((m_wHoverRow!=INVALID_WORD)&&(m_wHoverLine!=INVALID_WORD))
	{
		SetCursor(LoadCursor(GetModuleHandle(SHARE_CONTROL_DLL_NAME),MAKEINTRESOURCE(IDC_HAND_CUR)));
		return true;
	}

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//////////////////////////////////////////////////////////////////////////////////
