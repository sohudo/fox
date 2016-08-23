#include "Stdafx.h"
#include "Resource.h"
#include "GameUpdate.h"
#include "UpdateFrame.h"
#include "WHService.h"
#include "DlgUpdateCheck.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameUpdateApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameUpdateApp::CGameUpdateApp()
{
	//设置变量
	m_pIDispatch=NULL;
	m_pCustomOccManager=NULL;

	return;
}

//初始函数
BOOL CGameUpdateApp::InitInstance()
{
	__super::InitInstance();

	//常量定义
	LPCTSTR szUpdateClass = TEXT("WHGameUpdate");

#ifndef _DEBUG
	//变量定义
	bool bUpdateExist=false;
	CMutex Mutex(FALSE,szUpdateClass,NULL);
	if (Mutex.Lock(0)==FALSE) bUpdateExist=true;

	//搜索窗口
	CWnd * pWndGameUpdate=CWnd::FindWindow(szUpdateClass,NULL);
	if (pWndGameUpdate!=NULL) 
	{
		//设置变量
		bUpdateExist=true;

		//还原窗口
		if (pWndGameUpdate->IsIconic()) 
		{
			pWndGameUpdate->ShowWindow(SW_RESTORE);
		}

		//激活窗口
		pWndGameUpdate->SetActiveWindow();
		pWndGameUpdate->BringWindowToTop();
		pWndGameUpdate->SetForegroundWindow();
	}

	//结果处理
	if (bUpdateExist==true) return FALSE;
#endif

	//环境配置
	AfxOleInit();
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//初始化Socket
	WORD wVersionRequested;
	WSADATA wsaData;
	INT nErrorID;
	wVersionRequested = MAKEWORD(1,1);
	nErrorID = WSAStartup(wVersionRequested,&wsaData);
	if(nErrorID != 0)
	{
		::AfxMessageBox(TEXT("Socket 初始化失败!"));
		return FALSE;
	}

	//创建对象
	m_pIDispatch=new CImpIDispatch;
	AfxEnableControlContainer(new CCustomOccManager); 

	//变量定义
	GdiplusStartupInput StartupInput;
	GdiplusStartup(&m_lGdiPlusToken,&StartupInput,NULL);

	//变量定义
	WNDCLASS WndClasss;
	ZeroMemory(&WndClasss,sizeof(WndClasss));

	//注册窗口
	WndClasss.style=CS_DBLCLKS;
	WndClasss.hIcon=LoadIcon(IDI_DOWNLOAD);
	WndClasss.lpfnWndProc=DefWindowProc;
	WndClasss.lpszClassName=szUpdateClass;
	WndClasss.hInstance=AfxGetInstanceHandle();
	WndClasss.hCursor=LoadStandardCursor(MAKEINTRESOURCE(IDC_ARROW));
	if (AfxRegisterClass(&WndClasss)==FALSE) AfxThrowResourceException();

	//变量定义
	TCHAR szMouduleFileName[MAX_PATH]=TEXT("");
	TCHAR szTargetName[MAX_PATH]=TEXT("");
	TCHAR szSourceName[MAX_PATH]=TEXT("");
	TCHAR szTargetNameEx[MAX_PATH]=TEXT("");	

	//获取目录
	GetWorkDirectory(szMouduleFileName,CountArray(szMouduleFileName));

	//格式字串
	_sntprintf(szSourceName,CountArray(szSourceName),TEXT("%s/DownLoad/Update.exe"),szMouduleFileName);
	_sntprintf(szTargetName,CountArray(szTargetName),TEXT("%s/Update.exe"),szMouduleFileName);
	_sntprintf(szTargetNameEx,CountArray(szTargetNameEx),TEXT("%s/UpdateOld.exe"),szMouduleFileName);

	//查找文件
	CFileFind FileFind;
	BOOL bResult = FileFind.FindFile(szSourceName);
    if(bResult==TRUE)
	{
		//修改名字
		MoveFile( szTargetName,szTargetNameEx );

		//移动文件
		if(MoveFileWithProgress(szSourceName,szTargetName,NULL,NULL,MOVEFILE_WRITE_THROUGH|MOVEFILE_REPLACE_EXISTING)==TRUE)
		{	
			//重启程序
			ShellExecute(NULL,TEXT("open"),szTargetName,TEXT(""),NULL,true);

			return FALSE;
		}
	}	

	//更新检查
	CDlgUpdateCheck DlgUpdateCheck;
	DlgUpdateCheck.SetWindowDescribe(TEXT(""));
	INT_PTR nResultID = DlgUpdateCheck.DoModal();
	if(nResultID==IDOK) 
	{
		//清理文件
		DeleteFile(szTargetNameEx);

		//构造地址
		TCHAR szMainSetupPath[MAX_PATH]=TEXT("");
		_sntprintf(szMainSetupPath,CountArray(szMainSetupPath),TEXT("%s/GamePlaza.exe"),szMouduleFileName);

		
		//启动宿主程序
		ShellExecute(NULL,TEXT("open"),szMainSetupPath,TEXT("UPDATED"),NULL,true);

		return FALSE;
	}
	
	//数目判断
	if(DlgUpdateCheck.GetUpdateCount()==0)
	{
		return FALSE;
	}

	//清理文件
	DeleteFile(szTargetNameEx);

	//创建窗口
	CGameUpdateFrame * pGameUpdateFrame = new CGameUpdateFrame();
	pGameUpdateFrame->Create(szUpdateClass,szProduct,WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,CRect(0,0,0,0));

	//设置变量
	m_pMainWnd=pGameUpdateFrame;

	return TRUE;
}

//退出函数
INT CGameUpdateApp::ExitInstance()
{
	//关闭插件
	GdiplusShutdown(m_lGdiPlusToken);

	//终止Socket
	WSACleanup();

	//删除对象
	SafeDelete(m_pIDispatch);
	SafeDelete(m_pCustomOccManager);

	return __super::ExitInstance();
}

//////////////////////////////////////////////////////////////////////////////////

//应用程序对象
CGameUpdateApp theApp;

//////////////////////////////////////////////////////////////////////////////////
