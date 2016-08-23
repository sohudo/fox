#include "stdafx.h"
#include "UIButton5.h"

CUIButton5::CUIButton5(int id)
: CUIComponent(id)
, m_fAniElapsed(1.0f), m_isEntering(false), m_isLeaving(false)
, m_tex(0), m_iTexStatus(0), m_spt(0, 0, 0, 0, 0), m_iFrameW(0), m_iFrameH(0)
, m_align(0), m_font(0), m_txtAlign(0), m_txtOffsetX(0), m_txtOffsetY(0)
, m_colText(0xffffffff), m_colFrame(0xff000000)
{
} // CUIButton5

CUIButton5::~CUIButton5()
{
} // CUIButton5

void CUIButton5::SetTexture(HTEXTURE tex, int status)
{
	m_tex        = tex;
	m_iTexStatus = status;

	if (m_tex != 0 && m_iTexStatus != 0)
	{
		int w     = m_hge->Texture_GetWidth(m_tex, true);
		int h     = m_hge->Texture_GetHeight(m_tex, true);
		m_iFrameW = w / m_iTexStatus;
		m_iFrameH = h;
	}
	else
	{
		m_iFrameW = 0;
		m_iFrameH = 0;
	}

	m_spt.SetTexture(m_tex);
	SetSize((float)m_iFrameW, (float)m_iFrameH);
	SetMouseRect(0, 0, (float)m_iFrameW, (float)m_iFrameH);
} // SetTexture



void CUIButton5::SetAlign(unsigned int align)
{
	m_align = align;
} // SetAlign

void CUIButton5::SetFont(GfxFont* font)
{
	m_font = font;
} // SetFont

void CUIButton5::SetLabel(const tstring& label)
{
	m_label = label;
} // SetLabel

void CUIButton5::SetLabelAlign(unsigned int align)
{
	m_txtAlign = align;
} // SetLabelAlign

void CUIButton5::SetLabelOffset(float x, float y)
{
	m_txtOffsetX = x;
	m_txtOffsetY = y;
} // SetTextOffset

void CUIButton5::SetLabelColor(DWORD colText, DWORD colFrame)
{
	m_colText  = colText;
	m_colFrame = colFrame;
} // SetLabelColor

void CUIButton5::OnRender()
{
	float drawX = GetDrawX();
	float drawY = GetDrawY();

	if (!m_bEnable)
	{
		SetStateTexure(5);
		m_spt.Render(drawX, drawY);
		RenderText();
		return;
	}

	int beforeState = 1;
	int nowState    = 1;

	if (m_isEntering)
	{
		beforeState  = 4;

	}
	else if (m_isLeaving)
	{
		beforeState  = m_isDown ? 2 : 3;
	}

	if (m_isOver)
	{
		if (m_isDown || m_iTexStatus <= 2)
		{
			nowState = 2;
		}
		else
		{
			nowState = 4;
		}
	}

	if (beforeState != nowState)
	{

		int InAlpha  = (int)(m_fAniElapsed * 255);
		int OutAlpha = 255 - InAlpha;
		SetStateTexure(beforeState);
		m_spt.SetColor(ARGB(OutAlpha, 255, 255, 255));
		m_spt.Render(drawX, drawY);

		m_spt.SetColor(ARGB(InAlpha, 255, 255, 255));
	}

	SetStateTexure(nowState);
	m_spt.Render(drawX, drawY);

	RenderText();
} // OnRender

void CUIButton5::OnUpdate()
{
	if (m_isEntering || m_isLeaving)
	{
		float dt       = m_hge->Timer_GetDelta();
		m_fAniElapsed += dt + dt + dt;

		if (m_fAniElapsed >= 1.0f)
		{
			m_fAniElapsed = 1.0f;
			m_isLeaving   = false;
			m_isEntering  = false;
		}
	}
} // OnUpdate

void CUIButton5::SetStateTexure(int status)
{
	int index = max(0, min(status - 1, m_iTexStatus - 1));
	m_spt.SetTextureRect((float)index * m_iFrameW, 0.0f, (float)m_iFrameW, (float)m_iFrameH);
} // SetStateTexure

void CUIButton5::OnMouseOver(bool bOver, float x, float y)
{
	m_isOver = bOver;

	if (m_isOver)
	{
		Enter();
	}
	else
	{
		Leave();
	}
} // OnMouseOver

void CUIButton5::Enter()
{
	if (!m_bEnable)
	{
		m_isEntering  = false;
		m_isLeaving   = false;
		return;
	}
	m_isEntering  = true;
	m_isLeaving   = false;
	m_fAniElapsed = 1 - m_fAniElapsed;

	if (m_fAniElapsed < 0)
	{
		m_fAniElapsed = 0;
	}
} // Enter

void CUIButton5::Leave()
{
	if (!m_bEnable)
	{
		m_isEntering  = false;
		m_isLeaving   = false;
		return;
	}

	m_isEntering  = false;
	m_isLeaving   = true;
	m_fAniElapsed = 1 - m_fAniElapsed;

	if (m_fAniElapsed < 0)
	{
		m_fAniElapsed = 0;
	}
} // Leave



void CUIButton5::RenderText()
{
	if (m_font == 0)
	{
		return;
	}

	if (m_label.length() == 0)
	{
		return;
	}

	float x = GetDrawX();
	float y = GetDrawY();
	float w = GetWidth();
	float h = GetHeight();

	if (m_align & ALIGN_HCENTER)
	{
		x += w / 2;
	}
	else if (m_align & ALIGN_RIGHT)
	{
		x += w;
	}

	if (m_align & ALIGN_VCENTER)
	{
		y += h / 2;
	}
	else if (m_align & ALIGN_BOTTOM)
	{
		y += h;
	}

	if (m_isDown)
	{
		y += 1;
	}
	
	m_font->Render(m_txtOffsetX + x, m_txtOffsetY + y, m_txtAlign, m_label.c_str(), m_colText, m_colFrame);
} // RenderText