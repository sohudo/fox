#include "Stdafx.h"
#include "UIComponent.h"

CUIComponent::CUIComponent(int id)
  : m_hge(0)
  , m_x(0), m_y(0), m_width(0), m_height(0), m_hotX(0), m_hotY(0)
  , m_mX(0), m_mY(0), m_mWidth(0), m_mHeight(0)
  , m_bVisible(true)
  , m_bEnable(true)
  , m_bMouseEnable(true)
  , m_isDown(false)
  , m_isOver(false)
  , m_id(id)
  , m_ToolTips(0)
{
	m_hge = hgeCreate(HGE_VERSION);
} // CUIComponent

CUIComponent::~CUIComponent()
{
	if (m_hge)
	{
		m_hge->Release();
		m_hge = 0;
	}
} // ~CUIComponent

bool CUIComponent::Update()
{
	bool bDo = false;

	if (m_bMouseEnable && m_bVisible)
	{
		float x, y;
		m_hge->Input_GetMousePos(&x, &y);
		bool bLPressed  = m_hge->Input_KeyDown(HGEK_LBUTTON);
		bool bLReleased = m_hge->Input_KeyUp(HGEK_LBUTTON);
		bool bRPressed  = m_hge->Input_KeyDown(HGEK_RBUTTON);
		bool bRReleased = m_hge->Input_KeyUp(HGEK_RBUTTON);
		int nWheel      = m_hge->Input_GetMouseWheel();

		if (IsInside(x, y))
		{
			bDo = true;

			if (!m_isOver)
			{
				m_isOver = true;
				OnMouseOver(true, x, y);
				ToolTipsShow(x, y);
				dispatchEvent(EVT_UI_MOUSE_OVER, (unsigned int)this);
			}
			else
			{
				if (bLPressed || bRPressed)
				{
					OnMouseButton(true, x, y);
					dispatchEvent(EVT_UI_MOUSE_DOWN, (unsigned int)this);

					m_isDown = true;
				}
				else if (bLReleased || bRReleased)
				{
					if (m_isDown)
					{
						m_isDown = false;

						OnMouseButton(false, x, y);
						dispatchEvent(EVT_UI_MOUSE_UP, (unsigned int)this);
						dispatchEvent(EVT_UI_MOUSE_CLICK, (unsigned int)this);
					}
				}

				OnMouseMove(x, y);
				ToolTipsMove(x, y);
				dispatchEvent(EVT_UI_MOUSE_MOVE, (unsigned int)this);

				if (nWheel != 0)
				{
					OnMouseWheel(nWheel, x, y);
					dispatchEvent(EVT_UI_MOUSE_WHEEL, (unsigned int)this);
				}
			}
		}
		else
		{
			if (bLReleased || bRReleased)
			{
				m_isDown = false;
			}
		
			if (m_isOver)
			{
				ToolTipsHide();
				m_isOver = false;
				OnMouseOver(false, x, y);
				dispatchEvent(EVT_UI_MOUSE_OUT, (unsigned int)this);
			}
		}
	}
	else
	{
		ToolTipsHide();
		m_isDown = false;
		m_isOver = false;
	}

	if (m_bVisible)
	{
		OnUpdate();
	}

	return bDo;
} //Update

void CUIComponent::Render()
{
	if (m_bVisible)
	{
		OnRender();	
		ToolTipsRender();
	}
} // Render

void CUIComponent::SetPosition(float x, float y)
{
	m_x = x;
	m_y = y;
} // Render

void CUIComponent::SetSize(float width, float height)
{
	if (width != m_width || m_height != height)
	{
		m_width  = width;
		m_height = height;
		dispatchEvent(EVT_UI_RESIZE, (unsigned int)this);
	}
} // SetViewSize

void CUIComponent::SetHotSpot(float x, float y)
{
	m_hotX = x;
	m_hotY = y;
} // SetHotSpot

void CUIComponent::SetMouseRect(float x, float y, float width, float height)
{
	m_mX      = x;
	m_mY      = y;
	m_mWidth  = width;
	m_mHeight = height;
} //SetMouseRect

void CUIComponent::SetVisible(bool bVisible)
{
	m_bVisible = bVisible;

	if (!m_bVisible)
	{
		ToolTipsHide();
	}
} // SetVisible

void CUIComponent::SetEnable(bool bEnable)
{
	m_bEnable = bEnable;
} // SetEnable

void CUIComponent::SetEnableEx(bool bEnable, bool bMouseEanble)
{
	SetEnable(bEnable);
	SetMouseEnable(bMouseEanble);
} // SetEnableEx

void CUIComponent::SetMouseEnable(bool bMouseEnable)
{
	m_bMouseEnable = bMouseEnable;
} // SetMouseEnable

void CUIComponent::SetToolTips(IUIToolTips* toolTips, float fOffsetX, float fOffsetY)
{
	m_ToolTips			= toolTips;
	m_fToolTipsOffsetX	= fOffsetX;
	m_fToolTipsOffsetY	= fOffsetY;
} // SetToolTips

bool CUIComponent::IsVisible() const
{
	return m_bVisible;
} // IsVisible

int CUIComponent::Id() const
{
	return m_id;
} // Id

bool CUIComponent::IsEnable() const
{
	return m_bEnable;
} // IsEnable

bool CUIComponent::IsMouseEnable() const
{
	return m_bMouseEnable;
} // IsMouseEnable

float CUIComponent::GetX() const
{
	return m_x;
} // GetX

float CUIComponent::GetY() const
{
	return m_y;
} // GetY

float CUIComponent::GetWidth() const
{
	return m_width;
} // GetWidth

float CUIComponent::GetHeight() const
{
	return m_height;
} // GetHeight

float CUIComponent::GetDrawX() const
{
	return m_x - m_hotX;
} // GetDrawX

float CUIComponent::GetDrawY() const
{
	return m_y - m_hotY;
} // GetDrawY

bool CUIComponent::IsInside(float x, float y) const
{
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


float CUIComponent::GetToolTipsOffsetX() const
{
	return m_fToolTipsOffsetX;
} // GetToolTipsOffsetX

float CUIComponent::GetToolTipsOffsetY() const
{
	return m_fToolTipsOffsetY;
} // GetToolTipsOffsetY

void CUIComponent::ToolTipsShow(float x, float y)
{
	if (m_ToolTips == 0)
	{
		return;
	}

	m_ToolTips->Show(this, x, y);
} // ToolTipsShow

void CUIComponent::ToolTipsHide()
{
	if (m_ToolTips == 0 || !m_isOver)
	{
		return;
	}

	m_ToolTips->Hide(this);
} // ToolTipsHide

void CUIComponent::ToolTipsMove(float x, float y)
{
	if (m_ToolTips == 0)
	{
		return;
	}

	m_ToolTips->MouseMove(this, x, y);
} // ToolTipsMove

void CUIComponent::ToolTipsRender()
{
	if (m_isOver && m_ToolTips)
	{
		//m_ToolTips->Render(this);
	}
} // ToolTipsRender