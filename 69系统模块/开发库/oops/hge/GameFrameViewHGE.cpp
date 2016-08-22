#include "StdAfx.h"
#include "GameFrameViewHGE.h"


//////////////////////////////////////////////////////////////////////////
// 标志大小
#define FLAG_CX	21
#define FLAG_CY 18

// 滚动文本
#define IDI_ROLL_TEXT				30		
// 定时器标示
#define IDI_HGE						501								//HGE定时器

// 定时器时间
#define IDT_HGE_TIME				20								//HGE定时器时间

//////////////////////////////////////////////////////////////////////////

CGameFrameViewHGE* g_GameFrameView = NULL;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameFrameViewHGE, CGameFrameView)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameFrameViewHGE::CGameFrameViewHGE()
: m_pHge(NULL)
, m_dwClearColor(0xFFFE00FF)
, m_spt(0, 0, 0, 0, 0)
{
	g_GameFrameView = this;
}

//析构函数
CGameFrameViewHGE::~CGameFrameViewHGE()
{
}


//重置界面
void CGameFrameViewHGE::ResetGameView()
{
	return;
}

//调整控件
void CGameFrameViewHGE::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//界面更新
void CGameFrameViewHGE::InvalidGameView(INT nXPos, INT nYPos, INT nWidth, INT nHeight)
{
	//构造位置
	CRect rcInvalid;
	rcInvalid.SetRect(nXPos,nYPos,nXPos+nWidth,nYPos+nHeight);

	//位置调整
	if (rcInvalid.IsRectNull()==TRUE) GetClientRect(&rcInvalid);

	//设置矩形
	if (m_RectDirtySurface.IsRectEmpty()==FALSE)
	{
		//设置变量
		m_RectDirtySurface.top    = __min(m_RectDirtySurface.top,rcInvalid.top);
		m_RectDirtySurface.left   = __min(m_RectDirtySurface.left,rcInvalid.left);
		m_RectDirtySurface.right  = __max(m_RectDirtySurface.right,rcInvalid.right);
		m_RectDirtySurface.bottom = __max(m_RectDirtySurface.bottom,rcInvalid.bottom);
	}
	else
	{
		//设置变量
		m_RectDirtySurface=rcInvalid;
	}

	//刷新窗口
	InvalidateRect(&rcInvalid,FALSE);

	return;
}

//桌面消息
void CGameFrameViewHGE::AddGameTableMessage(LPCTSTR pszMessage,WORD wLen,WORD wType)
{
	if(wType&SMT_TABLE_ROLL)
	{
		m_strRollText=CString(pszMessage,wLen);
		if(m_strRollText.Find(TEXT("系统配桌"))>=0)m_bShowWaitDistribute=TRUE;
		if(m_vecText.empty())
		{
			m_ptRoll.SetPoint(295,475);
			m_wRollTextCount=300;
			SetTimer(IDI_ROLL_TEXT,20,NULL);
		}
		m_vecText.push_back(m_strRollText);
	}
// 	else if(wType&SMT_TABLE_FIX)
// 	{
// 		m_strFixText="";
// 		m_strFixText=CString(pszMessage,wLen);
// 		UpdateGameView(&CRect(m_wTextLeft,400,m_wTextLeft+500,600));
// 	}

	//if (m_bShowWaitDistribute)
	{
		
	}
}

void CGameFrameViewHGE::SetBackgroundColor(DWORD argb)
{
	m_dwClearColor = argb;
}

//绘画准备
void CGameFrameViewHGE::DrawUserReady(int x, int y, int align)
{
	if (m_pHge == 0)
	{
		return;
	}

	if (m_texReady == 0)
	{
		return;
	}


	// 计算位置
	int w = m_pHge->Texture_GetWidth(m_texReady, true);
	int h = m_pHge->Texture_GetHeight(m_texReady, true);
	AlignResetPosition(x, y, w, h, align);

	m_spt.SetTexture(m_texReady);
	m_spt.SetTextureRect(0, 0, (float)w, (float)h);
	m_spt.Render((float)x, (float)y);
} 

//绘画时间
void CGameFrameViewHGE::DrawUserClock(int x, int y, WORD wUserClock, int align)
{
	if (m_pHge == 0)
	{
		return;
	}

	if (m_texClockBack == 0 || m_texClockItem == 0)
	{
		return;
	}

	if (wUserClock <= 0 || wUserClock >= 100)
	{
		return;
	}

	// 计算位置
	int w = m_pHge->Texture_GetWidth(m_texClockBack, true);
	int h = m_pHge->Texture_GetHeight(m_texClockBack, true);
	AlignResetPosition(x, y, w, h, align);

	// 背景
	m_spt.SetTexture(m_texClockBack);
	m_spt.SetTextureRect(0, 0, (float)w, (float)h);
	m_spt.Render((float)x, (float)y);

	x += 13;
	y += 27;
	//绘画时间
	WORD nClockItem1 = wUserClock / 10;
	WORD nClockItem2 = wUserClock % 10;
	w = m_pHge->Texture_GetWidth(m_texClockItem, true) / 10;
	h = m_pHge->Texture_GetHeight(m_texClockItem, true);
	m_spt.SetTexture(m_texClockItem);

	m_spt.SetTextureRect((float)w * nClockItem1, 0, (float)w, (float)h);
	m_spt.Render((float)x , (float)y);

	m_spt.SetTextureRect((float)w * nClockItem2, 0, (float)w, (float)h);
	m_spt.Render((float)x + w, (float)y);
}

//绘画标志
void CGameFrameViewHGE::DrawOrderFlag(int x, int y, BYTE cbImageIndex, int align)
{
	if (m_pHge == 0)
	{
		return;
	}

	if (m_texMemberFlag == 0)
	{
		return;
	}


	if (cbImageIndex >= m_iMemberTotals)
	{
		cbImageIndex = 0;
	}

	//绘画头像
	int nXImagePos = cbImageIndex % m_iMemberTotals * FLAG_CX;
	int nYImagePos = (cbImageIndex / m_iMemberTotals) * FLAG_CY;

	AlignResetPosition(x, y, FLAG_CX, FLAG_CY, align);
	m_spt.SetTexture(m_texMemberFlag);
	m_spt.SetTextureRect((float)nXImagePos, (float)nYImagePos, FLAG_CX, FLAG_CY);
	m_spt.Render((float)x, (float)y);
}
//绘画头像
void CGameFrameViewHGE::DrawUserAvatar(int x, int y,IClientUserItem * pIClientUserItem, int align,float fzoom)
{
	//效验参数
	ASSERT(pIClientUserItem!=NULL);
	if (pIClientUserItem==NULL) return;

	//变量定义
	tagUserInfo*       pUserInfo       = pIClientUserItem->GetUserInfo();
	tagCustomFaceInfo* pCustomFaceInfo = pIClientUserItem->GetCustomFaceInfo();

	bool bOnLine = (pIClientUserItem->GetUserStatus() != US_OFFLINE);

	//绘画头像
	if ((pUserInfo->dwCustomID!=0)&&(pCustomFaceInfo->dwDataSize!=0))
	{// 自定义头像
		DrawAvatar(pCustomFaceInfo->dwCustomFace, x, y, bOnLine, align,fzoom);
	}
	else
	{
		DrawAvatar(pUserInfo->wFaceID, x, y,bOnLine, align,fzoom);
	}
}
void CGameFrameViewHGE::DrawAvatar(unsigned int faceId, int x, int y, bool isOnLine, unsigned int align,float fzoom)
{
	if (m_pHge == 0)
	{
		return;
	}

	if (m_hTextureAvatar == 0)
	{
		return;
	}


	if (faceId >= m_iAvatarTotals)
	{
		faceId = 0;
	}

	//绘画头像
	int nXImagePos = faceId % m_iAvatarCols * FACE_CX;
	int nYImagePos = (faceId / m_iAvatarCols) * 2 * FACE_CY + (isOnLine ? 0 : FACE_CY);

	AlignResetPosition(x, y, FACE_CX, FACE_CY, align);
	m_spt.SetTexture(m_hTextureAvatar);
	m_spt.SetTextureRect((float)nXImagePos, (float)nYImagePos, FACE_CX, FACE_CY);
	m_spt.RenderEx((float)x, (float)y,0.0f,fzoom);
} // DrawAvatar

void CGameFrameViewHGE::DrawAvatar(const DWORD (&dwCustomFace)[FACE_CX*FACE_CY], int x, int y, bool isOnLine, unsigned int align,float fzoom)
{
	if (m_pHge == 0)
	{
		return;
	}

	if (m_hCustomAvatar == 0)
	{
		return;
	}

	//绘画头像
	if (isOnLine)
	{
		DWORD* pData = m_pHge->Texture_Lock(m_hCustomAvatar, false);

		if (pData)
		{
			for (int i=0, l = FACE_CY * FACE_CX; i<l; ++i)
			{
				BYTE cbColorA    = ((BYTE)((dwCustomFace[i])>>24));
				BYTE cbColorR    = ((BYTE)((dwCustomFace[i])>>16));
				BYTE cbColorG    = ((BYTE)(((WORD)(dwCustomFace[i])) >> 8));
				BYTE cbColorB    = ((BYTE)(dwCustomFace[i]));

				pData[i] = ARGB(cbColorA, cbColorR, cbColorG, cbColorB);
			}

			m_pHge->Texture_Unlock(m_hCustomAvatar);
		}
	}
	else
	{
		DWORD* pData = m_pHge->Texture_Lock(m_hCustomAvatar, false);

		if (pData)
		{
			for (int i=0, l = FACE_CY * FACE_CX; i<l; ++i)
			{
				BYTE cbColorA    = ((BYTE)((dwCustomFace[i])>>24));
				BYTE cbColorR    = ((BYTE)((dwCustomFace[i])>>16));
				BYTE cbColorG    = ((BYTE)(((WORD)(dwCustomFace[i])) >> 8));
				BYTE cbColorB    = ((BYTE)(dwCustomFace[i]));
				BYTE cbColorGray = (BYTE)(cbColorR*0.30+cbColorG*0.59+cbColorB*0.11);

				pData[i] = ARGB(cbColorA, cbColorGray, cbColorGray, cbColorGray);
			}

			m_pHge->Texture_Unlock(m_hCustomAvatar);
		}
	}

	AlignResetPosition(x, y, FACE_CX, FACE_CY, align);
	m_spt.SetTexture(m_hCustomAvatar);
	m_spt.SetTextureRect(0, 0, FACE_CX, FACE_CY);
	m_spt.RenderEx((float)x, (float)y,0.0f,fzoom);
} // DrawAvatar

int CGameFrameViewHGE::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	if (!InitHge(GetSafeHwnd()))
	{
		return -1;
	}
	return  0;
}

//销毁消息
void CGameFrameViewHGE::OnDestroy()
{
	ShutDownHge();

	__super::OnDestroy();
}

//绘画函数
void CGameFrameViewHGE::OnPaint()
{
	CPaintDC dc(this);
}

BOOL CGameFrameViewHGE::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

//位置变化
void CGameFrameViewHGE::OnSize(UINT nType, int cx, int cy)
{

	//效验状态
	if ((cx==0)||(cy==0)) return;
	if (GetSystemMetrics(SM_CXSCREEN)<cx) return;

	if (m_pHge)
	{
		HWND hWnd = m_pHge->System_GetState(HGE_HWND);
		::MoveWindow(hWnd, 0, 0, cx, cy, false);		
	}

	__super::OnSize(nType, cx, cy);
}

//定时器
void CGameFrameViewHGE::OnTimer(UINT nIDEvent)
{
	__super::OnTimer(nIDEvent);

	switch(nIDEvent)
	{
		case IDI_ROLL_TEXT:
		{	
			m_wRollTextCount-=2;

			if (m_ptRoll.y>175)
			{
				m_ptRoll.y-=3;
			}


			if(m_wRollTextCount==0)
			{
				if(m_vecText.size()<=1)
				{
					m_vecText.clear();
					KillTimer(IDI_ROLL_TEXT);
					m_wRollTextCount=300;
					m_ptRoll.SetPoint(295,475);
					m_strRollText="";
					m_rcText.SetRect(0,0,1,1);
				}
				else
				{
					vector <CString>::iterator Iter;
					Iter=m_vecText.begin();
					m_vecText.erase(Iter);
					m_strRollText=m_vecText.at(0);
					m_wRollTextCount=300;
					m_ptRoll.SetPoint(295,475);
					m_rcText=CRect(m_wTextLeft,m_wRollTextCount+270,m_wTextLeft+500,m_wRollTextCount+370);
				}

			}

		}
	case IDI_HGE:				//HGE刷新
		{
			if(m_pHge && m_pHge->System_GetState(HGE_HWND))
			{
				bool bOk = m_pHge->System_Start();
			}
			return ;
		}

	}
}

// 初始化HGE
bool CGameFrameViewHGE::InitHge(HWND hWnd)
{
	if (NULL != m_pHge || NULL == hWnd) 
		return true;
	srand((unsigned int)time(NULL));
	m_pHge = hgeCreate(HGE_VERSION);

	m_pHge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	m_pHge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	m_pHge->System_SetState(HGE_GFXRESTOREFUNC, RestoreFunc);
	//m_pHge->System_SetState(HGE_SCREENBPP, 32);
	m_pHge->System_SetState(HGE_HWNDPARENT, hWnd);
	m_pHge->System_SetState(HGE_USESOUND, true);
	m_pHge->System_SetState(HGE_HIDEMOUSE, false);
	m_pHge->System_SetState(HGE_SHOWSPLASH, false);
	//m_pHge->System_SetState(HGE_TITLE, TEXT("GameEngine"));
	//m_pHge->System_SetState(HGE_FPS,60);

	m_pHge->System_SetState( HGE_SCREENWIDTH, 1024 );

	m_pHge->System_SetState( HGE_SCREENHEIGHT, 730 );

	m_pHge->System_SetState(HGE_WINDOWED, true); //默认为false
	//m_pHge->System_SetState(HGE_FPS, 100);

	m_MatchFont=new GfxFont("幼圆",16,true);
	m_MatchFont2=new GfxFont("幼圆",18,true);

	m_MatchRoll=new GfxFont("幼圆",22,true);

	if (m_pHge->System_Initiate())
	{
		//m_pBtMgr = new CHgeButtonMgr(this);
		//m_HgeSound.SetParent(this);

		if (!LoadResources())
		{
			MessageBox(TEXT("加载资源失败！"));
			return false;
		}

		if (!OnInit())
		{
			return false;
		}
		
		m_pHge->System_Start();

		SetTimer(IDI_HGE, IDT_HGE_TIME, NULL);
	}
	else 
	{
		//ShowInformationEx(m_pHge->System_GetErrorMessage(),0 ,MB_ICONINFORMATION, ("系统消息")) ;
		return false;
	}

	return true;
}

//关闭引擎
void CGameFrameViewHGE::ShutDownHge()
{
	KillTimer(IDI_HGE);
	OnDeinit();

	m_pHge->Texture_Free(m_hTextureAvatar);
	m_pHge->Texture_Free(m_hCustomAvatar);

	m_pHge->Texture_Free(m_texReady);
	m_pHge->Texture_Free(m_texMemberFlag);
	m_pHge->Texture_Free(m_texClockBack);
	m_pHge->Texture_Free(m_texClockItem);

	if (NULL != m_pHge)
	{
		m_pHge->System_Shutdown();
	}
	m_pHge = NULL;

}

// 装载资源
bool CGameFrameViewHGE::LoadResources()
{
	// 头像
	m_hTextureAvatar = GetDllTexture(m_pHge, TEXT("PNG"), TEXT("USER_FACE_IMAGE"), AVATAR_CONTROL_DLL_NAME);
	m_hCustomAvatar	 = m_pHge->Texture_Create(FACE_CX, FACE_CY);

	m_iAvatarCols	 = 0;
	m_iAvatarRows	 = 0;
	m_iAvatarTotals	 = 0;

	if (m_hTextureAvatar != 0)
	{
		int width		= m_pHge->Texture_GetWidth(m_hTextureAvatar, true);
		int height		= m_pHge->Texture_GetHeight(m_hTextureAvatar, true);
		m_iAvatarCols   = width / FACE_CX;
		m_iAvatarRows   = height / (FACE_CY * 2); // 黑白两组
		m_iAvatarTotals = m_iAvatarCols * m_iAvatarRows;
	}

	m_MatchView=GetDllTexture(m_pHge, TEXT("PNG"), TEXT("MATCH_INFO"), GAME_FRAME_DLL_NAME);
	m_GreenNumber=GetDllTexture(m_pHge, TEXT("PNG"), TEXT("NUMBER_GREEN"), GAME_FRAME_DLL_NAME);	
	m_OrangeNumber=GetDllTexture(m_pHge, TEXT("PNG"), TEXT("NUMBER_ORANGE"), GAME_FRAME_DLL_NAME);
	m_WaitTip=GetDllTexture(m_pHge, TEXT("PNG"), TEXT("MATCH_WAIT_TIP"), GAME_FRAME_DLL_NAME);
	m_Line=GetDllTexture(m_pHge, TEXT("PNG"), TEXT("MATCH_LINE"), GAME_FRAME_DLL_NAME);

	// 玩家标志 
	m_texMemberFlag	= GetDllTexture(m_pHge, TEXT("PNG"), TEXT("MEMBER_FLAG"), GAME_FRAME_DLL_NAME);
	m_iMemberCols	= 0;
	m_iMemberRows	= 0;
	m_iMemberTotals	= 0;

	if (m_texMemberFlag != 0)
	{
		int width		= m_pHge->Texture_GetWidth(m_texMemberFlag, true);
		int height		= m_pHge->Texture_GetHeight(m_texMemberFlag, true);
		m_iMemberCols   = width / FLAG_CX;
		m_iMemberRows   = height / FLAG_CY;
		m_iMemberTotals = m_iMemberCols * m_iMemberRows;
	}

	// 其他标志
	m_texReady      = GetDllTexture(m_pHge, TEXT("PNG"), TEXT("USER_READY"), GAME_FRAME_DLL_NAME);
	m_texClockBack  = GetDllTexture(m_pHge, TEXT("PNG"), TEXT("USER_CLOCK_BACK"), GAME_FRAME_DLL_NAME);
	m_texClockItem  = GetDllTexture(m_pHge, TEXT("PNG"), TEXT("USER_CLOCK_ITEM"), GAME_FRAME_DLL_NAME);


	return true;
}

// 静态绘制接口
bool CGameFrameViewHGE::FrameFunc()
{
	if (NULL != g_GameFrameView)
	{		
		g_GameFrameView->OnUpdate();
	}

	return false;
}

// 静态绘制接口
bool CGameFrameViewHGE::RenderFunc()
{
	if (NULL != g_GameFrameView)
	{	
		bool bRe = false;
		if (NULL != g_GameFrameView->m_pHge)
		{
			g_GameFrameView->m_pHge->Gfx_BeginScene();
			g_GameFrameView->m_pHge->Gfx_Clear(g_GameFrameView->m_dwClearColor);

			g_GameFrameView->OnDrawView();
			g_GameFrameView->DrawMatchInfo();
			g_GameFrameView->m_pHge->Gfx_EndScene();
		}

		return bRe;
	}

	return false;
}

bool CGameFrameViewHGE::RestoreFunc()
{
	if (NULL != g_GameFrameView)
	{
		g_GameFrameView->OnRestore();
	}

	return true;
}

HTEXTURE CGameFrameViewHGE::GetDllTexture(HGE* hge, LPCTSTR type, LPCTSTR name, LPCTSTR dll)
{
	try
	{
		HMODULE hModle  = GetModuleHandle(dll);
		HRSRC hRsrc     = FindResource(hModle, name, type);
		DWORD dwSize    = SizeofResource(hModle, hRsrc); 
		HGLOBAL hGlobal = LoadResource(hModle, hRsrc);
		LPVOID pBuffer  = LockResource(hGlobal);
		HTEXTURE tex    = hge->Texture_Load((const char*)pBuffer,dwSize);
		UnlockResource(hGlobal);
		return tex;
	}
	catch (...)
	{

	}

	return 0;
} // GetDllTexture

//通过自定义位图数据创建HGE文理 
HTEXTURE CGameFrameViewHGE::GetCustomTexture(DWORD* pChustomData, HGE* hge, int cx, int cy)
{
	HTEXTURE hTex = hge->Texture_Create(cx, cy);
	DWORD *pData  = hge->Texture_Lock(hTex,false);

	for ( int i=0; i<cy; i++ )
	{
		for ( int j=0; j<cx; j++ )
		{
			pData[i*cx+j] = pChustomData[i*cy+j];
		}
	}

	hge->Texture_Unlock(hTex);
	return hTex;
} // GetCustomUserFace

//比赛信息
VOID CGameFrameViewHGE::DrawMatchInfo()
{
	//窗口尺寸
	CRect rcClient;
	GetClientRect(&rcClient);

	//等待提示
	if(m_bShowWaitDistribute)
	{
		m_spt.SetTexture(m_WaitView);
		// 计算位置
		int w = m_pHge->Texture_GetWidth(m_WaitView, true);
		int h = m_pHge->Texture_GetHeight(m_WaitView, true);
		m_spt.SetTextureRect(0,0,w,h);
		m_spt.Render((rcClient.Width()-w)/2,(rcClient.Height()-h)/2);
	}
	//比赛信息
	//if(m_pMatchInfo!=NULL)
	//{
	//	m_spt.SetTexture(m_MatchView);
	//	// 计算位置
	//	int w = m_pHge->Texture_GetWidth(m_MatchView, true);
	//	int h = m_pHge->Texture_GetHeight(m_MatchView, true);
	//	m_spt.SetTextureRect(0,0,w,h);
	//	m_spt.Render(0,0);

	//	CString strNum;
	//	strNum.Format(TEXT("%d"),m_pMatchInfo->wGameCount);
	//	m_MatchFont->SetColor(ARGB(255,255,255,255));
	//	m_MatchFont->Print(12,40,strNum.GetBuffer());

	//	m_MatchFont->SetColor(ARGB(196,221,239));
	//	m_MatchFont->Print(38,10,m_pMatchInfo->szTitle[0]);

	//	m_MatchFont->SetColor(ARGB(196,221,239));
	//	m_MatchFont->Print(38,30,m_pMatchInfo->szTitle[1]);

	//	m_MatchFont->SetColor(ARGB(196,221,239));
	//	m_MatchFont->Print(38,50,m_pMatchInfo->szTitle[2]);

	//	m_MatchFont->SetColor(ARGB(255,221,35));
	//	m_MatchFont->Print(38,70,m_pMatchInfo->szTitle[3]);

	//}

	

	
}
void CGameFrameViewHGE::DrawNumberString(int nXPosition, int nYPosition,HTEXTURE btxNumber,int lNumber)
{
	int nCount=10;

	CSize SizeScoreNumber(m_pHge->Texture_GetWidth(btxNumber,true)/nCount,m_pHge->Texture_GetHeight(btxNumber,true));

	m_spt.SetTexture(btxNumber);

	//计算数目
	LONG lNumberCount=0;
	lNumber=lNumber>=0?lNumber:-lNumber;
	LONGLONG lNumberTemp=lNumber;
	do
	{
		lNumberCount++;
		lNumberTemp/=10;
	}
	while (lNumberTemp>0);

	lNumberCount+=(nCount==10?0:1);
	//位置定义
	INT nXDrawPos=0;

	nXDrawPos=nXPosition+lNumberCount*SizeScoreNumber.cx-SizeScoreNumber.cx;
	

	INT nYDrawPos=nYPosition-SizeScoreNumber.cy/2;

	for (LONG lIndex=0;lIndex<lNumberCount;lIndex++)
	{
		if(nCount == 11 && (lIndex+1) == lNumberCount)
		{
			m_spt.SetTextureRect(10*SizeScoreNumber.cx,0,SizeScoreNumber.cx,SizeScoreNumber.cy);

			m_spt.Render(nXDrawPos,nYDrawPos);

			nXDrawPos-=SizeScoreNumber.cx;
		}
		else
		{
			//绘画号码
			LONG lCellNumber=(LONG)(lNumber%10);

			m_spt.SetTextureRect(lCellNumber*SizeScoreNumber.cx,0,SizeScoreNumber.cx,SizeScoreNumber.cy);

			m_spt.Render(nXDrawPos,nYDrawPos);

			//设置变量
			lNumber/=10;

			nXDrawPos-=SizeScoreNumber.cx;
		}
	}
}
//////////////////////////////////////////////////////////////////////////