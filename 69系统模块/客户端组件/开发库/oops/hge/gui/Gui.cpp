#include "Stdafx.h"
#include "Gui.h"
#include <algorithm>

using namespace std;

CGui::CGui()
{
	m_hge		= hgeCreate(HGE_VERSION);
	m_curCtrl	= NULL;
}

CGui::~CGui()
{
	removeAll();
	m_hge->Release();
	m_hge = NULL;
}

void CGui::addCtrl(CUIComponent* component)
{
	CUIComponent* old = m_curCtrl;
	removeCtrl(component);
	m_list.push_back(component);
	m_curCtrl = old;
} // addCtrl

void CGui::removeCtrl(CUIComponent* component)
{
	list<CUIComponent*>::iterator it = find(m_list.begin(), m_list.end(), component);
	if (it != m_list.end())
	{
		m_list.erase(it);
	}

	if (m_curCtrl == component)
	{
		m_curCtrl = NULL;
	}
} // removeCtrl

void CGui::removeAll()
{
	m_list.clear();
	m_curCtrl = NULL;
} // removeAll

void CGui::update()
{
	float x, y;
	m_hge->Input_GetMousePos(&x, &y);
	bool bLPressed  = m_hge->Input_KeyDown(HGEK_LBUTTON);
	bool bLReleased = m_hge->Input_KeyUp(HGEK_LBUTTON);
	bool bRPressed  = m_hge->Input_KeyDown(HGEK_RBUTTON);
	bool bRReleased = m_hge->Input_KeyUp(HGEK_RBUTTON);
	int nWheel      = m_hge->Input_GetMouseWheel();

	// 找出当前获取鼠标的控件
	list<CUIComponent*>::reverse_iterator itNext = m_list.rbegin();
	list<CUIComponent*>::reverse_iterator itCur;

	CUIComponent* pCurCom = NULL;
	CUIComponent* pTemp	  = NULL;

	for (; itNext != m_list.rend();)
	{
		itCur = itNext++;
		
		pTemp = *itCur;

		if (pTemp->m_bMouseEnable && pTemp->m_bVisible)
		{
			if (pTemp->IsInside(x, y))
			{
				pCurCom = pTemp;
				break;
			}
		}
	}

	// 检测控件
	if (pCurCom != m_curCtrl)
	{
		if (m_curCtrl != NULL)
		{
			if (bLReleased || bRReleased)
			{
				m_curCtrl->m_isDown = false;
			}

			if (m_curCtrl->m_isOver)
			{
				m_curCtrl->ToolTipsHide();
				m_curCtrl->m_isOver = false;
				m_curCtrl->OnMouseOver(false, x, y);
				m_curCtrl->dispatchEvent(EVT_UI_MOUSE_OUT, (unsigned int)m_curCtrl);
			}
		}

		m_curCtrl = pCurCom;
	}

	if (m_curCtrl != NULL)
	{
		if (!m_curCtrl->m_isOver)
		{
			m_curCtrl->m_isOver = true;
			m_curCtrl->OnMouseOver(true, x, y);
			m_curCtrl->ToolTipsShow(x, y);
			m_curCtrl->dispatchEvent(EVT_UI_MOUSE_OVER, (unsigned int)m_curCtrl);
		}
		else
		{
			if (bLPressed || bRPressed)
			{
				m_curCtrl->OnMouseButton(true, x, y);
				m_curCtrl->dispatchEvent(EVT_UI_MOUSE_DOWN, (unsigned int)m_curCtrl);

				m_curCtrl->m_isDown = true;
			}
			else if (bLReleased || bRReleased)
			{
				if (m_curCtrl->m_isDown)
				{
					m_curCtrl->m_isDown = false;

					m_curCtrl->OnMouseButton(false, x, y);
					m_curCtrl->dispatchEvent(EVT_UI_MOUSE_UP, (unsigned int)m_curCtrl);
					m_curCtrl->dispatchEvent(EVT_UI_MOUSE_CLICK, (unsigned int)m_curCtrl);
				}
			}

			m_curCtrl->OnMouseMove(x, y);
			m_curCtrl->ToolTipsMove(x, y);
			m_curCtrl->dispatchEvent(EVT_UI_MOUSE_MOVE, (unsigned int)m_curCtrl);

			if (nWheel != 0)
			{
				m_curCtrl->OnMouseWheel(nWheel, x, y);
				m_curCtrl->dispatchEvent(EVT_UI_MOUSE_WHEEL, (unsigned int)m_curCtrl);
			}
		}
	}


	// 更新
	list<CUIComponent*>::iterator it1 = m_list.begin();
	list<CUIComponent*>::iterator it2;
	for (; it1 != m_list.end();)
	{
		it2 = it1++;
		if ((*it2)->m_bVisible)
		{
			(*it2)->OnUpdate();
		}
	}
} // update

void CGui::render()
{
	list<CUIComponent*>::iterator itNext = m_list.begin();
	list<CUIComponent*>::iterator itCur;

	for (; itNext != m_list.end();)
	{
		itCur = itNext++;

		(*itCur)->Render();
	}
} // render
