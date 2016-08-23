#ifndef _UIImageR_H_
#define _UIImageR_H_

#include "UIComponent.h"

class CUIImageR
	: public CUIComponent
{
public:
	CUIImageR(int id = 0, HTEXTURE tex = 0, int rows = 1, int cols = 1);
	~CUIImageR();

	void SetTexture(HTEXTURE tex, int rows = 1, int cols = 1);
	void SetFrame(int index);
	void SetAngle(float fAngle);
	void SetColor(DWORD dwColor, int index = -1);

	int GetCurrentFrame() const;
	int GetFrames() const;
	float GetAngle() const;
	DWORD GetColor() const;
	
protected:	
	virtual void OnRender();
	virtual void OnUpdate();
	virtual bool IsInside(float x, float y) const;

protected:
	CUIImageR(const CUIImageR &go);
	CUIImageR&	operator= (const CUIImageR &go);
protected:
	HTEXTURE	m_tex;			// 纹理
	int			m_iFrameW;		// 帧宽度
	int			m_iFrameH;		// 帧高度
	int			m_iRows;		// 行数
	int			m_iCols;		// 列数
	int			m_iFrames;		// 帧数
	int			m_iCurFrame;	// 当前帧
	hgeSprite	m_spt;			// 绘制精灵
	float		m_fAngle;
}; // CUIImageR
#endif // _UIImageR_H_