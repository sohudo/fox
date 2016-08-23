#include "stdafx.h"
#include "Resource.h"
#include "FileListView.h"
#include "GlobalFuncUnits.h"

////////////////////////////////////////////////////////////////////////////////////////////////
//消息映射
BEGIN_MESSAGE_MAP(CFileListView, CListCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteitem)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////////////////////

//全局定义
BOOL g_bSortAscending;

//列索引定义
#define COLUMN_FILENAME						0						//文件名称
#define COLUMN_FILESIZE						1						//文件大小
#define COLUMN_FILETYPE						2						//文件类型
#define COLUMN_FILECREATED					3						//创建时间
#define COLUMN_FILEVERSION					4						//文件版本

/////////////////////////////////////////////////////////////////////////////
//构造函数
CFileListView::CFileListView()
{
	//设置变量
	m_nSortedCol = 0; 
    g_bSortAscending = TRUE; 
}

//析构函数
CFileListView::~CFileListView()
{
}

//创建消息
int CFileListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct) == -1) 	return -1;
	
	//编辑风格
	ModifyStyle(0, LVS_REPORT|LVS_SHAREIMAGELISTS);

    //初始化列表
    InsertColumn(COLUMN_FILENAME,TEXT("名称"), LVCFMT_LEFT, 160);
    InsertColumn(COLUMN_FILESIZE,TEXT("大小"), LVCFMT_RIGHT, 85);
    InsertColumn(COLUMN_FILETYPE,TEXT("类型"), LVCFMT_LEFT, 100);
	InsertColumn(COLUMN_FILECREATED,TEXT("创建日期"), LVCFMT_LEFT, 100);
	InsertColumn(COLUMN_FILEVERSION,TEXT("文件版本"), LVCFMT_LEFT, 100);

	InitListViewImageLists();

	return 0;
}

//添加子项
tagItemInfo * CFileListView::AddItem(int nIndex, tagFileInfo * pFileInfo)
{
    tagItemInfo * pItem;

    try 
	{
        pItem = new tagItemInfo;
    }
    catch(CMemoryException* e) 
	{
        e->Delete();
        return NULL;
    }

	//设置子项
	pItem->strFileName = pFileInfo->szFileName;
	pItem->strFileVersion = pFileInfo->szFileVersion;
	pItem->nFileSize = pFileInfo->dwFileSize;
	pItem->cbFileKind = pFileInfo->cbFileKind;
	pItem->ftLastWriteTime=pFileInfo->ftModifyDate;
	pItem->bIsDirectory = pFileInfo->bIsDirectory;
	pItem->strType = pItem->bIsDirectory ? TEXT("Folder"):GetTypeName(pFileInfo->szFileName);
	int nImageIndex = GetIconIndex( pFileInfo->szFileName,pItem->bIsDirectory);
	
	//添加子项
    LV_ITEM lvi;
    lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM; 
    lvi.iItem = nIndex; 
    lvi.iSubItem = 0; 
    lvi.iImage = nImageIndex;
    lvi.pszText = LPSTR_TEXTCALLBACK; 
    lvi.lParam = (LPARAM) pItem;

    InsertItem(&lvi);

	return pItem;
}

//添加新文件夹
tagItemInfo * CFileListView::AddNewFolder(int nIndex, LPCTSTR lpszFolder)
{
    tagItemInfo * pItem;
    try 
	{
        pItem = new tagItemInfo;
    }
    catch(CMemoryException* e) 
	{
        e->Delete();
        return NULL;
    }

    pItem->strFileName = lpszFolder;
    pItem->nFileSize = 0;
	
	SYSTEMTIME systemTime;
	GetSystemTime(&systemTime);
	::SystemTimeToFileTime(&systemTime, &pItem->ftLastWriteTime);

	pItem->bIsDirectory = TRUE;
	pItem->strType = TEXT("Folder");
	int nImage = GetIconIndex(pItem->strFileName, pItem->bIsDirectory);
	
    LV_ITEM lvi;
    lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM; 
    lvi.iItem = nIndex; 
    lvi.iSubItem = 0; 
    lvi.iImage = nImage;
    lvi.pszText = LPSTR_TEXTCALLBACK; 
    lvi.lParam = (LPARAM) pItem;

    InsertItem(&lvi);

	return pItem;
}

//添加新文件
tagItemInfo * CFileListView::AddNewFile(int nIndex, LPCTSTR lpszFileName, DWORD dwFileSize)
{
    int nCount = GetItemCount();
    for(int i=0; i<nCount; i++)
	{
		if (GetItemText(i, 0).CompareNoCase(lpszFileName) == 0)
		{
			return NULL;
		}
	}

    tagItemInfo* pItem;
    try 
	{
        pItem = new tagItemInfo;
    }
    catch(CMemoryException* e) 
	{
        e->Delete();
        return NULL;
    }

    pItem->strFileName = lpszFileName;
    pItem->nFileSize = dwFileSize;
	
	SYSTEMTIME systemTime;
	GetSystemTime(&systemTime);
	::SystemTimeToFileTime(&systemTime, &pItem->ftLastWriteTime);

	pItem->bIsDirectory = FALSE;
	pItem->strType = GetTypeName(pItem->strFileName);;
	int nImage = GetIconIndex(pItem->strFileName, pItem->bIsDirectory);
	
    LV_ITEM lvi;
    lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM; 
    lvi.iItem = nIndex; 
    lvi.iSubItem = 0; 
    lvi.iImage = nImage;
    lvi.pszText = LPSTR_TEXTCALLBACK; 
    lvi.lParam = (LPARAM) pItem;

    InsertItem(&lvi);

	return pItem;
}

//释放子项
VOID CFileListView::FreeItemMemory()
{
    int nCount = GetItemCount();
    if(nCount) 
	{
        for(int i=0; i<nCount; i++)
            delete(tagItemInfo*) GetItemData(i);
    }
}

//获取信息
VOID CFileListView::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
    CString string;
	LV_DISPINFO* pDispInfo =(LV_DISPINFO*) pNMHDR;

    if(pDispInfo->item.mask & LVIF_TEXT) 
	{
		tagItemInfo* pItem =(tagItemInfo*) pDispInfo->item.lParam;

        switch(pDispInfo->item.iSubItem) 
		{
        case COLUMN_FILENAME:
			{
				::lstrcpy(pDispInfo->item.pszText, pItem->strFileName);
				break;
			}
        case COLUMN_FILESIZE:	
			{
				if (pItem->bIsDirectory)
				{
					::lstrcpy(pDispInfo->item.pszText, TEXT(""));
				}
				else
				{
					::lstrcpy(pDispInfo->item.pszText, FormatSize(pItem->nFileSize));
				}
				break;
			}
        case COLUMN_FILETYPE:
			{
				::lstrcpy(pDispInfo->item.pszText, pItem->strType);
				break;
			}
        case COLUMN_FILECREATED:
			{
				CTime time(pItem->ftLastWriteTime);

				BOOL pm = FALSE;
				int nHour = time.GetHour();
				if(nHour == 0)
					nHour = 12;
				else 
				if(nHour == 12)
					pm = TRUE;
				else 
				if(nHour > 12) 
				{
					nHour -= 12;
					pm = TRUE;
				}

				string.Format(_T("%d-%.2d-%0.2d %d:%0.2d"),time.GetYear(),time.GetMonth(),time.GetDay(),
					nHour,time.GetMinute());
				::lstrcpy(pDispInfo->item.pszText, string);
				break;
			}
		 case COLUMN_FILEVERSION:
			{
				::lstrcpy(pDispInfo->item.pszText, pItem->strFileVersion);
				break;
			}
        }
    }
	*pResult = 0;
}

//列头点击
VOID CFileListView::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView =(NM_LISTVIEW*) pNMHDR;

    if(pNMListView->iSubItem == m_nSortedCol)
		g_bSortAscending = !g_bSortAscending;
	else
		g_bSortAscending = TRUE;

	m_nSortedCol = pNMListView->iSubItem;
    
	SortItems(CompareFunc, pNMListView->iSubItem);
	*pResult = 0;
}

//比较函数
int CALLBACK CFileListView::CompareFunc(LPARAM lParam1, LPARAM lParam2,LPARAM lParamSort)
{
    tagItemInfo* pItem1 =(tagItemInfo*) lParam1;
    tagItemInfo* pItem2 =(tagItemInfo*) lParam2;
    int nResult;

    switch(lParamSort) 
	{
    case COLUMN_FILENAME:
		{
			if (pItem1->bIsDirectory && !pItem2->bIsDirectory)
				return (g_bSortAscending ? -1 : 1);

			if (!pItem1->bIsDirectory && pItem2->bIsDirectory)
				return (g_bSortAscending ? 1 : -1);

			nResult = pItem1->strFileName.CompareNoCase(pItem2->strFileName);
			break;
		}
    case COLUMN_FILESIZE: 
		{
			if (pItem1->bIsDirectory && !pItem2->bIsDirectory)
				return (g_bSortAscending ? -1 : 1);
			if (!pItem1->bIsDirectory && pItem2->bIsDirectory)
				return (g_bSortAscending ? 1 : -1);

			nResult = pItem1->nFileSize - pItem2->nFileSize;
			break;
		}
    case COLUMN_FILETYPE: 
		{
			if (pItem1->bIsDirectory && !pItem2->bIsDirectory)
				return -1;
			if (!pItem1->bIsDirectory && pItem2->bIsDirectory)
				return 1;

			nResult = pItem1->strType.CompareNoCase(pItem2->strType);
			break;
		}
    case COLUMN_FILECREATED:
		{
			nResult = ::CompareFileTime(&pItem1->ftLastWriteTime, &pItem2->ftLastWriteTime);
			break;
		}
	 case COLUMN_FILEVERSION:
		{			
			if (pItem1->bIsDirectory && !pItem2->bIsDirectory)
				return -1;
			if (!pItem1->bIsDirectory && pItem2->bIsDirectory)
				return 1;

			nResult = pItem1->strFileVersion.CompareNoCase(pItem2->strFileVersion);
			break;
		}
    }
    return (g_bSortAscending ? nResult : -nResult);
}

//获取类型名
LPCTSTR CFileListView::GetTypeName(CString strPath)
{
	SHFILEINFO sfi;
	memset(&sfi, 0, sizeof(sfi));

	static TCHAR lpBuff[MAX_PATH];
	lpBuff[0] = 0;

	SHGetFileInfo(strPath, 0, &sfi, sizeof(sfi), SHGFI_TYPENAME);

	lstrcpy(lpBuff, sfi.szTypeName);
	if (lpBuff[0] == 0)
	{
		int nDotIdx = strPath.ReverseFind('.');
		int nBSIdx = strPath.ReverseFind('\\');
		if (nDotIdx > nBSIdx)
		{
			strPath = strPath.Mid(nDotIdx+1);
			strPath.MakeUpper();
			lstrcpy (lpBuff, strPath + ' ');
		}

		lstrcat(lpBuff, TEXT("File"));
	}
	return lpBuff;
}

//格式化大小
LPCTSTR CFileListView::FormatSize(DWORD dwSizeLow, DWORD dwSizeHigh)
{
	//变量定义
	static TCHAR szBuff[100];

	unsigned __int64 nFileSize = ((unsigned __int64)(((DWORD)(dwSizeLow)) | 
								 ((unsigned __int64)((DWORD)(dwSizeHigh))) << 32));
	unsigned __int64 kb = 1;

	if (nFileSize > 1024)
	{
		kb = nFileSize / 1024;
		if (nFileSize % 1024)
			kb++;
	}

	//数字转换
	_ui64tot(kb, szBuff, 10);

	int nLength = lstrlen(szBuff);
	if (nLength > 3)
	{
		LPCTSTR ptr = szBuff;
		ptr += (nLength-1);

		TCHAR szTemp[100];

		LPTSTR ptrTemp = szTemp;
		for(int i=0; i<nLength; i++)
		{
			if (i && ((i % 3) == 0)) 
			{
				if (*ptrTemp != ',')
				{
					*ptrTemp = ',';
					ptrTemp = _tcsinc(ptrTemp);
				}
			}
			*ptrTemp = *ptr;
			ptrTemp = _tcsinc(ptrTemp);
			ptr = _tcsdec(szBuff, ptr);
		}
		
		//结束字符
		*ptrTemp = '\0';
		_tcsrev(szTemp);
		lstrcpy(szBuff, szTemp);
	}

	lstrcat(szBuff,TEXT(" KB"));

	return szBuff;
}

//文件夹标识
BOOL CFileListView::IsDirectory(int nItem)
{
	return ((tagItemInfo*)GetItemData(nItem))->bIsDirectory;
}

//获取文件大小
DWORD CFileListView::GetFileSize(int nItem)
{
	return ((tagItemInfo*)GetItemData(nItem))->nFileSize;
}

//获取修改时间
FILETIME CFileListView::GetLastWriteTime(int nItem)
{
	return ((tagItemInfo*)GetItemData(nItem))->ftLastWriteTime;
}

//删除子项
VOID CFileListView::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	delete(tagItemInfo*) GetItemData(pNMListView->iItem);
	
	*pResult = 0;
}

//列表排序
VOID CFileListView::Sort(int nCol)
{
	SortItems(CompareFunc, (nCol != -1) ? nCol : m_nSortedCol);
}

//初始视图
BOOL CFileListView::InitListViewImageLists()
{
	HIMAGELIST himlSmall;
	HIMAGELIST himlLarge;
	SHFILEINFO sfi;

	himlSmall = (HIMAGELIST) SHGetFileInfo ( TEXT("C:\\"), 
		0, &sfi, sizeof (SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

	himlLarge = (HIMAGELIST) SHGetFileInfo ( TEXT("C:\\"), 
		0, &sfi, sizeof (SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);

	if (himlSmall && himlLarge)
	{
		::SendMessage(m_hWnd, LVM_SETIMAGELIST, (WPARAM)LVSIL_SMALL, (LPARAM)himlSmall);
		::SendMessage(m_hWnd, LVM_SETIMAGELIST, (WPARAM)LVSIL_NORMAL, (LPARAM)himlLarge);
		return TRUE;
	}

	return FALSE;
} 

//设置文件名
VOID CFileListView::SetFileName(int nIndex, LPCTSTR lpszFileName)
{
	((tagItemInfo*)GetItemData(nIndex))->strFileName = lpszFileName;
	SetItemText(nIndex, 0, lpszFileName);
}


//消息解释
BOOL CFileListView::PreTranslateMessage(MSG* pMsg) 
{
	return __super::PreTranslateMessage(pMsg);
}

//重置列表
VOID CFileListView::ResetListView()
{
	//删除子项
	DeleteAllItems();
}

//递归文件列表
VOID CFileListView::RecursiveFileList(LPCTSTR lpszPath, CStringArray &strFileNameArray)
{
	CFileFind fileFind;
	CStringArray strDirectoryArray;

	CString strFileName;
	strFileName.Format(TEXT("|%s"), lpszPath);
	strFileNameArray.Add(strFileName);
	
	BOOL bContinue = fileFind.FindFile(CString(lpszPath) + TEXT("\\*.*"));
	if (!bContinue)
	{
		fileFind.Close();
		return;
	}

	while (bContinue)
	{
		bContinue = fileFind.FindNextFile();

		if (fileFind.IsDots())
			continue;

		if (fileFind.IsDirectory())
		{
			strDirectoryArray.Add(fileFind.GetFileName());
		}
		else
		{
			strFileName.Format(TEXT("%s|%s"), fileFind.GetFileName(), lpszPath);
			strFileNameArray.Add(strFileName);
		}
	}
	fileFind.Close();

	for (int i = 0; i < strDirectoryArray.GetSize(); i++)
	{
		RecursiveFileList(CString(lpszPath) + TEXT("\\") + strDirectoryArray.GetAt(i), strFileNameArray);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
