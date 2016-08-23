#include "stdafx.h"
#include "Resource.h"
#include "GlobalFuncUnits.h"
#include "UpdateConfig.h"
#include "UpdateConfigDlg.h"

//////////////////////////////////////////////////////////////////////////////////////////
//标识定义
#define IDC_LIST_VIEW								100

//常量定义
#define UPDATE_CONFIG_NAME							TEXT("Update.ini")

//消息定义
#define WM_UPDATE_PROGRESS							(WM_USER+100)

//更新命令
#define PROGRESS_CMD_SETUP							0
#define PROGRESS_CMD_UPDATE							1	

//////////////////////////////////////////////////////////////////////////////////////////
//消息映射
BEGIN_MESSAGE_MAP(CUpdateConfigDlg, CDialog)

	//系统消息
	ON_WM_ERASEBKGND()

	ON_MESSAGE(WM_UPDATE_PROGRESS,OnUpdateProgress)

	//控件消息
	ON_BN_CLICKED(IDC_RADIO_ALL, OnBnClickedRadioAll)
	ON_BN_CLICKED(IDC_RADIO_PLATFORM, OnBnClickedRadioPlatform)
	ON_BN_CLICKED(IDC_RADIO_MULTI_GAME, OnBnClickedRadioGames)
	ON_BN_CLICKED(IDC_BUTTON_DONE, OnBnClickedButtonDone)
	ON_BN_CLICKED(IDC_BROWSE_INPUTPATH, OnBnClickedBrowseInputpath)
	ON_BN_CLICKED(IDC_BROWSE_OUTPUTPATH, OnBnClickedBrowseOutputpath)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////////////

//构造函数
CUpdateConfigDlg::CUpdateConfigDlg(CWnd* pParent) : CDialog(IDD_DIALOG_UPDATEMAIN, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//配置变量
	m_szGameExpandName[0]=0;
	m_szPlazaConfigName[0]=0;	
}

//数据交换
void CUpdateConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

	DDX_Control(pDX,IDC_COMBO_MULTI_GAME,m_ComboGameList);
}

//命令消息
BOOL CUpdateConfigDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	DWORD dwCommandID = HIWORD(wParam);
	if(m_ComboGameList.m_hWnd == (HWND)lParam)
	{
		switch(dwCommandID)	
		{
		case CBN_SELCHANGE:
			{
				//获取控件
				CButton * pRadioButton=static_cast<CButton *>(GetDlgItem(IDC_RADIO_MULTI_GAME));
				if(pRadioButton && pRadioButton->GetCheck()==BST_CHECKED)
				{
					//获取路径
					TCHAR szInputPath[MAX_PATH]=TEXT("");
					GetDlgItemText(IDC_EDIT_INPUT_PATH,szInputPath,CountArray(szInputPath));

					//筛选文件
					FilterFiles(szInputPath,FILE_KIND_GAME,m_ComboGameList.GetText());
				}

				return TRUE;
			}
		}
	}

	return __super::OnCommand(wParam,lParam);
}

//初始对话框
BOOL CUpdateConfigDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//设置图标
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	//创建控件
	CRect rcCreate(12,215,560,543);
	m_FileListView.Create(WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_LIST_VIEW);

	//工作目录
	TCHAR szWorkDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szWorkDirectory,CountArray(szWorkDirectory));

	//变量定义
	CString szConfigFile;
	szConfigFile.Format(TEXT("%s\\%s"),szWorkDirectory,UPDATE_CONFIG_NAME);

	//加载配置
	CFileFind filefind;
	if(filefind.FindFile(szConfigFile)==TRUE) LoadConfig(szConfigFile);

	return TRUE;
}

//加载配置
bool CUpdateConfigDlg::LoadConfig(LPCTSTR pszConfigFile)
{
	//读取配置
	CWHIniData IniData;
	IniData.SetIniFilePath(pszConfigFile);

	//存放路径
	TCHAR szInputPath[MAX_PATH]=TEXT("");
	IniData.ReadString(TEXT("PATHCONFIG"),TEXT("INPUT_PATH"),TEXT(""),szInputPath,sizeof(szInputPath));

	//网络路径
	TCHAR szInputNetAddr[MAX_PATH]=TEXT("");
	IniData.ReadString(TEXT("PATHCONFIG"),TEXT("INPUT_NET_ADDR"),TEXT(""),szInputNetAddr,sizeof(szInputNetAddr));

	//输出路径
	TCHAR szOutPutPath[MAX_PATH]=TEXT("");
	IniData.ReadString(TEXT("PATHCONFIG"),TEXT("OUTPUT_PATH"),TEXT(""),szOutPutPath,sizeof(szOutPutPath));

	//平台名称
	IniData.ReadString(TEXT("NAMECONFIG"),TEXT("PLAZA_CONFIG_NAME"),TEXT("Client.txt"),m_szPlazaConfigName,sizeof(m_szPlazaConfigName));

	//游戏扩展名
	IniData.ReadString(TEXT("NAMECONFIG"),TEXT("GAME_EXPAND_NAME"),TEXT(".txt"),m_szGameExpandName,sizeof(m_szGameExpandName));	

	//设置控件
	SetDlgItemText(IDC_EDIT_INPUT_PATH,szInputPath);
	SetDlgItemText(IDC_EDIT_INPUT_NET_ADDR,szInputNetAddr);
	SetDlgItemText(IDC_EDIT_OUTPUT_PATH,szOutPutPath);

	//获取按钮
	CButton * pRadioButton = static_cast<CButton *>(GetDlgItem(IDC_RADIO_ALL));
	if(pRadioButton != NULL) pRadioButton->SetCheck(BST_CHECKED);

	//筛选文件
	FilterFiles(szInputPath,FILE_KIND_GAME|FILE_KIND_PLATFORM,NULL);

	return true;
}

//保存配置
bool CUpdateConfigDlg::SaveConfig(LPCTSTR pszConfigFile)
{
	//存放路径
	TCHAR szInputPath[MAX_PATH]=TEXT("");
	GetDlgItemText(IDC_EDIT_INPUT_PATH,szInputPath,CountArray(szInputPath));

	//网络路径
    TCHAR szInputNetAddr[MAX_PATH]=TEXT("");
	GetDlgItemText(IDC_EDIT_INPUT_NET_ADDR,szInputNetAddr,CountArray(szInputNetAddr));	

	//输出路径
	TCHAR szOutPutPath[MAX_PATH]=TEXT("");
	GetDlgItemText(IDC_EDIT_OUTPUT_PATH,szOutPutPath,CountArray(szOutPutPath));

	//打开文件
	CFile ConfigFile;
	if (ConfigFile.Open(pszConfigFile,CFile::modeReadWrite|CFile::modeCreate,NULL)==FALSE)
	{
		return false;
	}

	//保存数据
	TCHAR szResult[10240]=TEXT("");
	TCHAR szFileData[10240]=TEXT("");
	LPCTSTR pszInfo=TEXT("-----------------网狐棋牌 6603s 更新配置文件  版权：深圳市网狐科技有限公司------------------ \r\n\r\n");

	//UNICODE
#ifdef _UNICODE
	BYTE cbUnicodeHead[]={0xFF,0xFE};
	ConfigFile.Write(cbUnicodeHead,sizeof(cbUnicodeHead));
#endif

	//开始字段
	ConfigFile.Write(pszInfo,lstrlen(pszInfo)*sizeof(TCHAR));

	//路径配置
	_sntprintf(szFileData,CountArray(szFileData),TEXT("[PATHCONFIG]\r\n") TEXT("INPUT_PATH=%s\r\n") TEXT("INPUT_NET_ADDR=%s\r\n") TEXT("OUTPUT_PATH=%s\r\n\r\n"), szInputPath, szInputNetAddr, szOutPutPath);
	ConfigFile.Write(szFileData,lstrlen(szFileData)*sizeof(TCHAR));

	//名称配置
	_sntprintf(szFileData, CountArray(szFileData), TEXT("[NAMECONFIG]\r\n") TEXT("PLAZA_CONFIG_NAME=%s\r\n") TEXT("GAME_EXPAND_NAME=%s\r\n"), m_szPlazaConfigName, m_szGameExpandName);
	ConfigFile.Write(szFileData,lstrlen(szFileData)*sizeof(TCHAR));

	return true;
}

//统计文件
void CUpdateConfigDlg::CensusFileCount(LPCTSTR pszFilePath,int & nFileCount)
{
	//变量定义	
	size_t pathlen = 0;
	TCHAR szFind[MAX_PATH];
	WIN32_FIND_DATA FindFileData;

	//变量定义
	TCHAR szFilename[MAX_PATH]=TEXT(""); 	

	//连接字符
	lstrcpy(szFind,pszFilePath);
	lstrcat(szFind,TEXT("\\*.*"));

	//查找文件
	HANDLE hFind=::FindFirstFile(szFind,&FindFileData);
	if(INVALID_HANDLE_VALUE == hFind) return;

	//遍历文件
	while(TRUE)
	{
		//分析文件
		if (FindFileData.cFileName[0] != '.') 
		{
			//格式字符
			_sntprintf(szFilename, sizeof(szFilename),TEXT("%s\\%s"), pszFilePath, FindFileData.cFileName);			

			//递归目录
			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				CensusFileCount(szFilename, nFileCount);
			}
			else if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)==0)
			{
				//变量定义
				FILE *fHandle = NULL;

				//存在判断
				fHandle=fopen(CT2CA(szFilename),"r");
                if(fHandle==NULL) return;

				//累计数目
				++nFileCount;
                
				//关闭文件
				fclose(fHandle);
			}
		}

		//查找下一个
		if(!FindNextFile(hFind,&FindFileData)) break;
	}

	//结束查找
	FindClose(hFind);
}

//查找文件
void CUpdateConfigDlg::SearchFiles(LPCTSTR pszFilePath,LPCTSTR pszPrefix,CStringArray & StringArray)
{
	//变量定义	
	size_t pathlen = 0;
	TCHAR szFind[MAX_PATH];
	WIN32_FIND_DATA FindFileData;

	//变量定义
	TCHAR szPrefix[MAX_PATH]=TEXT("");
	TCHAR szFilename[MAX_PATH]=TEXT(""); 	

	//连接字符
	lstrcpy(szFind,pszFilePath);
	lstrcat(szFind,TEXT("\\*.*"));

	//查找文件
	HANDLE hFind=::FindFirstFile(szFind,&FindFileData);
	if(INVALID_HANDLE_VALUE == hFind) return;

	//遍历文件
	while(TRUE)
	{
		//分析文件
		if (FindFileData.cFileName[0] != '.') 
		{
			//格式字符
			_sntprintf(szPrefix, sizeof(szPrefix),TEXT("%s%s"),  pszPrefix, FindFileData.cFileName);
			_sntprintf(szFilename, sizeof(szFilename),TEXT("%s\\%s"), pszFilePath, FindFileData.cFileName);			

			//递归目录
			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				lstrcat(szPrefix, TEXT("\\"));
				SearchFiles(szFilename, szPrefix, StringArray);
			}
			else if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)==0)
			{
				//变量定义
				FILE *fHandle = NULL;
				TCHAR pszFileName[MAX_PATH]=TEXT(""); 

				//存在判断
				fHandle=fopen(CT2CA(szFilename),"r");
                if(fHandle==NULL) return;
				
				//格式字符
				_sntprintf(pszFileName, sizeof(pszFileName), TEXT("%s%s"), pszPrefix, FindFileData.cFileName);
		
				//加入列表
				StringArray.Add(pszFileName);
                
				//关闭文件
				fclose(fHandle);
			}
		}

		//查找下一个
		if(!FindNextFile(hFind,&FindFileData)) break;
	}

	//结束查找
	FindClose(hFind);
}

//筛选文件
void CUpdateConfigDlg::FilterFiles(LPCTSTR pszFilePath,BYTE cbFileKind,LPCTSTR pszGameNames)
{
	//变量定义
	CFileFind FileFind;	
	CString strFilePath;
	CString szDirectory(pszFilePath);	

	//文件信息
	WORD wFileCount=0;
	tagFileInfo FileInfo={0};
	 
	//补齐斜杠
	if(szDirectory.Right(1)!= TEXT("\\"))
	{
		szDirectory += TEXT("\\");
	}
	 
	//设置变量
	szDirectory += TEXT("*.*");

	//删除子项	
	m_FileListView.DeleteAllItems();	

	try
	{
		//查找文件
		BOOL bFind = FileFind.FindFile(szDirectory);
		while(bFind)
		{
			//查找下一个
			bFind = FileFind.FindNextFile();		  

			//文件过滤
			if(FileFind.IsHidden()) continue;

			//目录是文件夹
			if(FileFind.IsDirectory())
			{
				//目录过滤
				if(FileFind.IsDots()) continue;

				//设置变量
				FileInfo.bIsDirectory=TRUE;	
				FileInfo.dwFileSize=0;
				FileInfo.szFileVersion[0]=0;
			}
			else
			{
				//变量定义
				strFilePath = FileFind.GetFilePath();

				//设置变量
				FileInfo.bIsDirectory=FALSE;
				FileInfo.dwFileSize=GetFileSize(strFilePath);

				//获取版本
				GetFileVersion(strFilePath,FileInfo.szFileVersion,CountArray(FileInfo.szFileVersion));
			}

			//修改时间
			FileFind.GetLastWriteTime(&FileInfo.ftModifyDate);

			//文件名称		
			lstrcpyn(FileInfo.szFileName,FileFind.GetFileName(),CountArray(FileInfo.szFileName));

			//标记类型
			MarkFileKind(pszFilePath,FileInfo);
			if(FileInfo.cbFileKind == FILE_KIND_GAME && FileInfo.bIsDirectory && pszGameNames == NULL)
			{
				m_ComboGameList.AddString(FileInfo.szFileName);
			}

			if((FileInfo.cbFileKind&cbFileKind)!=0)
			{
				//标识变量
				bool bAddItem=true;

				if((cbFileKind&FILE_KIND_GAME) != 0 && pszGameNames != NULL)
				{	
					//变量定义
					CString strGameNames;
					strGameNames.Format(TEXT(", %s,"),pszGameNames);

					//变量定义
					CString strFileName(FileInfo.szFileName);
					int nPos = strFileName.Find(TEXT('.'));
					if(nPos != -1) strFileName = strFileName.Left(nPos);

					//变量定义
					CString strSearchName;
					strSearchName.Format(TEXT(", %s,"),strFileName);

					//查找字串
					if(strGameNames.Find(strSearchName) == -1) 
					{
						bAddItem=false;
					}
				}

				//添加子项
				if(bAddItem == true)
				{
					m_FileListView.AddItem(wFileCount++,&FileInfo);
				}
			}
		}

		//关闭句柄
		FileFind.Close();

	}
	catch(...){}

	//按名称排序
	m_FileListView.Sort(0);

	return;
}

//使能设置
VOID CUpdateConfigDlg::EnableControls(bool bEnabled)
{
	//设置控件	
	GetDlgItem(IDC_EDIT_INPUT_PATH)->EnableWindow(bEnabled);
	GetDlgItem(IDC_EDIT_OUTPUT_PATH)->EnableWindow(bEnabled);
	GetDlgItem(IDC_EDIT_INPUT_NET_ADDR)->EnableWindow(bEnabled);

	//设置控件
	GetDlgItem(IDC_RADIO_ALL)->EnableWindow(bEnabled);
	GetDlgItem(IDC_RADIO_PLATFORM)->EnableWindow(bEnabled);
	GetDlgItem(IDC_RADIO_MULTI_GAME)->EnableWindow(bEnabled);
	GetDlgItem(IDC_COMBO_MULTI_GAME)->EnableWindow(bEnabled);

	//设置控件
	GetDlgItem(IDC_BUTTON_DONE)->EnableWindow(bEnabled);
	GetDlgItem(IDC_BROWSE_INPUTPATH)->EnableWindow(bEnabled);
	GetDlgItem(IDC_BROWSE_OUTPUTPATH)->EnableWindow(bEnabled);
}

//文件大小
DWORD CUpdateConfigDlg::GetFileSize(LPCTSTR pszFileName)
{
	//变量定义
	WIN32_FIND_DATA fileData;
    HANDLE hFind;
    DWORD dwFileSize=0;

	//查找文件
    hFind = FindFirstFile(pszFileName,&fileData);
    if(hFind != INVALID_HANDLE_VALUE)
	{
        dwFileSize = fileData.nFileSizeLow;
	}
   
	//关闭句柄
    FindClose(hFind);

    return dwFileSize;
}

//文件存在
BOOL CUpdateConfigDlg::IsFileExist(LPCTSTR pszFileName)
{
	//变量定义
	CFileFind FileFind;
    return FileFind.FindFile(pszFileName);
}

//目录存在
BOOL CUpdateConfigDlg::IsDirectoryExist(LPCTSTR pszDirectory)
{
	//变量定义
	WIN32_FIND_DATA wfd;
    BOOL bResult = FALSE;

	//查找文件
    HANDLE hFind = FindFirstFile(pszDirectory, &wfd);
    if ((hFind!=INVALID_HANDLE_VALUE) &&(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
    {
        bResult = TRUE;
    }

	//关闭句柄
    FindClose(hFind);

    return bResult;
}

//游戏进程
BOOL CUpdateConfigDlg::IsGameProcess(LPCTSTR pszFileName)
{
	//加载资源	
	bool bGameProcess=false;
	HINSTANCE hInstance=AfxLoadLibrary(pszFileName);

	//加载图形	
	CBitmap GameLogo;
	AfxSetResourceHandle(hInstance);
	if (GameLogo.LoadBitmap(TEXT("GAME_LOGO"))) bGameProcess=true;
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//释放资源
	AfxFreeLibrary(hInstance);

	return bGameProcess;
}

//标记类型
VOID CUpdateConfigDlg::MarkFileKind(LPCTSTR pszWorkPath,tagFileInfo & FileInfo)
{
	//设置类型
	FileInfo.cbFileKind=FILE_KIND_PLATFORM;

	//目录判断
	if(FileInfo.bIsDirectory==TRUE)
	{
		//变量定义
		CString strFilePath;
		strFilePath.Format(TEXT("%s\\%s.exe"),pszWorkPath,FileInfo.szFileName);

		//查找同名目录
		if(IsFileExist(strFilePath) == TRUE && IsGameProcess(strFilePath) == TRUE)
		{
			FileInfo.cbFileKind=FILE_KIND_GAME;
		}
	}
	else
	{
		//变量定义		
		CString strFileName(FileInfo.szFileName);

		//执行文件
		if(strFileName.Find(TEXT(".exe")) > 0)
		{
			//变量定义
			CString strFilePath,strFileName1;

			//格式化路径
			strFilePath.Format(TEXT("%s\\%s"),pszWorkPath,strFileName.Left(strFileName.Find(TEXT('.'))));
			strFileName1.Format(TEXT("%s\\%s"),pszWorkPath,strFileName);
			
			//查找同名目录
			if(IsDirectoryExist(strFilePath) == TRUE && IsGameProcess(strFileName1) == TRUE)
			{
				FileInfo.cbFileKind=FILE_KIND_GAME;
			}
		}
	}
}

//构造更新
BOOL CUpdateConfigDlg::MakeUpdateConfig(CStringArray & FileNameArray,LPCTSTR pszConfigName)
{
	//变量定义	
	CString szCheckSum;
	CString szFileCount;
	CString szConfigFile;
	CString szInputPath,szInputNetAddr,szOutputPath;

	//获取路径
	GetDlgItemText(IDC_EDIT_INPUT_PATH,szInputPath);
	GetDlgItemText(IDC_EDIT_INPUT_NET_ADDR,szInputNetAddr);
	GetDlgItemText(IDC_EDIT_OUTPUT_PATH,szOutputPath);

	//输入验证
	if(szInputPath==TEXT("")) return FALSE;
	if(szInputNetAddr==TEXT("")) return FALSE;
	if(szOutputPath==TEXT("")) return FALSE;

	//格式字符
	szConfigFile.Format(TEXT("%s/%s"),szOutputPath,pszConfigName);

	//计算大小
	WORD wFileCount=(WORD)FileNameArray.GetCount();
	DWORD dwDataSize=sizeof(tagUpdateInfo)+sizeof(tagUpdateItem)*wFileCount;

	//设置内存
	DWORD dwDataIndex=0;
	BYTE * pFileData=new BYTE[dwDataSize];
	if(pFileData==NULL) throw TEXT("内存不足！");

	//初始数据
	ZeroMemory(pFileData,dwDataSize);

	//更新信息
	tagUpdateInfo UpdateInfo;
	UpdateInfo.wUpdateCount=wFileCount;
	lstrcpynA(UpdateInfo.szDownLoadAddress, CT2CA(szInputNetAddr), CountArray(UpdateInfo.szDownLoadAddress));
	UpdateInfo.wStructSize=sizeof(UpdateInfo)-sizeof(UpdateInfo.szDownLoadAddress);
	UpdateInfo.wStructSize+=CountStringBufferA(UpdateInfo.szDownLoadAddress);

	//拷贝数据
	CopyMemory(pFileData, &UpdateInfo, UpdateInfo.wStructSize);
	dwDataIndex+=UpdateInfo.wStructSize;

	//变量定义
	tagUpdateItem UpdateItem;

	//生成文件
	for(INT_PTR nIndex=0;nIndex<FileNameArray.GetCount();nIndex++)
	{
		//事件处理
		DoEvents();

		//变量定义
		CString  szFilePath;		

		//获取校验
		szFilePath.Format(TEXT("%s\\%s"),szInputPath,FileNameArray[nIndex]);
		szCheckSum=CWHMD5Checksum::GetMD5(szFilePath);

		ZeroMemory(&UpdateItem,sizeof(UpdateItem));

        //设置变量		
		lstrcpynA(UpdateItem.szFileName,CT2CA(FileNameArray[nIndex]),CountArray(UpdateItem.szFileName));
		lstrcpynA(UpdateItem.szFileCheckSum,CT2CA(szCheckSum),CountArray(UpdateItem.szFileCheckSum));

		//计算大小
		UpdateItem.wStructSize=sizeof(UpdateItem)-sizeof(UpdateItem.szFileName);
		UpdateItem.wStructSize+=CountStringBufferA(UpdateItem.szFileName);

		//拷贝数据
		CopyMemory(&pFileData[dwDataIndex],&UpdateItem,UpdateItem.wStructSize);
		dwDataIndex+=UpdateItem.wStructSize;

		//更新进度
		PostMessage(WM_UPDATE_PROGRESS,PROGRESS_CMD_UPDATE);
	}

	//创建文件
	CFile FileDownLoad;
	if (FileDownLoad.Open(szConfigFile,CFile::modeCreate|CFile::modeReadWrite|CFile::shareDenyNone)==FALSE)
	{
		::AfxMessageBox(TEXT("更新文件创建失败！"));

		//释放资源
		SafeDeleteArray(pFileData);

		return FALSE;
	}

	//压缩数据
	DWORD dwResultSize=0;
	BYTE * pResultData=new BYTE[dwDataIndex];
	ZeroMemory(pResultData,sizeof(pResultData));
	dwResultSize=CWHService::CompressData(pFileData,dwDataIndex,pResultData,dwDataIndex);

	//写入文件
	FileDownLoad.Write(pResultData,dwResultSize);
	FileDownLoad.Close();

	//释放资源
	SafeDeleteArray(pFileData);
	SafeDeleteArray(pResultData);

	return TRUE;
}

//文件版本
BOOL CUpdateConfigDlg::GetFileVersion(LPCTSTR pszFileName, TCHAR szFileVersion[],WORD wBufferCount)
{
	//设置变量
	szFileVersion[0]=0;

	//获取版本
	DWORD dwFileVersion;
	if(CWHService::GetModuleVersion(pszFileName,dwFileVersion))
	{
		//格式转换
		BYTE * pcbFileVersion = (BYTE *)&dwFileVersion;
		_sntprintf(szFileVersion,wBufferCount,TEXT("%d.%d.%d.%d"),pcbFileVersion[3],pcbFileVersion[2],pcbFileVersion[1],pcbFileVersion[0]);

		return TRUE;
	}

	return FALSE;
}

//绘画边框
VOID CUpdateConfigDlg::DrawControlFrame(CDC * pDC,CRect & rcFrame)
{
	//绘制边框
	pDC->MoveTo(rcFrame.left,rcFrame.top);
	pDC->LineTo(rcFrame.right,rcFrame.top);
	pDC->MoveTo(rcFrame.right,rcFrame.top);
	pDC->LineTo(rcFrame.right,rcFrame.bottom);
	pDC->MoveTo(rcFrame.right,rcFrame.bottom);
	pDC->LineTo(rcFrame.left,rcFrame.bottom);
	pDC->MoveTo(rcFrame.left,rcFrame.bottom);
	pDC->LineTo(rcFrame.left,rcFrame.top);
}

//浏览目录
CString CUpdateConfigDlg::GetBrowseFolder(HWND hParent,CString strTitle)
{
	//变量定义   
    BROWSEINFO bi;
    ITEMIDLIST *pidl=NULL;
	TCHAR szDirectory[MAX_PATH]={0};

	//构造结构
    bi.hwndOwner=hParent;
    bi.pidlRoot=NULL;    
    bi.lpszTitle=strTitle;   
    bi.lpfn=NULL;
    bi.lParam=0;
    bi.iImage=0;
	bi.pszDisplayName=szDirectory;
	bi.ulFlags=BIF_RETURNONLYFSDIRS;

	//打开目录
    pidl = SHBrowseForFolder(&bi);
    if(NULL == pidl) return TEXT("");

	//获取目录	
    if(!SHGetPathFromIDList(pidl,szDirectory)) return TEXT("");

    return CString(szDirectory);
}

//绘画背景
BOOL CUpdateConfigDlg::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);

	//获取对象
	CRect rcControl;

	//获取区域
	m_FileListView.GetWindowRect(&rcControl);    

	//坐标转换
	ScreenToClient(&rcControl);

	//区域膨胀
	rcControl.InflateRect(1,1,1,1);

	//绘画框架
	DrawControlFrame(pDC,rcControl);

	return TRUE;
}

//更新进度
LRESULT CUpdateConfigDlg::OnUpdateProgress(WPARAM wParam, LPARAM lParam)
{
	//获取对象
	CProgressCtrl * pProgress = static_cast<CProgressCtrl * >(GetDlgItem(IDC_PROGRESS));
	if(pProgress == NULL) return 0;

	//变量定义
	WORD wProgressCmd=(WORD)wParam;
	WORD wProgressValue=(WORD)lParam;

	//设置进度
	if(PROGRESS_CMD_SETUP == wProgressCmd)
	{
		pProgress->SetPos(0);
		pProgress->SetRange(0,wProgressValue);		
	}

	//更新进度
	if(PROGRESS_CMD_UPDATE == wProgressCmd)
	{		
		pProgress->SetPos(pProgress->GetPos()+1);
	}

	return 1;
}

//全部文件
void CUpdateConfigDlg::OnBnClickedRadioAll()
{
	//获取路径
	TCHAR szInputPath[MAX_PATH]=TEXT("");
	GetDlgItemText(IDC_EDIT_INPUT_PATH,szInputPath,CountArray(szInputPath));

	//移除列表
	while(m_ComboGameList.GetCount()) m_ComboGameList.DeleteString(0);

	//筛选文件
	FilterFiles(szInputPath,FILE_KIND_GAME|FILE_KIND_PLATFORM,NULL);
}

//平台文件
void CUpdateConfigDlg::OnBnClickedRadioPlatform()
{
	//获取路径
	TCHAR szInputPath[MAX_PATH]=TEXT("");
	GetDlgItemText(IDC_EDIT_INPUT_PATH,szInputPath,CountArray(szInputPath));

	//移除列表
	while(m_ComboGameList.GetCount()) m_ComboGameList.DeleteString(0);

	//筛选文件
	FilterFiles(szInputPath,FILE_KIND_PLATFORM,NULL);
}

//多款游戏
void CUpdateConfigDlg::OnBnClickedRadioGames()
{
	//获取路径
	TCHAR szInputPath[MAX_PATH]=TEXT("");
	GetDlgItemText(IDC_EDIT_INPUT_PATH,szInputPath,CountArray(szInputPath));
	
	//移除列表
	while(m_ComboGameList.GetCount()) m_ComboGameList.DeleteString(0);

	//筛选文件
	FilterFiles(szInputPath,FILE_KIND_GAME,NULL);

	//全部选择
	m_ComboGameList.SelectAll(TRUE);
}

//生成更新
void CUpdateConfigDlg::OnBnClickedButtonDone()
{
	//变量定义
	CStringArray GameFileArray;
	CStringArray PlatformFileArray;	

	//变量定义
	CWHArray<tagItemInfo *,tagItemInfo *> GameItemArray;
	CWHArray<tagItemInfo *,tagItemInfo *> PlatformItemArray;

	//获取路径
	TCHAR szInputPath[MAX_PATH]=TEXT("");
	GetDlgItemText(IDC_EDIT_INPUT_PATH,szInputPath,CountArray(szInputPath));

	//变量定义
	tagItemInfo * pItemInfo=NULL;
	CString strInputPath,strPrefix,strConfigName;

	//禁用控件
	EnableControls(false);

	//统计文件数
	int nFileCount=0; 
	
	//子项分类
	for(int i = 0; i < m_FileListView.GetItemCount(); i++)
	{
		pItemInfo = reinterpret_cast<tagItemInfo*>(m_FileListView.GetItemData(i));
		if(pItemInfo != NULL)
		{
			//平台子项
			if(pItemInfo->cbFileKind == FILE_KIND_PLATFORM)
			{
				PlatformItemArray.Add(pItemInfo);

				//文件统计				
				if(pItemInfo->bIsDirectory == TRUE)
				{
					strInputPath.Format(TEXT("%s\\%s"),szInputPath,pItemInfo->strFileName);
					CensusFileCount(strInputPath,nFileCount);
				}
				else
				{
					++nFileCount;
				}
			}

			//游戏子项
			if(pItemInfo->cbFileKind == FILE_KIND_GAME && pItemInfo->bIsDirectory == TRUE)
			{
				GameItemArray.Add(pItemInfo);

				//文件统计
				strInputPath.Format(TEXT("%s\\%s"),szInputPath,pItemInfo->strFileName);
				CensusFileCount(strInputPath,++nFileCount);
			}
		}
	}	

	//设置进度
	PostMessage(WM_UPDATE_PROGRESS,PROGRESS_CMD_SETUP,nFileCount);

	//平台文件
	if(PlatformItemArray.GetCount() > 0)
	{
		//搜索文件
		for(INT_PTR i = 0; i < PlatformItemArray.GetCount(); i++)
		{
			//事件处理
			DoEvents();

			pItemInfo=PlatformItemArray[i];
			if(pItemInfo->bIsDirectory == TRUE)
			{
				//格式参数
				strPrefix.Format(TEXT("%s\\"),pItemInfo->strFileName);
				strInputPath.Format(TEXT("%s\\%s"),szInputPath,pItemInfo->strFileName);

				//搜索文件
				SearchFiles(strInputPath,strPrefix,PlatformFileArray);
			}
			else
			{
				PlatformFileArray.Add(pItemInfo->strFileName);
			}
		}

		//构造平台配置
		if(PlatformFileArray.GetCount() > 0)
		{
			//构造配置
			MakeUpdateConfig(PlatformFileArray, m_szPlazaConfigName);
		}
	}

	//游戏文件
	if(GameItemArray.GetCount() > 0)
	{
		//搜索游戏文件
		for(INT_PTR i = 0; i < GameItemArray.GetCount(); i++)
		{
			//事件处理
			DoEvents();

			//移除元素
			GameFileArray.RemoveAll();

			//获取对象
			pItemInfo=GameItemArray[i];

			//添加游戏进程
			strInputPath.Format(TEXT("%s.exe"),pItemInfo->strFileName);
			GameFileArray.Add(strInputPath);

			//格式参数
			strPrefix.Format(TEXT("%s\\"),pItemInfo->strFileName);
			strInputPath.Format(TEXT("%s\\%s"),szInputPath,pItemInfo->strFileName);

			//搜索文件
			SearchFiles(strInputPath,strPrefix,GameFileArray);

			//构造游戏配置
			if(GameFileArray.GetCount() > 0)
			{
				//构造配置
				strConfigName.Format(TEXT("%s%s"),pItemInfo->strFileName,m_szGameExpandName);
				MakeUpdateConfig(GameFileArray, strConfigName);
			}
		}
	}

	//工作目录
	TCHAR szWorkDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szWorkDirectory,CountArray(szWorkDirectory));

	//变量定义
	CString szConfigFile;
	szConfigFile.Format(TEXT("%s\\%s"),szWorkDirectory,UPDATE_CONFIG_NAME);

	//保存配置	
	SaveConfig(szConfigFile);

	//启用控件
	EnableControls(true);
}

//浏览目录
void CUpdateConfigDlg::OnBnClickedBrowseInputpath()
{
	//显示浏览
	CString strInputPath=GetBrowseFolder(m_hWnd,TEXT("选择更新文件存放目录"));
	if(strInputPath.IsEmpty() == false)	
	{
		SetDlgItemText(IDC_EDIT_INPUT_PATH,strInputPath);
	}
}

//浏览目录
void CUpdateConfigDlg::OnBnClickedBrowseOutputpath()
{
	//显示浏览
	CString strOutPutPath=GetBrowseFolder(m_hWnd,TEXT("选择更新配置输出目录"));
	if(strOutPutPath.IsEmpty() == false)	
	{
		SetDlgItemText(IDC_EDIT_OUTPUT_PATH,strOutPutPath);
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
