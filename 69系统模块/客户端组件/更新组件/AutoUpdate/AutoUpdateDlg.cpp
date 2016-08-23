//AutoUpdateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoUpdate.h"
#include "AutoUpdateDlg.h"
#include "TransparentHelper.h"
#include "..\MD5Checksum\MD5Checksum.h"
#include "..\..\..\全局定义\Platform.h"



#include <io.h> 
#include <fcntl.h> 
#include <stdio.h>

#import <msxml3.dll>
using namespace MSXML2;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define HTTP_PROTOCOL	_T("http://")

// CAutoUpdateDlg dialog


CString m_strXMLFileName = "";
CString m_strGameKindID = "";
CString m_strServerID = "";
CString m_DefaultXMLFile = "AutoUpdate.xml";
int m_iGameKindID = -1;
int m_iServerID = -1;

extern CString		g_strOldName;		//命名后更新器名称

CAutoUpdateDlg::CAutoUpdateDlg(CWnd* pParent): CDialog(IDD_AUTOUPDATE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_ulTotalLength = 0L;
	m_ulCurentLenght = 0L;
	m_pMemDC = NULL;
	m_pOldBitmap = NULL;
	m_pBitmapMem = NULL;
	m_bmBack.LoadBitmap(IDB_DIALOG_BACK);
	m_bmShowTip.LoadBitmap(IDB_SHOW_TIP);
	m_bShowTip=true;
}

CAutoUpdateDlg::~CAutoUpdateDlg()
{
	if (m_pMemDC!=NULL)
	{
		m_pMemDC->SelectObject(m_pOldBitmap);
		delete m_pMemDC;
		delete m_pBitmapMem;
	}

}

void CAutoUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CURRENT_PROGRESS, m_Progress1);
	DDX_Control(pDX, IDC_TOTAL_PROGRESS, m_Progress2);
	DDX_Control(pDX, IDC_STATUS_TEXT, m_StatusText);
	DDX_Control(pDX, IDC_COUNT_TEXT, m_CountText);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}

BEGIN_MESSAGE_MAP(CAutoUpdateDlg, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_MESSAGE( WM_TRANSPARENT_BK, &CAutoUpdateDlg::OnTransaprentBk)
	ON_BN_CLICKED(IDCANCEL, &CAutoUpdateDlg::OnBnClickedCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CAutoUpdateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	////int nCrt = 0;
	////FILE* fp;
	////AllocConsole();
	////nCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
	////fp = _fdopen(nCrt, "w");
	////*stdout = *fp;
	////setvbuf(stdout, NULL, _IONBF, 0);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_StatusText.SetTextColor(RGB(255,255,255));
	m_CountText.SetTextColor(RGB(255,255,255));

	HWND hNotifyWindow = NULL;
	TCHAR szModuleFile[MAX_PATH]={0};
	CString strSiteURL;
	CString strTempDir;

	//查找窗口

	//	hNotifyWindow = ::FindWindow(_T("TaurusGamePlaza"), NULL);
	////#ifndef _DEBUG
	////	if (hNotifyWindow==NULL)
	////	{
	////		PostMessage(WM_CLOSE);
	////		return TRUE;
	////	}
	////#endif

	// 防多开
	HWND hWnd =::FindWindow(NULL,"AutoUpdate");
	if(hWnd>0)  
	{
		SetActiveWindow();
		BringWindowToTop();
		SetForegroundWindow();
		exit(0);  
	}
	SetWindowText("AutoUpdate");

	GetModuleFileName(AfxGetInstanceHandle(), szModuleFile, MAX_PATH);
	PathRemoveFileSpec(szModuleFile);
	PathAddBackslash(szModuleFile);
	m_strCurrentDir = szModuleFile;






	//////////////////// [2015/7/21 16:07 I'm Lei Feng]////////////////////////////////////////////////////////

	CString t_cmdLine = AfxGetApp()->m_lpCmdLine;
	if(t_cmdLine.GetLength() > 0)
	{
		int index = t_cmdLine.Find(' ');
		m_strXMLFileName = t_cmdLine.Mid(0,index);
		m_strXMLFileName = m_strXMLFileName.Mid(0,m_strXMLFileName.GetLength()-4);
		m_strXMLFileName += ".xml";

		m_strGameKindID = t_cmdLine.Mid(index+1);

		index = m_strGameKindID.Find(' ');

		if(index!=-1)
		{
			m_strServerID = m_strGameKindID.Mid(index+1);
			m_strGameKindID = m_strGameKindID.Mid(0,index);
			m_iServerID = atoi(m_strServerID.GetBuffer());
		}

		m_iGameKindID = atoi(m_strGameKindID.GetBuffer());

		//读取配置参数
		strSiteURL = AfxGetApp()->GetProfileString(_T("Download"), _T("site2"));
		strTempDir = AfxGetApp()->GetProfileString(_T("Setup"), _T("TempDir2"));

		//设置URL路径
		if( strSiteURL.Left(7) != HTTP_PROTOCOL )
			strSiteURL = HTTP_PROTOCOL + strSiteURL;

		strSiteURL += _T("/")+strTempDir+_T("/");

	}
	else
	{
		m_strXMLFileName = m_DefaultXMLFile;

		//读取配置参数
		strSiteURL = AfxGetApp()->GetProfileString(_T("Download"), _T("site1"));
		strTempDir = AfxGetApp()->GetProfileString(_T("Setup"), _T("TempDir1"));
		

		//设置URL路径
		if( strSiteURL.Left(7) != HTTP_PROTOCOL )
			strSiteURL = HTTP_PROTOCOL + strSiteURL;

		strSiteURL += _T("/")+strTempDir+_T("/");
	}

	//MessageBox(t_cmdLine, _T("test"),  MB_OK|MB_ICONINFORMATION);
	//MessageBox(m_strXMLFileName, _T("test"),  MB_OK|MB_ICONINFORMATION);
	//MessageBox(m_strGameKindID, _T("test"),  MB_OK|MB_ICONINFORMATION);


	m_DownloadMgr.Initialize((IDownloadSink*)this, strSiteURL, m_strCurrentDir+strTempDir, m_strCurrentDir);

	BITMAP bm;
	m_bmBack.GetBitmap(&bm);
	SetWindowPos(NULL,0,0,bm.bmWidth,bm.bmHeight,SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOMOVE|SWP_NOZORDER);

	ShowWindow(SW_SHOW);
	BringWindowToTop();
	BuildBkDC();
	Invalidate(FALSE);
	UpdateWindow();

	//检查版本
	if (!CheckVersionUpdate(strSiteURL + m_strXMLFileName))
	{
		//MessageBox(strSiteURL + m_strXMLFileName, _T("test"),  MB_OK|MB_ICONINFORMATION);
		//ASSERT(hNotifyWindow!=NULL);
		//::SendMessage(hNotifyWindow, WM_USER+102, 0, 0);
		//PostMessage(WM_CLOSE);
		OnStatusCallback(0, 0, DMS_ENDSETUP, NULL);
		return TRUE;
	}
	else
	{
		//::SendMessage(hNotifyWindow, WM_USER+102, 1, 0);
	}
	m_bShowTip=false;
	BuildBkDC();
	Invalidate(FALSE);

	m_btCancel.ShowWindow(SW_SHOW);
	m_StatusText.ShowWindow(SW_SHOW);
	m_CountText.ShowWindow(SW_SHOW);

	m_Progress1.ShowWindow(SW_SHOW);
	m_Progress2.ShowWindow(SW_SHOW);
	//设置范围
	m_Progress2.SetRange32(0, m_ulTotalLength/1024);

	// Send WM_NCCALCSIZE messages to the window
	m_Progress1.SetWindowPos(NULL, 0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	m_Progress2.SetWindowPos(NULL, 0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

	BringWindowToTop();
	UpdateWindow();

	//启动下载
	m_DownloadMgr.Start();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAutoUpdateDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);

		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		CRect rtClient;
		GetClientRect(&rtClient);

		if ( m_pMemDC )
		{
			dc.BitBlt( 0,0,rtClient.Width(), rtClient.Height(), m_pMemDC, 0,0, SRCCOPY);
		}
	}
}

BOOL CAutoUpdateDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CAutoUpdateDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	BuildBkDC();
	Invalidate(FALSE);
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAutoUpdateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAutoUpdateDlg::OnStatusCallback(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCTSTR pszStatusText)
{
	switch( ulStatusCode )
	{
	case DMS_SENDREQUEST:
		{
			break;
		}
	case DMS_BEGINDOWNLOAD:
		{
			m_ulCurentLenght = ulProgressMax;
			m_Progress1.SetRange32(0, ulProgressMax/1024);
			m_Progress1.SetPos(0);
			break;
		}
	case DMS_DOWNLOADDATA:
		{
			CString strText;
			CString strText2;
			strText.Format(TEXT("正在下载 %s "), pszStatusText);
			strText2.Format(TEXT("大小 %d KB  已下载 %d KB"), m_ulCurentLenght/1024, ulProgress/1024);
			m_StatusText.SetWindowText(strText);
			m_CountText.SetWindowText(strText2);

			m_Progress1.SetPos(ulProgress/1024);
			m_Progress2.SetPos(ulProgressMax/1024);
			break;
		}
	case DMS_ENDDOWNLOADDATA:
		{
			m_ulCurentLenght = 0;
			//m_Progress1.SetPos(0);
			break;
		}
	case DMS_FINISHDOWNLOAD:
		{
			m_btCancel.EnableWindow(FALSE);
			break;
		}
	case DMS_CANCELDOWNLOAD:
		{
			SendMessage(WM_CLOSE);
			break;
		}
	case DMS_ENDSETUP:
		{
			if (m_strXMLFileName.GetLength() > 0 && m_DefaultXMLFile != m_strXMLFileName)
			{
				HWND hWnd =::FindWindow(szPlazaClass,NULL);  
				if (hWnd>0)  
				{
					//////////////////// [2012/3/13 8:51 XF_Modify]////////////////////////////////////////////////////////
					if(m_iServerID)
						::SendMessage(hWnd,1125,m_iGameKindID,m_iServerID);
					else
						::SendMessage(hWnd,1125,m_iGameKindID,0);;
				}
				SendMessage(WM_CLOSE);
				break;
			}

			//启动程序
			TCHAR szModuleName[MAX_PATH]={0};
			GetModuleFileName(AfxGetInstanceHandle(), szModuleName, MAX_PATH);
			PathRemoveFileSpec(szModuleName);
			strcat(szModuleName,_T("\\GamePlaza.exe -"));
			strcat(szModuleName,g_strOldName.GetBuffer());
			WinExec(szModuleName, SW_SHOWDEFAULT);

			

			SendMessage(WM_CLOSE);
			break;
		}
	case DMS_ERROR:
		{
			TCHAR szWebUrl[255];
			TCHAR szModuleFile[MAX_PATH]={0};

			GetPrivateProfileString("DownUrl","WebUrl","www.85jn.com",szWebUrl,sizeof(szWebUrl),theApp.m_pszProfileName);
			CString strError;
			strError.Format(_T("更新失败，请到%s下载最新版本"),szWebUrl);
			strError = AfxGetApp()->GetProfileString(_T("messages"), _T("ErrorMsg"), strError);
			MessageBox(pszStatusText, _T("提示"), MB_OK|MB_ICONINFORMATION);
			MessageBox(strError, _T("提示"), MB_OK|MB_ICONINFORMATION);
			SendMessage(WM_CLOSE);
			break;
		}
	}
}

#define MAXSIZE 1024
#pragma comment(lib, "Wininet.lib")

int urlopen(TCHAR* url, CString & strXML)
{
	strXML.Empty();
	BYTE cbBuffer[MAXSIZE];
	HINTERNET hSession = InternetOpen(TEXT("UrlTest"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	if (hSession != NULL)
	{
		HINTERNET hHttp = InternetOpenUrl(hSession, url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);

		if (hHttp != NULL)
		{
			ULONG Number = 1;
			while (Number > 0)
			{
				InternetReadFile(hHttp, cbBuffer, MAXSIZE - 1, &Number);
				cbBuffer[Number] = '\0';
				strXML.Append((char*)cbBuffer);
			}
			InternetCloseHandle(hHttp);
			hHttp = NULL;
		}
		else
			return __LINE__;

		InternetCloseHandle(hSession);
		hSession = NULL;
	}
	else
		return __LINE__;

	return 0;
}

BOOL CAutoUpdateDlg::CheckVersionUpdate(CString& strUpdateURL)
{
	BOOL bHasUpdate = FALSE;

	//HRESULT hr = S_OK;
	//IXMLHTTPRequestPtr pHttpRequest;
	//IDispatchPtr pDispatch;
	MSXML2::IXMLDOMDocumentPtr pXmlDoc;
	MSXML2::IXMLDOMNodeListPtr pList;
	MSXML2::IXMLDOMElementPtr pChild;

	UINT nFileSize;
	CString strFileName, strFileVer, strMD5String;
	LONG lElementCount = 0L;

	try
	{
		//hr = pHttpRequest.CreateInstance(TEXT("Msxml2.XMLHTTP.3.0"));
		//if( FAILED(hr) )
		//	_com_issue_error(hr);

		//hr = pHttpRequest->open(TEXT("GET"), (_bstr_t)strUpdateURL, false);
		//if( FAILED(hr) )
		//	_com_issue_error(hr);

		//hr = pHttpRequest->send();
		//if( FAILED(hr) )
		//	_com_issue_error(hr);

		//if (pHttpRequest->Getstatus() != 200)
		//	throw (0);

		//pDispatch = pHttpRequest->GetresponseXML();
		//hr = pDispatch->QueryInterface(pXmlDoc.GetIID(), (void**)&pXmlDoc);
		//if( FAILED(hr) )
		//	_com_issue_error(hr);

		CString strTemp;
		int nRet = urlopen(strUpdateURL.GetBuffer(), strTemp);
		if (nRet != 0) throw (nRet);

		if (strTemp.Find("无法找到该页") != -1)
			throw ((int)__LINE__);

		pXmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30));
		pXmlDoc->loadXML(strTemp.GetBuffer());

		pList = pXmlDoc->selectNodes("/manifest/filelist/file");
		lElementCount = pList->Getlength();
		for( LONG i = 0; i < lElementCount; i++ )
		{
			pChild = pList->Getitem(i);
			strFileName = pChild->getAttribute("filename");
			nFileSize = pChild->getAttribute("filesize");
			strFileVer = pChild->getAttribute("fileversion");
			strMD5String = pChild->getAttribute("md5");

			//不更新注册信息
			if(strFileName==TEXT("Reg.Local")) continue;

			//如果本地文件存在则效验文件，不存在就下载这个文件
			if (PathFileExists(m_strCurrentDir+strFileName))
			{
				//效验文件,如果MD5码不相同则重新下载
				if (CMD5Checksum::GetMD5(m_strCurrentDir+strFileName) != strMD5String)
				{
					m_ulTotalLength += nFileSize;
					m_DownloadMgr.AddTask(strFileName);
					bHasUpdate = TRUE;
				}	
			}
			else
			{
				m_ulTotalLength += nFileSize;
				m_DownloadMgr.AddTask(strFileName);
				bHasUpdate = TRUE;
			}
		}

		return bHasUpdate;
	}

#ifdef _DEBUG
	catch(_com_error& e)
	{
		CString strError;
		strError.Format(_T("Error code:%d\nMessage:%s\nDescrption:%s"), (int)e.WCode(), e.ErrorMessage(), (TCHAR*)e.Description());
		MessageBox(strError, _T("提示"), MB_OK|MB_ICONWARNING);
		return FALSE;
	}
#endif
	catch(int line)
	{
		CString strError;
		strError.Format(TEXT("Error line:%d\n"), line);
		MessageBox(strError, TEXT("提示"), MB_OK|MB_ICONWARNING);
		return FALSE;
	}
	catch(...)
	{
		return FALSE;
	}
}

void CAutoUpdateDlg::OnBnClickedCancel()
{
	if ( m_DownloadMgr.GetStatus() == DMS_FINISHDOWNLOAD || m_DownloadMgr.GetStatus() == DMS_ERROR )
	{
		DestroyWindow();
	}
	else
	{
		m_DownloadMgr.CancelTask();
		m_btCancel.EnableWindow(FALSE);
	}
}

void CAutoUpdateDlg::OnClose()
{
	DestroyWindow();
}

void CAutoUpdateDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
}

LRESULT CAutoUpdateDlg::OnTransaprentBk( WPARAM wParam, LPARAM lParam )
{
	HDC hdc = ( HDC)wParam;
	HWND hwnd = ( HWND)lParam;
	CTransparentHelper::OnTranparentControl( m_pMemDC->GetSafeHdc(), (WPARAM)hdc, (LPARAM)hwnd);
	return TRUE;
}

void CAutoUpdateDlg::BuildBkDC()
{
	CDC* pDC;
	CDC MemDC;
	CBitmap* pOldBitmap;
	CRect rtClient;

	pDC = GetDC();
	MemDC.CreateCompatibleDC(pDC);
	pOldBitmap = MemDC.SelectObject( &m_bmBack );

	GetClientRect(&rtClient);

	if ( m_pMemDC )
	{
		m_pMemDC->SelectObject( m_pOldBitmap);
		delete m_pBitmapMem;
		m_pBitmapMem = NULL;
		delete m_pMemDC;
		m_pMemDC = NULL;
	}

	if ( m_pMemDC == NULL )
	{
		m_pMemDC = new CDC;
		m_pMemDC->CreateCompatibleDC(pDC);

		m_pBitmapMem = new CBitmap;
		m_pBitmapMem->CreateCompatibleBitmap(pDC, rtClient.Width(), rtClient.Height());
		m_pOldBitmap = (CBitmap*)m_pMemDC->SelectObject( m_pBitmapMem);
	}

	//if(m_bShowTip)
	//	m_bmShowTip->
	m_pMemDC->BitBlt( 0,0, rtClient.Width(), rtClient.Height(), &MemDC, 0, 0, SRCCOPY);
	MemDC.SelectObject( pOldBitmap );

	if(m_bShowTip)
	{
		pOldBitmap = MemDC.SelectObject( &m_bmShowTip );
		m_pMemDC->BitBlt( 0,0, rtClient.Width(), rtClient.Height(), &MemDC, 0, 0, SRCCOPY);
		MemDC.SelectObject( pOldBitmap );
	}

	ReleaseDC(pDC);

	//when the parent dialog's background is rebuild, notify the child which has an transparent tag.
	CTransparentHelper::NotifyTransparentChild( GetSafeHwnd());
}