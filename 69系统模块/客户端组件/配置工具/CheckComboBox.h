#ifndef CHECK_COMBOBOX_HEAD_FILE
#define CHECK_COMBOBOX_HEAD_FILE


//////////////////////////////////////////////////////////////////////////////////////
//复选下拉框
class CCheckComboBox : public CComboBox
{
	//变量定义
protected:
	HWND								m_hListBox;						//下拉句柄	
	BOOL								m_bTextUpdated;					//更新标识
	BOOL								m_bItemHeightSet;				//设置标识
	CString								m_strText;						//复选文本

	//函数定义
public:
	//构造函数
	CCheckComboBox();
	//析构函数
	virtual ~CCheckComboBox();

	//功能函数
public:
	//创建函数
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//选择设置
	INT SetCheck(INT nIndex, BOOL bFlag);
	//获取选择
	BOOL GetCheck(INT nIndex);
	//全选设置
	void SelectAll(BOOL bCheck = TRUE);
	//获取文件
	CString GetText() { return m_strText; }

	//重载函数
protected:
	//绘画子项
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//测量子项
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	//辅助函数
protected:
	//计算文本
	void RecalcText();

	//消息函数
protected:
	//控件颜色
	afx_msg LRESULT OnCtlColorListBox(WPARAM wParam, LPARAM lParam);
	//获取文本
	afx_msg LRESULT OnGetText(WPARAM wParam, LPARAM lParam);
	//文件长度
	afx_msg LRESULT OnGetTextLength(WPARAM wParam, LPARAM lParam);
	//下拉消息
	afx_msg void OnDropDown();

	DECLARE_MESSAGE_MAP()	
};

/////////////////////////////////////////////////////////////////////////////
#endif
