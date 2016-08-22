#ifndef _UIButton_H_
#define _UIButton_H_

#include "UIComponent.h"

class CUIButton
	: public CUIComponent
{
public:
	CUIButton(int id = 0);
	~CUIButton();

	void SetTexture(HTEXTURE tex, int status);
	void SetAlign(unsigned int align);
	void SetFont(GfxFont* font);
	void SetLabel(const tstring& label);
	void SetLabelAlign(unsigned int align);
	void SetLabelOffset(float x, float y);
	void SetLabelColor(DWORD color);
protected:	
	void SetStateTexure(int status);
	virtual void OnRender();
	virtual void OnUpdate();
	virtual void OnMouseOver(bool bOver, float x, float y);
	
	void Enter();
	void Leave();
	void RenderText();
protected:
	CUIButton(const CUIButton &go);
	CUIButton&	operator= (const CUIButton &go);
protected:
	// 按钮动画
	float      m_fAniElapsed;	// 动画
	bool       m_isEntering;	// 是否正在进入
	bool       m_isLeaving;		// 是否正在退出

	// 按钮纹理
	int        m_iTexStatus;	// 纹理状态
	HTEXTURE   m_tex;			// 纹理
	int		   m_iFrameW;		// 帧宽度
	int		   m_iFrameH;		// 帧高度
	hgeSprite  m_spt;			// 绘制精灵

	// 按钮问
	unsigned int m_align;		// 内部文本对齐方式
	GfxFont*     m_font;		// 文本字体
	tstring      m_label;		// 显示文本
	unsigned int m_txtAlign;	// 文本对齐方式
	float        m_txtOffsetX;	// 文本x坐标偏移
	float        m_txtOffsetY;	// 文本y坐标偏移
	DWORD        m_color;		// 文本颜色
}; // CUIButton
#endif // _UIButton_H_