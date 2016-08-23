#include "Stdafx.h"
#include "GlobalFuncUnits.h"

////////////////////////////////////////////////////////////////////////////////////////////////

//处理消息
VOID DoEvents()
{
	MSG msg;

	while (PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE))         
	{            
		TranslateMessage(&msg);            
		DispatchMessage(&msg);         
	}      
}

//获取图标索引
int GetDiskIconIndex()
{
	static int nIconIndex=-1;

	if(nIconIndex==-1)
	{
		//获取目录
		TCHAR szCurrDirectory[MAX_PATH]=TEXT("");
		GetCurrentDirectory(MAX_PATH,szCurrDirectory);

		//变量定义
		CString strCurrDirectory(szCurrDirectory);
		int nPos = strCurrDirectory.Find(TEXT("\\"));
		strCurrDirectory = strCurrDirectory.Left(nPos+1);

		//获取索引
		nIconIndex = GetIconIndex(strCurrDirectory,TRUE,FALSE);
	}

	return nIconIndex;
}

//获取图标索引
int GetDirectoryIconIndex()
{
	static int nIconIndex=-1;

	if(nIconIndex==-1)
	{
		//获取目录
		TCHAR szCurrDirectory[MAX_PATH]=TEXT("");
		GetCurrentDirectory(MAX_PATH,szCurrDirectory);

		//获取索引
		nIconIndex = GetIconIndex(szCurrDirectory,TRUE,FALSE);
	}	

	return nIconIndex;
}

//获取图标索引
int GetIconIndex(LPCTSTR lpszPath, BOOL bIsDir, BOOL bSelected)
{
	int _nDirIconIdx = -1;
	SHFILEINFO sfi;
	memset(&sfi, 0, sizeof(sfi));

	if (bIsDir)
	{
		//if (_nDirIconIdx == -1)
		{
			SHGetFileInfo(lpszPath,FILE_ATTRIBUTE_DIRECTORY,&sfi,sizeof(sfi), 
				SHGFI_SMALLICON | SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES | (bSelected ? SHGFI_OPENICON : 0)
			);
			_nDirIconIdx = sfi.iIcon;
		}

		return _nDirIconIdx;
	}
	else
	{
		SHGetFileInfo (lpszPath,FILE_ATTRIBUTE_NORMAL,&sfi,sizeof(sfi), 
			SHGFI_SMALLICON | SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES | (bSelected ? SHGFI_OPENICON : 0)
		);
		return sfi.iIcon;
	}
	return -1;
}

//等待操作结果
BOOL WaitOperateResult(HANDLE hEventHandle,BOOL * pOperateResult,int nTimeout)
{
	//变量定义
	if(pOperateResult==NULL) return FALSE;

	//设置事件
	*pOperateResult=FALSE;
	ResetEvent(hEventHandle);

	//变量定义
	DWORD dwRet;
    DWORD dwMaxTick = GetTickCount() + nTimeout;

	while(TRUE)
	{
		dwRet = MsgWaitForMultipleObjects(1, &hEventHandle, FALSE, dwMaxTick - GetTickCount(), QS_ALLINPUT);
		if (dwRet == WAIT_OBJECT_0)
		{			
			return (BOOL)*pOperateResult;      
		}   
		else
		if (dwRet == WAIT_OBJECT_0 + 1)
		{			
			DoEvents();
		}  
		else
		{
			return FALSE;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////