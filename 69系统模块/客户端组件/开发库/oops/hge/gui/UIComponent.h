#ifndef _UIComponent_H_
#define _UIComponent_H_

#include "IUIToolTips.h"
//////////////////////////////////////////////////////////////////////////
// 事件
//////////////////////////////////////////////////////////////////////////
static const char* EVT_UI_RESIZE      = "uiResize";
static const char* EVT_UI_MOUSE_OVER  = "mouseOver";
static const char* EVT_UI_MOUSE_OUT   = "mouseOut";
static const char* EVT_UI_MOUSE_DOWN  = "mouseDown";
static const char* EVT_UI_MOUSE_UP    = "mouseUp";
static const char* EVT_UI_MOUSE_MOVE  = "mouseMove";
static const char* EVT_UI_MOUSE_WHEEL = "mouseWheel";
static const char* EVT_UI_MOUSE_CLICK = "mouseClick";

/**
 * GUI组件基类
 */
class CUIComponent
	: public CEventDispatcher
{
public:
	CUIComponent(int id);
	virtual ~CUIComponent() = 0;
public:
	bool Update();
	void Render();


	virtual void SetPosition(float x, float y);
	virtual void SetSize(float width, float height);
	virtual void SetHotSpot(float x, float y);
	virtual void SetMouseRect(float x, float y, float width, float height);
	virtual void SetVisible(bool bVisible);
	virtual void SetEnable(bool bEnable);
	virtual void SetEnableEx(bool bEnable, bool bMouseEnable);
	virtual void SetMouseEnable(bool bMouseEnable);
	virtual void SetToolTips(IUIToolTips* toolTips, float fOffsetX, float fOffsetY);
	
	int  Id() const;
	bool IsVisible() const;
	bool IsEnable() const;
	bool IsMouseEnable() const;
	float GetX() const;
	float GetY() const;
	float GetHotX() const;
	float GetHotY() const;
	float GetDrawX() const;
	float GetDrawY() const;
	float GetWidth() const;
	float GetHeight() const;

	float GetToolTipsOffsetX() const;
	float GetToolTipsOffsetY() const;	
protected:

	virtual bool IsInside(float x, float y) const;
	virtual void OnUpdate(){}
	virtual void OnRender(){}
	virtual void OnMouseOver(bool bOver, float x, float y){}
	virtual void OnMouseMove(float x, float y){}
	virtual void OnMouseWheel(int wheel, float x, float y){}
	virtual void OnMouseButton(bool bDown, float x, float y){}

private:
	void ToolTipsShow(float x, float y);
	void ToolTipsHide();
	void ToolTipsMove(float x, float y);
	void ToolTipsRender();
protected:
	CUIComponent(const CUIComponent &go);
	CUIComponent& operator=(const CUIComponent &go);
protected:
	HGE*  m_hge;

	// 组件状态
	bool m_bVisible;
	bool m_bEnable;
	bool m_bMouseEnable;
	bool m_isDown;
	bool m_isOver;
protected:
	int	m_id;
	// 视图区域
	float m_x;
	float m_y;
	float m_width;
	float m_height;
	float m_hotX;
	float m_hotY;
	// 鼠标响应区域(相对于视图区域)
	float m_mX;
	float m_mY;
	float m_mWidth;
	float m_mHeight;

	IUIToolTips*	m_ToolTips;
	float			m_fToolTipsOffsetX;
	float			m_fToolTipsOffsetY;
private:
	friend class CGui;
}; // CUIComponent
#endif // _UIComponent_H_