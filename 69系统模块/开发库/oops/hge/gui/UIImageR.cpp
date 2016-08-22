#include "stdafx.h"
#include "UIImageR.h"

CUIImageR::CUIImageR(int id, HTEXTURE tex, int rows, int cols)
: CUIComponent(id)
, m_tex(0)
, m_iFrameW(0)
, m_iFrameH(0)
, m_iFrames(0)
, m_iRows(0)
, m_iCols(0)
, m_spt(0, 0, 0, 0, 0)
, m_fAngle(0)
{
	SetTexture(tex, rows, cols);
} // CUIImageR

CUIImageR::~CUIImageR()
{
} // CUIImageR

void CUIImageR::SetTexture(HTEXTURE tex, int rows, int cols)
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

void CUIImageR::SetFrame(int index)
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

void CUIImageR::SetAngle(float fAngle)
{
	m_fAngle = fAngle;
}

void CUIImageR::SetColor(DWORD dwColor, int index)
{
	m_spt.SetColor(dwColor, index);
}

int CUIImageR::GetCurrentFrame() const
{
	return m_iCurFrame;
} // GetCurrentFrame

int CUIImageR::GetFrames() const
{
	return m_iFrames;
} // GetFrames

float CUIImageR::GetAngle() const
{
	return m_fAngle;
}

DWORD CUIImageR::GetColor() const
{
	return m_spt.GetColor();
}

void CUIImageR::OnUpdate()
{
	
} // OnUpdate

void CUIImageR::OnRender()
{
	float fDX = GetDrawX() - m_x;
	float fDY = GetDrawY() - m_y;
	float fx = fDX * cosf(m_fAngle) - fDY * sinf(m_fAngle) + m_x;
	float fy = fDX * sinf(m_fAngle) + fDY * cosf(m_fAngle) + m_y;
	m_spt.RenderEx(fx, fy, m_fAngle);
}

bool CUIImageR::IsInside(float x, float y) const
{
	float fx = x - m_x;
	float fy = y - m_y;
	x = fx * cosf(-m_fAngle) - fy * sinf(-m_fAngle) + m_x;
	y = fx * sinf(-m_fAngle) + fy * cosf(-m_fAngle) + m_y;

	x -= GetDrawX();
	y -= GetDrawY();

	if (x <= m_mX)
	{
		return false;
	}

	if (y <= m_mY)
	{
		return false;
	}

	if (x >= m_mX + m_mWidth)
	{
		return false;
	}

	if (y >= m_mY + m_mHeight)
	{
		return false;
	}

	return true;
} // IsInside
