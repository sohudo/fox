#ifndef GAME_FRAME_VIEW_HGE_HEAD_FILE
#define GAME_FRAME_VIEW_HGE_HEAD_FILE
#include "GfxFont.h"
//GAME_FRAME_CLASS
//游戏视图框架
class CGameFrameViewHGE : public CGameFrameView
{
	//函数定义
public:
	//构造函数
	CGameFrameViewHGE();
	//析构函数
	virtual ~CGameFrameViewHGE();

	//重载函数
public:
	//重置界面
	virtual void ResetGameView();
	//调整控件
	virtual void RectifyControl(int nWidth, int nHeight);
	//界面更新
	virtual void InvalidGameView(int nXPos, int nYPos, int nWidth, int nHeight);
	//桌面消息
	virtual void AddGameTableMessage(LPCTSTR pszMessage,WORD wLen,WORD wType);
	//设置背景色
	virtual void SetBackgroundColor(DWORD argb);

	//游戏元素
public:
	//绘画准备
	void DrawUserReady(int x, int y, int align = 0);
	//绘画时间
	void DrawUserClock(int x, int y, WORD wUserClock, int align = 0);
	//绘画标志
	void DrawOrderFlag(int x, int y, BYTE cbImageIndex, int align = 0);
	//绘画头像
	void DrawUserAvatar(int x, int y, IClientUserItem * pIClientUserItem, int align = 0,float fzoom=1.0f);
	//绘画头像
	void DrawAvatar(unsigned int faceId, int x, int y, bool isOnLine, unsigned int align = 0,float fzoom=1.0f);
	//绘画头像
	void DrawAvatar(const DWORD (&dwCustomFace)[FACE_CX*FACE_CY], int x, int y, bool isOnLine, unsigned int align = 0,float fzoom=1.0f);

	//比赛信息
	VOID DrawMatchInfo();

	void DrawNumberString(int nXPosition, int nYPosition,HTEXTURE btxNumber,int iNumber);

	//消息映射
public:
	//创建消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//销毁消息
	afx_msg void OnDestroy();
	//绘画函数
	afx_msg void OnPaint();
	//绘画函数
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//位置变化
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	
	DECLARE_MESSAGE_MAP()

	// 功能函数
public:
	static HTEXTURE GetDllTexture(HGE* hge, LPCTSTR type, LPCTSTR name, LPCTSTR dll);
	static HTEXTURE GetCustomTexture(DWORD* pChustomData, HGE* hge, int cx, int cy);

	// 子类重载
protected:
	// 初始化
	virtual bool OnInit() {return true;}
	// 结束释放
	virtual void OnDeinit() {}
	// 恢复
	virtual void OnRestore() {}
	// 更新
	virtual void OnUpdate() {}
	// 绘画界面
	virtual void OnDrawView(){}

	// HGE 
private:
	// 初始化HGE
	bool InitHge(HWND hWnd);
	// 关闭Hge
	void ShutDownHge();
	// 加载资源
	bool LoadResources();
	// 静态处理接口
	static bool FrameFunc();
	// 静态绘制接口
	static bool RenderFunc();
	// 静态恢复接口
	static bool RestoreFunc();

	// HGE 引擎
protected:
	HGE*		m_pHge;					// HGE指针

	//界面资源
public:
	CRect		m_RectDirtySurface;		// 重画区域
	DWORD		m_dwClearColor;			// 清屏颜色

	hgeSprite	m_spt;					// 绘制精灵

	// 头像
	unsigned int	m_iAvatarRows;		// 行数
	unsigned int	m_iAvatarCols;		// 列数
	unsigned int	m_iAvatarTotals;	// 总个数
	HTEXTURE		m_hTextureAvatar;	// 头像纹理
	HTEXTURE		m_hCustomAvatar;	// 自定义纹理

	//比赛
	HTEXTURE		m_MatchView;		//比赛信息背景图片
	HTEXTURE		m_WaitView;			//等待配桌图片
	HTEXTURE		m_GreenNumber;	
	HTEXTURE		m_OrangeNumber;
	HTEXTURE		m_WaitTip;
	HTEXTURE		m_Line;	
	


	// 玩家标志		
	unsigned int	m_iMemberRows;		// 行数
	unsigned int	m_iMemberCols;		// 列数
	unsigned int	m_iMemberTotals;	// 总个数
	HTEXTURE		m_texMemberFlag;	// 标志纹理

	// 其他标志
	HTEXTURE     m_texReady;		// 准备纹理
	HTEXTURE     m_texClockBack;	// 倒计时背景
	HTEXTURE     m_texClockItem;	// 倒计时数字

public:
	GfxFont			*m_MatchFont;		//
	GfxFont			*m_MatchFont2;
	GfxFont			*m_MatchRoll;		//滚屏字体
	CPoint			m_ptRoll;			//滚动文字位置
};

//////////////////////////////////////////////////////////////////////////

#endif