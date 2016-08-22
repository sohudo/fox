#include "stdafx.h"
#include "UIImage.h"

CUIImage::CUIImage(int id, HTEXTURE tex, int rows, int cols)
: CUIComponent(id)
, m_tex(0)
, m_iFrameW(0)
, m_iFrameH(0)
, m_iFrames(0)
, m_iRows(0)
, m_iCols(0)
, m_spt(0, 0, 0, 0, 0)
{
	SetTexture(tex, rows, cols);
} // CUIImage

CUIImage::~CUIImage()
{
} // CUIImage

void CUIImage::SetTexture(HTEXTURE tex, int rows, int cols)
{
	m_tex        = tex;
	m_iRows		 = rows;
	m_iCols		 = cols;
	m_iFrames	 = m_iRows * m_iCols;

	if (m_tex != 0 && m_iFrames > 0)
	{
		int w     = m_hge->Texture_GetWidth(m_tex, true);
		int h     = m_hge->Texture_GetHeight(m_tex, true);
		m_iFrameW = w / m_iCols;
		m_iFrameH = h / m_iRows;
	}
	else
	{
		m_iFrameW = 0;
		m_iFrameH = 0;
		m_iRows	  = 0;
		m_iCols   = 0;
		m_iFrames = 0;
	}

	m_spt.SetTexture(m_tex);
	SetSize((float)m_iFrameW, (float)m_iFrameH);
	SetMouseRect(0, 0, (float)m_iFrameW, (float)m_iFrameH);

	SetFrame(0);
} // SetTexture

void CUIImage::SetFrame(int index)
{
	if (index < 0 || index >= m_iFrames)
	{
		m_iCurFrame = 0;
		m_spt.SetTextureRect(0, 0, 0, 0);
	}
	else
	{
		m_iCurFrame = index;
		int r = index / m_iCols;
		int c = index % m_iCols;
		m_spt.SetTextureRect((float)c * m_iFrameW, (float)r * m_iFrameH, (float)m_iFrameW, (float)m_iFrameH);
	}
} // SetFrame

int CUIImage::GetCurrentFrame() const
{
	return m_iCurFrame;
} // GetCurrentFrame

int CUIImage::GetFrames() const
{
	return m_iFrames;
} // GetFrames

void CUIImage::OnUpdate()
{
	
} // OnUpdate

void CUIImage::OnRender()
{
	m_spt.SetColor(ARGB(255, 255, 255, 255));
	m_spt.Render(GetDrawX(), GetDrawY());
}