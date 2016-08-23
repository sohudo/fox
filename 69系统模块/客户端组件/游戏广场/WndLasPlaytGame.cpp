#include "Stdafx.h"
#include "GamePlaza.h"
#include "PlatformFrame.h"
#include "WndLasPlaytGame.h"

//////////////////////////////////////////////////////////////////////////////////

#define SHOW_LAST_COUNT				3

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CWndLastGamePlay, CWnd)

	//系统消息
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()

	//自定消息
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////

//构造函数
CWndLastGamePlay::CWndLastGamePlay()
{
	//状态标志
	m_bClickFace=false;
	m_bMouseEvent=false;

	//盘旋标志
	m_bHoverFace=false;
	//设置区域
	m_rcFaceArea.SetRect(0,0,0,0);

	return;
}

//析构函数
CWndLastGamePlay::~CWndLastGamePlay()
{
}

//建立消息
INT CWndLastGamePlay::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;


	m_ImgType.LoadFromResource(AfxGetInstanceHandle(),IDB_SERVER_TYPE);

	m_ImgArrow.LoadImage(AfxGetInstanceHandle(),TEXT("TYPE_ARROW"));

	m_rcFaceArea.SetRect(0,0,m_ImgType.GetWidth()/3,m_ImgType.GetHeight());

	//创建字体
	m_FontBold.CreateFont(-12,0,0,0,600,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("宋体"));

	//移动窗口
	CRect rcUserInfo;
	GetWindowRect(&rcUserInfo);
	SetWindowPos(NULL,0,0,rcUserInfo.Width(),112,SWP_NOMOVE|SWP_NOZORDER);


	return 0;
}

//资源目录
VOID CWndLastGamePlay::GetGameResDirectory(TCHAR szProcessName[], TCHAR szResDirectory[], WORD wBufferCount)
{
	//变量定义
	WORD wStringIndex=0;

	//构造目录
	ZeroMemory(szResDirectory,wBufferCount*sizeof(TCHAR));
	lstrcpyn(szResDirectory,szProcessName,wBufferCount);
	while ((szResDirectory[wStringIndex]!=0)&&(szResDirectory[wStringIndex]!=TEXT('.'))) wStringIndex++;

	//字符终止
	szResDirectory[wStringIndex]=0;

	return;
}

//绘画背景
BOOL CWndLastGamePlay::OnEraseBkgnd(CDC * pDC)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//建立缓冲
	CImage ImageBuffer;
	if(ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32) == FALSE) return TRUE;

	//创建 DC
	CImageDC BufferDC(ImageBuffer);
	CDC * pBufferDC=CDC::FromHandle(BufferDC);

	pBufferDC->FillSolidRect(&rcClient,RGB(234,242,247));

	INT nTypeWidth=m_ImgType.GetWidth()/3,nTypeHeight=m_ImgType.GetHeight();
	INT nArrowWidth=m_ImgArrow.GetWidth()/2,nArrowHeight=m_ImgArrow.GetHeight();

	INT nIndex=0;
	if(m_bHoverFace) nIndex=2;
	if(m_bClickFace) nIndex=1;

	m_ImgType.BitBlt(pBufferDC->GetSafeHdc(),0,0,nTypeWidth,nTypeHeight,nIndex*nTypeWidth,0);
	m_ImgArrow.DrawImage(pBufferDC,6,6,nArrowWidth,nArrowHeight,nArrowWidth,0);

	//设置环境
	pBufferDC->SetBkMode(TRANSPARENT);
	pBufferDC->SelectObject(m_FontBold);
	pBufferDC->SetTextColor(RGB(13,61,88));
	//pBufferDC->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());
	pBufferDC->TextOut(21,5,TEXT("最近游戏"));

	//绘制最近游戏
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	ZeroMemory(m_LastGame,sizeof(m_LastGame));
	int iLastGameCount=0;
	int iSpace=13;
	POSITION pos=m_LastPlayGameList.GetHeadPosition();
	while(pos!=NULL)
	{
		WORD wServerID=m_LastPlayGameList.GetNext(pos);
		CGameServerItem* pServerItem=CPlatformFrame::GetInstance()->SearchGameServer(wServerID);
		if(pServerItem!=NULL)
		{
			TCHAR szResDirectory[LEN_KIND]=TEXT("");
			GetGameResDirectory(pServerItem->m_pGameKindItem->m_GameKind.szProcessName,szResDirectory,CountArray(szResDirectory));

			TCHAR szIconPath[MAX_PATH]=TEXT("");
			//_sntprintf(szIconPath,CountArray(szIconPath),TEXT("%s\\GameKind\\Icon_%s.png"),szDirectory,szResDirectory);
			_sntprintf(szIconPath,CountArray(szIconPath),TEXT("%s\\GameKind\\GK_%d.png"),szDirectory,pServerItem->m_GameServer.wKindID);

			CPngImage ImageIcon;
			if(ImageIcon.LoadImage(szIconPath)!=true)
			{
				ImageIcon.LoadImage(AfxGetInstanceHandle(),TEXT("ICON_DEFAULT"));
			}

           //修改图片带房间名字
			//INT nXPos=15+iLastGameCount*(ImageIcon.GetWidth()+15);
			//ImageIcon.DrawImage(pBufferDC,nXPos,24);

			//CRect rcText(nXPos,102-8,nXPos+70,132-8);
			//pBufferDC->DrawText(pServerItem->m_GameServer.szServerName,rcText,DT_CENTER|DT_SINGLELINE|DT_WORDBREAK);

			//m_LastGame[iLastGameCount].bActive=true;
			//m_LastGame[iLastGameCount].wServerID=wServerID;
			//m_LastGame[iLastGameCount].rcArea.left=nXPos+4;
			//m_LastGame[iLastGameCount].rcArea.top=22;
			//m_LastGame[iLastGameCount].rcArea.right=nXPos+70;
			//m_LastGame[iLastGameCount].rcArea.bottom=130;

			INT nXPos=15+iLastGameCount*(ImageIcon.GetWidth()+15);
			ImageIcon.DrawImage(pBufferDC,nXPos,230-198);

			CRect rcText(nXPos,230+70-198,nXPos+70,230+100-198);
			//pBufferDC->DrawText(pServerItem->m_GameServer.szServerName,rcText,DT_CENTER|DT_SINGLELINE|DT_WORDBREAK);

			m_LastGame[iLastGameCount].bActive=true;
			m_LastGame[iLastGameCount].wServerID=wServerID;
			m_LastGame[iLastGameCount].rcArea.left=nXPos+4;
			m_LastGame[iLastGameCount].rcArea.top=130;
			m_LastGame[iLastGameCount].rcArea.right=nXPos+4+66;
			m_LastGame[iLastGameCount].rcArea.bottom=130+86;

			iLastGameCount++;
		}
	}



	//设置环境
	pBufferDC->SetBkMode(TRANSPARENT);
	pBufferDC->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

	//绘画界面
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pBufferDC,0,0,SRCCOPY);

	return TRUE;
}

//加载记录
void CWndLastGamePlay::LoadLastPlayGame()
{
	//工作目录
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//构造路径
	TCHAR szFileName[MAX_PATH]={0};
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\ConstantlyPlayGame.wh"),szDirectory);

	//读取文件
	CFile file;
	if(file.Open(szFileName, CFile::modeRead))
	{
		//读取数据
		char buffer[128]={0};
		UINT uReadCount=file.Read(buffer, CountArray(buffer));
		uReadCount /= 2;

		//加入记录
		WORD *pServerIDArry = (WORD *)buffer;
		for(BYTE i=0; i<uReadCount; i++)
		{
			if(pServerIDArry[i]>0) m_LastPlayGameList.AddHead(pServerIDArry[i]);
		}

		//关闭文件
		file.Close();
	}

	return;
}

//保存记录
void CWndLastGamePlay::SaveLastPlayGame()
{
	//工作目录
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//构造路径
	TCHAR szFileName[MAX_PATH]={0};
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\ConstantlyPlayGame.wh"),szDirectory);

	//写入文件
	CFile file;
	if(file.Open(szFileName, CFile::modeCreate|CFile::modeWrite))
	{
		//设置数据
		POSITION Position=m_LastPlayGameList.GetHeadPosition();
		WORD wServerIDArry[SHOW_LAST_COUNT]={0};
		for(BYTE i=0; i<SHOW_LAST_COUNT; i++)
		{
			if(Position == NULL) break;

			wServerIDArry[i]=m_LastPlayGameList.GetNext(Position);
		}

		//写入数据
		file.Write(wServerIDArry, sizeof(wServerIDArry));

		//关闭文件
		file.Close();
	}

	return;
}

//添加最后游戏
void CWndLastGamePlay::AddLastPlayGame(WORD wServerID)
{
	//查找记录
	POSITION Position=m_LastPlayGameList.GetHeadPosition();
	while(Position != NULL)
	{
		POSITION temppos=Position;
		if(m_LastPlayGameList.GetNext(Position) == wServerID)
		{
			m_LastPlayGameList.RemoveAt(temppos);
			break;
		}
	}

	//记录游戏
	m_LastPlayGameList.AddHead(wServerID);

	//删除子项
	if(m_LastPlayGameList.GetCount() > SHOW_LAST_COUNT)
	{
		//删除记录
		WORD wDeleServerID=m_LastPlayGameList.GetTail();
		m_LastPlayGameList.RemoveTail();
	}

	return;
}


//鼠标消息
VOID CWndLastGamePlay::OnLButtonUp(UINT nFlags, CPoint MousePoint)
{
	__super::OnLButtonUp(nFlags,MousePoint);

	if ((m_bHoverFace==true)&&(m_bClickFace==true))
	{
		//释放捕获
		ReleaseCapture();

		//设置变量
		m_bClickFace=false;

		//个人信息
		if (m_rcFaceArea.PtInRect(MousePoint)==TRUE)
		{
			RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
		}
	}

	return;
}

//鼠标消息
VOID CWndLastGamePlay::OnLButtonDown(UINT nFlags, CPoint MousePoint)
{
	__super::OnLButtonDown(nFlags,MousePoint);

	//设置焦点
	SetFocus();

	//动作处理
	if ((m_bHoverFace==true)&&(m_bClickFace==false))
	{
		//鼠标扑获
		SetCapture();

		//设置变量
		m_bClickFace=true;

		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	for(int i=0; i<3; i++)
	{
		if(m_LastGame[i].bActive!=true) continue;

		if(m_LastGame[i].rcArea.PtInRect(MousePoint))
		{
			CGameServerItem* pServerItem=CPlatformFrame::GetInstance()->SearchGameServer(m_LastGame[i].wServerID);
			CPlatformFrame::GetInstance()->EntranceServerItem(pServerItem);
		}
	}

	return;
}

//光标消息
BOOL CWndLastGamePlay::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//获取光标
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//变量定义
	bool bMouseEvent=false;
	bool bRedrawWindow=false;

	//盘旋判断
	if ((m_bHoverFace==false)&&(m_rcFaceArea.PtInRect(MousePoint)==TRUE))
	{
		//设置变量
		bMouseEvent=true;
		bRedrawWindow=true;

		//设置变量
		m_bHoverFace=true;
	}

	//离开判断
	if ((m_bHoverFace==true)&&(m_rcFaceArea.PtInRect(MousePoint)==FALSE))
	{
		//设置变量
		bRedrawWindow=true;

		//设置变量
		m_bHoverFace=false;
	}


	//注册事件
	if ((m_bMouseEvent==false)&&(bMouseEvent==true))
	{
		//设置变量
		m_bMouseEvent=true;

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

	//更新界面
	if (bRedrawWindow==true)
	{
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	//设置光标
	if (m_bHoverFace==true)
	{
		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HAND_CUR)));
		return true;
	}

	for(int i=0; i<3; i++)
	{
		if(m_LastGame[i].bActive!=true) continue;

		if(m_LastGame[i].rcArea.PtInRect(MousePoint))
		{
			SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HAND_CUR)));
			return true;
		}
	}

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//鼠标消息
LRESULT CWndLastGamePlay::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	//设置变量
	m_bMouseEvent=false;

	//离开处理
	if (m_bHoverFace==true)
	{
		//设置变量
		m_bMouseEvent=false;

		//更新界面
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
