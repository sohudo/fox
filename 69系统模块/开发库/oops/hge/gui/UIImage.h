#ifndef _UIImage_H_
#define _UIImage_H_

#include "UIComponent.h"

class CUIImage
	: public CUIComponent
{
public:
	CUIImage(int id = 0, HTEXTURE tex = 0, int rows = 1, int cols = 1);
	~CUIImage();

	void SetTexture(HTEXTURE tex, int rows = 1, int cols = 1);
	void SetFrame(int index);

	int GetCurrentFrame() const;
	int GetFrames() const;
	
protected:	
	virtual void OnRender();
	virtual void OnUpdate();

protected:
	CUIImage(const CUIImage &go);
	CUIImage&	operator= (const CUIImage &go);
protected:
	HTEXTURE	m_tex;			// 纹理
	int			m_iFrameW;		// 帧宽度
	int			m_iFrameH;		// 帧高度
	int			m_iRows;		// 行数
	int			m_iCols;		// 列数
	int			m_iFrames;		// 帧数
	int			m_iCurFrame;	// 当前帧
	hgeSprite	m_spt;			// 绘制精灵
}; // CUIImage
#endif // _UIImage_H_