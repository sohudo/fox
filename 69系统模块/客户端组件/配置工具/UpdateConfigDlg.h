#ifndef UPDATE_MAIN_HEAD_FILE
#define UPDATE_MAIN_HEAD_FILE
#pragma once

//引入文件
#include "FileListView.h"
#include "CheckComboBox.h"

///////////////////////////////////////////////////////////////////////////////////

// 更新配置
class CUpdateConfigDlg : public CDialog
{
	//资源变量
protected:
	HICON							m_hIcon;						//工程图标 	

	//配置变量
protected:
	TCHAR							m_szPlazaConfigName[32];		//配置名称
	TCHAR							m_szGameExpandName[32];			//扩展名称

	//控件变量
protected:	
	CFileListView					m_FileListView;					//文件列表
	CCheckComboBox					m_ComboGameList;				//游戏下拉框	

	//函数定义
public:
	//构造函数
	CUpdateConfigDlg(CWnd* pParent = NULL);	

	//继承函数
protected:	
	//初始化窗口
	virtual BOOL OnInitDialog();
	//数据交换
	virtual void DoDataExchange(CDataExchange* pDX);	
	//命令消息
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//文件操作
protected:
	//加载配置
	bool LoadConfig(LPCTSTR pszConfigFile);	
	//保存配置
	bool SaveConfig(LPCTSTR pszConfigFile);
	//统计文件
	void CensusFileCount(LPCTSTR pszFilePath,int & nFileCount);
	//筛选文件
	void FilterFiles(LPCTSTR pszFilePath,BYTE cbFileKind,LPCTSTR pszGameNames);
	//查找文件
	void SearchFiles(LPCTSTR pszFilePath,LPCTSTR pszPrefix,CStringArray & StringArray);	

	//辅助函数
protected:
	//使能设置
	VOID EnableControls(bool bEnabled);
	//文件大小
	DWORD GetFileSize(LPCTSTR pszFileName);	
	//绘制边框
	VOID DrawControlFrame(CDC * pDC,CRect & rcFrame);	
	//浏览目录
	CString GetBrowseFolder(HWND hParent,CString strTitle);
	//文件版本
	BOOL GetFileVersion(LPCTSTR pszFileName, TCHAR szFileVersion[],WORD wBufferCount);

	//功能函数
protected:
	//文件存在
	BOOL IsFileExist(LPCTSTR pszFileName);	
	//游戏进程
	BOOL IsGameProcess(LPCTSTR pszFileName);
	//目录存在
	BOOL IsDirectoryExist(LPCTSTR pszDirectory);	
	//标记类型
	VOID MarkFileKind(LPCTSTR pszWorkPath,tagFileInfo & FileInfo);
	//构造配置
	BOOL MakeUpdateConfig(CStringArray & FileNameArray,LPCTSTR pszConfigName);

	//消息函数
protected:	
	//绘画背景
	BOOL OnEraseBkgnd(CDC* pDC);

	//消息函数
public:
	//更新进度
	LRESULT OnUpdateProgress(WPARAM wParam, LPARAM lParam);

	//消息函数
public:
	//全部文件
	afx_msg void OnBnClickedRadioAll();
	//游戏文件
	afx_msg void OnBnClickedRadioGames();
	//平台文件
	afx_msg void OnBnClickedRadioPlatform();	
	//生成更新
	afx_msg void OnBnClickedButtonDone();
	//浏览目录
	afx_msg void OnBnClickedBrowseInputpath();
	//浏览目录
	afx_msg void OnBnClickedBrowseOutputpath();

	DECLARE_MESSAGE_MAP()			
};

///////////////////////////////////////////////////////////////////////////////////
#endif
