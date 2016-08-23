#ifndef FILE_LIST_VIEW_HEAD_FILE
#define FILE_LIST_VIEW_HEAD_FILE
#pragma once

///////////////////////////////////////////////////////////////////////////////////////////
//文件信息
struct tagFileInfo
{
	TCHAR							szFileName[64];						//文件名称
	TCHAR							szFileType[32];						//文件类型
	TCHAR							szFileVersion[32];					//文件版本
	BYTE							cbFileKind;							//文件类型
	BOOL							bIsDirectory;						//目录标识
	DWORD							dwFileSize;							//文件大小
	FILETIME						ftModifyDate;						//修改日期
};

//子项信息
struct tagItemInfo 
{
	CString							strFileName;						//文件名称
	CString							strFileVersion;						//文件版本
	CString							strType;							//文件类型
	BYTE							cbFileKind;							//文件类型
	BOOL							bIsDirectory;						//是否文件夹
	DWORD							nFileSize;							//文件大小	
	FILETIME						ftLastWriteTime;					//修改日期
};

//文件类型
#define FILE_KIND_GAME				0x01								//游戏文件
#define FILE_KIND_PLATFORM			0x02								//平台文件

///////////////////////////////////////////////////////////////////////////////////////////
//列表视图
class CFileListView : public CListCtrl
{
	//变量定义
protected:
	int											m_nSortedCol;						//排序列索引

	//函数定义
public: 
	//构造函数
	CFileListView();
	//析构函数
	virtual ~CFileListView();	

	//功能函数
public:
	//添加子项
	tagItemInfo * AddItem(int nIndex, tagFileInfo * pFileInfo);
	//添加新文件夹
	tagItemInfo * AddNewFolder(int nIndex, LPCTSTR lpszFolder);
	//添加新文件
	tagItemInfo * AddNewFile(int nIndex, LPCTSTR lpszFileName, DWORD dwFileSize);

	//重写函数
public:
	//消息解释
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//功能函数
public:
	//重置列表
	VOID ResetListView();
	//设置文件名
	VOID SetFileName(int nIndex, LPCTSTR lpszFileName);
	//执行排序
	VOID Sort(int nCol = -1);
	//文件夹标识
	BOOL IsDirectory(int nItem);
	//获取文件大小
	DWORD GetFileSize(int nItem);
	//获取修改时间
	FILETIME GetLastWriteTime(int nItem);
	//格式化大小
	LPCTSTR FormatSize(DWORD dwSizeLow, DWORD dwSizeHigh = 0);

	//辅助函数
protected:
	//释放子项
	VOID FreeItemMemory();
	//递归列表
	VOID RecursiveFileList(LPCTSTR lpszPath, CStringArray &strFileNameArray);
	//获取类型
	LPCTSTR GetTypeName(CString strPath);
	//初始列表
	BOOL InitListViewImageLists();	

	//静态函数
public:
	//比较函数
	static int CALLBACK CompareFunc (LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	//消息函数
protected:	
	//创建消息
	int OnCreate(LPCREATESTRUCT lpCreateStruct);	
	//获取信息
	VOID OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	//列头点击
	VOID OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	//删除子项
	VOID OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////////////////////////////
#endif
