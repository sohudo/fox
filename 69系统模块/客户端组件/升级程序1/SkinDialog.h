#ifndef SKIN_DIALOG_HEAD_FILE
#define SKIN_DIALOG_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

//窗口资源
class CSkinDialogAttribute : public CSkinResourceManager
{
	friend class CSkinDialog;

	//控制信息
public:
	bool							m_bShowIcon;						//显示图标
	bool							m_bShowTitle;						//显示标题
	HICON							m_hDefaultIcon;						//默认图标

	//偏移信息
public:
	INT								m_nXIconExcursion;					//图标偏移
	INT								m_nYIconExcursion;					//图标偏移
	INT								m_nXButtonExcursion;				//按钮偏移
	INT								m_nYButtonExcursion;				//按钮偏移
	INT								m_nXTitleTextExcursion;				//标题偏移
	INT								m_nYTitleTextExcursion;				//标题偏移

	//配色变量
public:
	COLORREF						m_crTitleText;						//标题颜色
	COLORREF						m_crBackGround;						//背景颜色
	COLORREF						m_crControlText;					//控制颜色

	//辅助变量
public:
	INT								m_nButtonWidth;						//按钮宽度
	INT								m_nButtonHeigth;					//按钮高度

	//边框变量
public:
	INT								m_nXBorder;							//边框大小
	INT								m_nYBorder;							//边框大小
	INT								m_nCaptionHeigth;					//标题高度

	//背景画刷
public:
	CBrush							m_brBackGround;						//背景画刷

	//标题资源
public:
	CBitImage						m_SkinImageTL;						//标题位图
	CBitImage						m_SkinImageTM;						//标题位图
	CBitImage						m_SkinImageTR;						//标题位图
	CBitImage						m_SkinImageMR;						//标题位图
	CBitImage						m_SkinImageML;						//标题位图
	CBitImage						m_SkinImageBL;						//标题位图
	CBitImage						m_SkinImageBM;						//标题位图
	CBitImage						m_SkinImageBR;						//标题位图

	//按钮资源
public:
	CBitImage						m_ButtonMin;						//按钮位图
	CBitImage						m_ButtonMax;						//按钮位图
	CBitImage						m_ButtonRes;						//按钮位图
	CBitImage						m_ButtonClose;						//按钮位图

	//函数定义
protected:
	//构造函数
	CSkinDialogAttribute();
	//析构函数
	virtual ~CSkinDialogAttribute();

	//功能函数
public:
	//更新资源
	//bool UpdateSkinResource(ISkinResource * pISkinResource);
};

//////////////////////////////////////////////////////////////////////////

//对话框类
class CSkinDialog : public CDialog
{
	//窗口状态
private:
	bool							m_bActive;							//激活标志
	bool							m_bMaxShow;							//最大标志

	//标题图片
public:
	CPngImage						m_ImageTitle;						//标题图片

	//按钮状态
private:
	BYTE							m_cbNowHotButton;					//现在焦点
	BYTE							m_cbNowDownButton;					//按下按钮
	BYTE							m_cbButtonState[3];					//按钮状态

	//位置变量
private:
	CRect							m_rcButton[3];						//按钮位置
	CRect							m_rcNormalSize;						//正常位置

	//资源变量
public:
	static CSkinDialogAttribute		m_SkinAttribute;					//资源变量

	//函数定义
public:
	//构造函数
	CSkinDialog(UINT nIDTemplate, CWnd * pParentWnd=NULL);
	//析构函数
	virtual ~CSkinDialog();

	//重载函数
public:
	//初始化消息
	virtual BOOL OnInitDialog();
	//绘画消息
	virtual VOID OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeigth){}

	//控制函数
public:
	//最大窗口
	VOID MaxSizeWindow();
	//还原窗口
	VOID RestoreWindow();
	//按钮控制
	VOID EnableButton(UINT uButtonStyle);

	//信息函数
protected:
	//获取边框
	INT GetXBorder() { return m_SkinAttribute.m_nXBorder; }
	//获取边框
	INT GetYBorder() { return m_SkinAttribute.m_nYBorder; }
	//获取标题
	INT GetCaptionHeight() { return m_SkinAttribute.m_nCaptionHeigth; }

	//绘画函数
protected:
	//绘画边框
	VOID DrawBorder(CDC * pDC);
	//绘画标题
	VOID DrawCaption(CDC * pDC);
	//绘画背景
	VOID DrawBackGround(CDC * pDC);

	//辅助函数
private:
	//更新标题
	VOID UpdateCaption();
	//调整按钮
	VOID AdjustTitleButton(INT nWidth, INT nHeight);

	//消息映射
protected:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//按键测试
	UINT OnNcHitTest(CPoint Point);
	//鼠标消息
	VOID OnMouseMove(UINT nFlags, CPoint Point);
	//鼠标消息
	VOID OnLButtonUp(UINT nFlags, CPoint Point);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//鼠标消息
	VOID OnLButtonDblClk(UINT nFlags, CPoint Point);
	//位置改变
	VOID OnWindowPosChanged(WINDOWPOS * lpwndpos);
	//激活消息
	VOID OnActivateApp(BOOL bActive, DWORD dwThreadID);
	//激活消息
	VOID OnActivate(UINT nState, CWnd * pWndOther, BOOL bMinimized);
	//改变消息
	VOID OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	//控件颜色
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);
	//标题消息
	LRESULT	OnSetTextMesage(WPARAM wParam, LPARAM lParam);
	//获取大小
	VOID OnGetMinMaxInfo(MINMAXINFO* lpMMI);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif